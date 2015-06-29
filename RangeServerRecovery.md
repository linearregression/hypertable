
## Introduction ##

When a Range Server goes down, all of the ranges that it manages need to get re-assigned to other range servers.  This is performed by the Master.  This document describes the process.

## Master Recovery Procedure ##

  * Master gets notified of RangeServer failure via Hyperspace or by scanning METADATA and the /hypertable/servers Hyperspace directory upon obtaining the lock on the /hypertable/master Hyperspace file (e.g. it is taking over master responsibility)

  * Master will create a TEMP file in Hyperspace with the name /hypertable/recovery/_recovery-id_
    * The _recovery-id_ is obtained from the 'last\_recovery\_id' attribute of the /hypertable/master Hyperspace file which is atomically incremented by the master each time a recovery starts
    * For each range being recovered, the master will write a special value into the 'Location' column of the METADATA entry for the range that indicates the _recovery-id_
    * If the root range is being recovered, then the 'Location' attribute of the /metadata/root file in Hyperspace is updated to contain the special value that indicates the _recovery-id_

  * Master creates a ReplayScatterBuffer and loads it with the range information for all of the ranges being recovered, including their new location.

  * Master invokes `ReplayScatterBuffer::start_replay()` which will invoke the `RangeServer::start_replay()` method of all RangeServers participating in the recovery.

  * Master re-assigns all of the Ranges owned by the failed RangeServer to new RangeServers.  It does this by invoking the `RangeServer::load_range()` method with the REPLAY bit set in the flags argument, for each Range being recovered.

  * Master replays the commit logs (first the primary, then the split log if it exists) by doing the following for each log entry:
    * Reads the _timestamp_ and _table name_ from the header of the log entry
    * Replays the commit log entry by invoking the `ReplayScatterBuffer::replay()` method with the commit timestamp, the table information, and the block of updates.
    * The ReplayScatterBuffer will periodically flush these replayed commits to their respective RangeServers via a call to `RangeServer::update_replay()`.
      * _For the initial implementation, we'll let the (potentially new) master rescan the commit log in case of any failures (master of range server) during the recovery._
      * _The paper, talks about using distributed sort to speed up the recovery._
      * _An even better way seems to be a distributed replay scheduled on log replicas._

  * After log has been completely replayed, the Master invokes the `ReplayScatterBuffer::commit_replay()` method which has the effect of invoking the `RangeServer::commit_replay()` method of all RangeServers participating in the recovery.

  * If the `ReplayScatterBuffer::commit_replay()` returned success:
    * the Master deletes the obsolete commit log (all the files and directories in DFS, as well as those of its linked entries from splits or previous recoveries).
    * Master deletes the Hyperspace file /hypertable/recovery/_recovery-id_

  * If the `ReplayScatterBuffer::commit_replay()` fails, or if the Master successfully obtains the lock on the /hypertable/master Hyperspace file (e.g. it is taking over master responsibility), the it will do the following:
    * Determine all live Range Servers by scanning the /hypertable/servers Hyperspace directory
    * Scan the 'Location' column of METADATA (and read 'Location' attribute of /hypertable/root) to discover any ranges that are not assigned to any live server
    * For each unassigned range, it will restart the recovery process

## Range Server Recovery Procedure ##

  * Master must recover the RangeServer that held the METADATA

  * When normal updates are received by a range server, they get persisted into a commit log.  This group of updates gets written as a single entry that begins with a fixed header.  The header includes the real timestamp (i.e. real time sampled by the server prior to the commit) of the commit.

  * When an Access Group is compacted, the real time of the compaction is written into the CellStore trailer.  The most recent timestamp from all of the CellStore trailers is taken to be the _last\_compaction\_time_ for the Access Group.

  * When the `start_replay()` method is invoked, the Range Server will:
    * Delete all of the objects in the "replay" map and clear the map
    * Remove any existing "replay" commit log
    * Create a new "replay" commit log

  * When recovering a range, the Master will invoke the `load_range()` method of the Range Server with the REPLAY bit set in the flags argument.  The RangeServer will do the following when this happens:
    * Create the Range object which will load all of the CellStores and determine the _last\_compaction\_time_ for each Access Group.
    * Puts the Range object into the "replay" map

  * When the Master replays the commit log from the failed RangeServer, it will read the real timestamp of the commit log entry and send it, along with the updates, to each RangeServer receiving a recovered Range.  It does this by invoking the `RangeServer::update_replay()` method

  * For each call to `update_replay()`, the RangeServer will commit the updates to the "replay" commit log and then call a special version of `Range::add()` that will ignore any updates to an Access Group that have a timestamp that is less than the _last\_compaction\_time_ for the Access Group.
    * _It probably also needs to ignore (instead of errors) updates with older timestamps than what's already in the replay map as the replay of the commit log could be restarted by a new master._

  * After the Master has replayed the entire commit log, it will invoke the `RangeServer::commit_replay()` method of all the Range Servers participating in the recovery.  Each RangeServer will atomically move all of its "replay" ranges into the the main Range map and will link the "replay" commit log into the main commit log.

  * The participating Range Servers write a new entry for each recovered range in METADATA table for lookup.

## Hypertable Client Recovery Procedure ##

  * If an i/o error occurs that's due to a missing range server, the RangeLocator of Client would get the _recovery id_ from location of the range in the metadata or hyperspace (if the recovering range is root). The _recovery id_ can then be used to register a callback in the hyperspace.
  * User of the API would have options to either wait (block) for the recovery to be finished, when no callback object is supplied, or a callback method is invoked with a reference of the current transaction object as an argument. User can then decide whether to retry, abort or rollback (probably not supported in 1.0) the transaction.