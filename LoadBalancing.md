

# Introduction #

The purpose of load balancing is to spread load across the range servers to maximize overall performance and availability.

# Balancer Mechanics #

The Master will include a load balancer implemented as a class derived from an abstract base class (LoadBalancer) so that alternate balancers can be easily added.  The LoadBalancer class will have a `balance()` method that gets called by the Master maintenance task which runs periodically (default = 30 seconds).  If new range servers are added or the load variance among existing servers gets too large, it will commence a balance operation.  This section describes the basic mechanics of how the system carries out a balance operation.

## Balance Operation ##

The balance operation will consist of the following steps:

  1. The LoadBalancer will set a flag indicating that a balance operation is progress.  This will prevent the balancer from commencing another balance operation while another one is in progress.
  1. A BALANCE\_START MML (Master Meta Log) entry will get written that includes all of the information necessary to carry out the rebalance operation (each range being moved and the source and destination servers).
  1. For each range being moved, the balancer will call the asynchronous `RangeServer::relinquish_range()` method.  The balancer may do this over some time period so that it doesn't overwhelm the system with move operations.
  1. When each range is relinquished, the master will be notified via it's `Master::move_range()` method which will first check the move-started cache to determine which server it is assigned to.  If the range is not in the move-started cache, it will ask the balancer to assign it a range, write a RANGE\_MOVE\_START MML entry (with info about the assigned range server), insert the range into the move-started cache, and respond success to the relinquishing range server.  It will then invoke the `RangeServer::load_range()` method on the target range server.  Upon successful completion it will write a RANGE\_MOVE\_LOADED MML entry, remove the range from the move-started cache, and inform the balancer that the range has been loaded.
  1. When the RangeServer writes the SPLIT\_DONE or RELINQUISH\_DONE entry in it's log, it will invoke the `Master::relinquish_acknowledge()` method of the Master which will write a RANGE\_MOVE\_ACKNOWLEDGED entry in the MML and inform the LoadBalancer that the range move has been acknowledged.  Un-acknowledged ranges will be excluded from future balance operations.
  1. Once the last range has been loaded, the balancer will write a BALANCE\_COMPLETE MML entry, unset the flag indicating that a balance is in progress, and continue normal operation.

## Master Failover ##

When the Master fails over during a balance operation, it will recover its state with the following steps.

  1. Master will re-play the MML and will initialize the LoadBalancer with unclosed BALANCE\_START information as well as any ranges which are not part of the in-progress balance operation but are in the RANGE\_MOVE\_START or unacknowledged RANGE\_MOVE\_LOADED state.
  1. LoadBalancer will record unacknowledged RANGE\_MOVE\_LOADED ranges in an exclude map to prevent them from being considered for subsequent balance operations
  1. All ranges in the RANGE\_MOVE\_START state will get added to the move-started cache.
  1. LoadBalancer set a flag indicating that a balance operation is in progress and will signal to decrease the maintenance interval to 1 second
  1. Master will start maintenance thread
  1. As soon as each participating range server re-connects, the LoadBalancer will handle each range as follows:
    * For ranges not in RANGE\_MOVE\_START state, it will invoke `RangeServer::relinquish_range()`  If this returns RANGE\_NOT\_FOUND, it will assume the move operation was completed for this range.
    * For ranges in the RANGE\_MOVE\_START state it will create a MoveRange task and enqueue it onto the Master's application queue.
  1. Once all of the necessary servers have re-connected the LoadBalancer will signal to increase the maintenance interval back to its normal value and the balance operation will continue as normal.

## RangeServer Failure ##

If a range server participating in a balance operation fails and a recovery operation commences for that range server, the LoadBalancer will be notified and will handle uncompleted ranges as follows:

  * For ranges not yet in the RANGE\_MOVE\_START state, it will assume the move operation was completed for this range and mark it completed.
  * For those ranges in the RANGE\_MOVE\_START state it will consult the recovery process to determine if the range was loaded or not.  If the range was loaded, it will write a RANGE\_MOVE\_LOADED MML entry for the range.  Otherwise, it will assign the range a new location and write a RANGE\_MOVE\_RESTARTED MML entry with the old location and the new one.  This will effectively be the same state as RANGE\_MOVE\_STARTED with the new location.  It will then create a MoveRange task and enqueue it onto the Master's application queue.

# Balancer Input #

## `sys/RS_METRICS` Table ##

This will be a system table used to track load statistics for servers and ranges.  Load statistics will be computed and stored hourly.  To minimize the amount of data kept in this table, 24 samples will be kept for each range and 7\*24 (one week) samples will be kept for each range server.  This will be achieved with the MAX\_VERSIONS option of the column specifications.  The reason we use MAX\_VERSIONS instead of TTL is so that when a system is shut down for long periods of time, it can be brought back up again and still have load statistics to work with.  The table will have the following row key format:
```
<server-id>[:<table-id>]
```
The reason for this row key format is so that when a table is dropped, it can be easily deleted from `sys/RS_METRICS`.  One problem to consider when deciding on a row key format is row overflow.  Let's assume that the system is managing a single very large table, each range server has 10,000 ranges, and the row key and value data for each sample is 1000.  With 24 samples, this puts each row in the `sys/RS_METRICS` table at about 10000\*1000\*24, or 240MB.  This is less than the 256MB max range size and considerably less with compression factored in.  The `sys/RS_METRICS` table will have the following schema:
```
use sys;
create table RS_METRICS (
  server MAX_VERSIONS=168,
  range MAX_VERSIONS=24,
  range_start_row MAX_VERSIONS=1,
  range_move MAX_VERSIONS=1,
  ACCESS GROUP server ( server ),
  ACCESS GROUP range ( range, range_start_row, range_move )
);
```

The columns in the range access group will have a qualifier that represents the end row of the range.

**RangeServer failure**

System ranges (e.g. from `sys/METADATA`, `sys/RS_METRICS`, etc.) will need to be recovered first.  The Master will maintain range server load information and will use that information to select lightly loaded servers to move the system ranges to.  If it does not have this information (e.g. the Master was recently restarted), it will randomly assign the system ranges.  The balancing operation cannot proceed until all of the system ranges have been recovered.

## Monitoring Data ##

Every 30 seconds the Master gathers monitoring statistics from all RangeServers.  This data will be delivered to the balancer in the form of a vector of per-range server statistics.  The data for each range server includes the following general server statistics and per-table statistics which may be useful for balancing:

**general server statistics:**
  * Amount of RAM
  * Number of CPU cores
  * Disk capacity and
  * range count

**per-table statistics:**
  * range count

# Load Balancer Class Definition #

The following is pseudo-code for the LoadBalancer base class.
```
class LoadBalancer {
  public:
    LoadBalancer(vector<RangeState> &range_states) = 0;
    LoadBalancer(vector<RangeState> &range_states, BalanceOperation &operation) = 0;
    void receive_monitoring_data(vector<RangeServerStatistics> &stats) = 0;
    void balance() = 0;
    String assign_to_server(TableIdentifier &tid, RangeIdentifier &rid) = 0;
    void range_move_loaded(TableIdentifier &tid, RangeIdentifier &rid) = 0;
    void range_relinquish_acknowledged(TableIdentifier &tid, RangeIdentifier &rid) = 0;
    time_t maintenance_interval() = 0;
};
```

# Basic Balancing Algorithm #

The Basic balancer will attempt to optimize for both load balance as well as even distribution of ranges for each table across the range servers.  There are three situations that will prompt a balancing operation.  The following sections describe each of these situations, the policy for triggering a balance operation, and the algorithm for coming up with a balance plan.

## Load Imbalance ##

Balance operations for load imbalances will happen once per day.  The balancer will initialize itself by scanning the weekly server load data to determine the best time of day to perform a balance operation and will schedule a balance operation for that time.  When the time arrives, it will consider performing a balance using server load data for the previous 24 hours only.  It will scan the `sys/RS_METRICS` table to build a map of server load average (load map) for the past 24 hours and use this information along with a configured load deviation threshold to determine if a balance operation is necessary.  Whenever a server or servers are added, it will wait for at least 24 hours before doing a "load imbalance" balance operation to give the servers time to build up a day's worth of load statistics.  The load deviation threshold will be set at a large enough value to account for variance due to range split operations.  If a balance operation is necessary, the following algorithm will be used to come up with a balance plan.

A vector of load average statistics (server\_load\_vec) will be created by scanning the `sys/RS_METRICS` table.  Each position in the vector will hold the following information for a range server:

  * loadavg
  * deviation
  * server\_name

`loadavg` is the measured load average over the past 24 hours.  `deviation` is a measure of how much deviation is in the server's loadavg number, computed by subtracting the loadavg mean across all the servers from the individual server's loadavg value.  The algorithm will try to reduce the load for one server at a time.  When processing a server, it will create a vector of range load statistics (range\_load\_vec) by scanning the `sys/RS_METRICS` table.  Ranges that have moved within the last week will be skipped.  Each position in the vector will hold the following information for a range:

  * loadestimate
  * start\_row
  * end\_row

`loadestimate` is a metric for load that is a function of number of scans/updates per second, cells read/written per second, bytes read/written per second.  It will also compute a `loadavg_per_loadestimate` factor for the server as follows:
```
server loadavg / sum(loadestimate) over all ranges
```

The following is pseudo code for coming up with the balance plan.
```
server_load_vec_desc = sort_descending(server_load_vec);
server_load_vec_asc = sort_ascending(server_load_vec);
while (server_load_vec_desc[0].deviation > DEVIATION_THRESHOLD) {
  populate_range_load_vector(server_load_vec_desc[0].server_name);
  sort descending range_load_vec;
  i=0;
  while (server_load_vec_desc[0].deviation > DEVIATION_THRESHOLD &&
            i < range_load_vec.size()) {
    if (moving range_load_vec[i] from server_load_vec_desc[0] to server_load_vec_asc[0] reduces deviation) {
       add range_load_vec[i] to balance plan
       partial_deviation = range_load_vec[i].loadestimate * loadavg_per_loadestimate;
       server_load_vec_desc[0].loadavg -= partial_deviation;
       server_load_vec_desc[0].deviation -= partial_deviation;
       server_load_vec_asc[0].loadavg += partial_deviation;
       server_load_vec_asc[0].deviation += partial_deviation;
       server_load_vec_asc = sort_ascending(server_load_vec_asc); 
    }
    i++;
  }
  if (i == range_load_vec.size())
    remove server_load_vec_desc[0] and corresponding entry in server_load_vec_asc  
  server_load_vec_desc = sort_descending(server_load_vec_desc);
}
```

## RangeServer Added ##

Whenever a new range server or range servers are added to the system, the balancer will immediately schedule a balance operation to move ranges from the existing servers to the newly added servers.  To come up with a balance plan, the algorithm described in [LoadBalancing#Load\_Imbalance](LoadBalancing#Load_Imbalance.md) will be used with one slight modification.  Instead of moving the worst load offenders from the existing servers, the algorithm will randomly select ranges from the servers so that an even balance of ranges with heavy and light load will be moved to the newly added servers.

## Range Split ##

When the `Master::move_range()` method is invoked due to a range split operation, the balancer will assign it to a new range server.  Since split operations can happen frequently throughout the day and have the effect of throwing off the load average statistics, a simpler approach will be used to assign the range to a server by solely optimizing for even range distribution for the table of the range being split.  The balancer will use the monitoring information it receives on a regular basis to build up a range count map for the table being split.  This map will contain range count information for each range server in the system.  The server containing the smallest number of ranges for the table of the range being split will be chosen to receive the range.