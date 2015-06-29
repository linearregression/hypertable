# Introduction #

Under some unexpected situations, e.g. after recovery with incomplete meta logs (probably due to [buggy underlying DFS](https://issues.apache.org/jira/browse/HADOOP-4379)), the METADATA table might become inconsistent with range server states. It'd be nice to have a maintenance tool (we'll call it _htck_) that checks the consistency of METADATA and tries to fix it.

## Usage ##

I decide to implement htck as a stand-alone tool instead of integrated into a master maintenance thread because it is simpler and more controllable. Administrators may run htck multiple times to make sure everything goes well or stop it half-way if the process takes too long. The command line usage of htck is:

```
   htck [-d] [-f]
```

By default, htck just checks the consistency of METADATA table, but doesn't try to fix it immediately. To do the real fix, an explicit command line switch '-f' must be used.

The check phase has two modes: fast scan (default) and deep scan (-d). The difference is, for every range deep scan inquires the corresponding range server to see if it is really there, which is much more time-consuming. See the following sections for more details.

# Design Details #

## Types of Common Inconsistencies ##

  1. Range is assigned to a range server, but the range server is unreachable<br />This is common since auto recovery is not implemented yet. When a range server crashes, all of its ranges goes into this state before the server's restarted
  1. Range is assigned to a range server in the METADATA, while the range server denies it<br />Possbile cause: fast recovery with incomplete meta log.
  1. Range is unassigned<br />Possible cause: range server crashes before a new range is successfully loaded and that the source range server metalog is incomplete.
  1. Overlapped Ranges<br />Possible cause: range server crashes during the split-shrunk phase
  1. Gap between Ranges<br />Couldn't think up a scenario that may cause this, I haven't ever met it either.

## Preparation ##

To find the actual reason of an inconsistency, the current information stored in the METADATA table is inadequate, e.g.:

  1. It is OK if new ranges came from a split are unassigned before the master gets notified. While it is not if the range are assigned by the master to a range server but the server fails to load it. It's hard to tell the difference of these two situations without knowing the state of the range i.e. NEW or ASSIGNED.
  1. It is hard to detect if a range is `lost' i.e. locates on a crashed server and needs recovery, if the crashed server is restarted and is alive again. The reason is due to the lack of generation information of range servers.

Thus I propose two new columns be added into the schema of METADATA table: state and generation.

Some possible states include: NEW, ASSIGNED and LOST (to be recovered), it'd be nice to define more states like READONLY and UNLOADED (HBase supports unloading a table completely from memory). The ASSIGNED state doesn't need to be explicitly specified as it is the default one.

The generation is a serial number that is strictly increasing. A range server must use a new generation number every time it restarts. For now I think It's OK to borrow the Hyperspace session id.

## Fast Scan ##

When doing a fast scan, htck should:

  1. Read a list of live range servers along with their generation numbers from Hyperspace
  1. Scan the METADATA row by row to see if the corresponding range's location & generation columns match a live range server.
  1. If the match fails, check the state column to see if the range is NEW, LOST or UNLOADED, if so save and report the inconsistency, otherwise skip and proceed.
  1. Check to see if the current range overlaps with last one.

In more details: to deal with LOST portions of the METADATA table. Step 2 should take place in stages, i.e. first check /hyperspace/root, then the ROOT table, then second-level METADATA.

## Deep Scan ##

Deep scan is the same as fast scan except that, for each range in the METADATA table, htck also inquires the corresponding range server to see if the range is there. This can be done by adding a new RPC method that returns true or false:

```
bool RangeServerClient::load_range(const sockaddr_in &addr, const TableIdentifier &table, const RangeSpec &range);
```

## Fix Phase ##

For inconsistency 1, as for now, print a message and let the admin restart the server for fast recovery. When auto recovery is implemented, treat it the same as inconsistency 2.

For inconsistency 2, mark the range LOST, then notify the master to choose a range server and load the range with proper commit logs replayed.

For inconsistency 3, mark the range NEW, then notify the master to choose a range server and load the range with the split log replayed.

For inconsistency 4 and 5, these are really rare cases, do not know how to fix yet...

The reason of letting the master give load\_range orders is that it should have more detailed information about live range servers and can do better load balance in the future, besides it seems safer to let the master monopolize the power of loading / unloading ranges.

To find the commit / split logs that needs to be replayed, the range server chosen to load the LOST / NEW range must know:
  1. The directory log files resides: can be made up from former location / end row, right?
  1. From when the logs should be replayed: the latest revision number of cell stores

## Potential Problems ##

A potential problem is that htck / master / range servers may read / update the METADATA table concurrently.

The solution is to let the Hypertable cluster go into read-only state while htck is running, which may e a future task. For now, if the admin is sure there's no updates to Hypertable during a time period, which is roughly the same as having a read-only state, it is also safe to run htck. This makes htck useful especially in cases that data is always loaded in batch during certain time periods (e.g. a crawler db that gets updated once every week).

Another problem is range servers may still crash while htck is running even if the cluster is read-only. This is actually a rare case. If it really happens, htck may detect it by subscribing changes to the "/hypertable/servers" hyperspace directory, and stop running with an error message printed. The admin may then deal with the situation and run htck again later.

## Summary of Changes ##

METADATA table schema:
  * Add two columns: state and generation.
  * Neither needs to be in-memory.

Range Server:
  * Modify the register-to-hyperspace process to set the generation attribute
  * Set generation along with location after loading a range.
  * Add an RPC method: check\_range

Master:
  * Add an RPC method that loads a LOST range and replays proper commit logs.

# Future Work #

As range state is added to the METADATA, we may add more features to Hypertable in the future, for example make a table READONLY or unload it from memory. These features are useful if we need to do snapshots of tables in Hypertable.

Client table scanner / mutator can also make use of the range state information while handling errors. For instance, if a scanner fails to look up a key in a specific range, it may then look at the state column of that range to see whether the range is NEW (most likely to be the result of a recent split), LOST or UNLOADED, and apply different policies, e.g. wait and retry on the NEW and LOST state, and fail directly if the range is UNLOADED.

Luke suggested an approach to do htck offline:

> I thought about the problem a little. It seems to me that the cleanest approach
> would be doing the htck while hypertable (except dfs brokers) is offline. You can
> basically do a merge scan of metadata cellstores and metadata commit logs to construct
> the complete metadata and compare with all the metalogs. You can even reclaim lost
> ranges (by looking at gaps in the metadata) that are neither in metadata or metalogs
> but still in cellstores this way, just like fsck does for lost blocks. You can fix
> problems by (re)writing to commit logs/metalogs, so when hypertable restarts, the
> recovery process will take care of all the issues.

> This requires some refactoring of the range server code. Much of the range server
> functionality is already in libHyperRanger.a/so anyway, which is used by csdump. I
> think this is a better approach (not to mention more challenging and exciting :)
> that does not suffer all kinds of chicken and egg problems from the live scans.

Clearly this approach is safer and cleaner. It can even fix catastrophic problems that prevent range servers from restarting. The only drawback is all range servers must be shutdown, which introduces relatively long down time.