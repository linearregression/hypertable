# Introduction #

AccessGroups in Hypertable consist of a CellCache (memtable) and some number of CellStores (sstables).  Occasionally, these data structures undergo "compaction" whereby either the CellCache or the CellCache and some/all of the CellStores get merged together and written out to the filesystem as a new CellStore.  The following situations cause compactions:

  * Commit (Edit) Log needs pruning
  * Low memory
  * Range split or move
  * Garbage Collection (TTL & MAX\_VERSIONS)

This document describes the compaction algorithm.

# Algorithm #

## Commit (Edit) Log needs pruning ##

The RangeServers have a maintenance timer that schedules maintenance tasks every 30 seconds (default).  Tasks (which include compactions, splits, memory purge, etc.) are put onto a maintenance queue and are carried out by maintenance threads.

The default maintenance prioritization scheme is called LogCleanup.  This scheme tries to keep the commit log pruned back to a reasonable size so that in the event of a failover, replaying the commit log will be quick.  The way it keeps the commit log pruned back is by compacting Ranges (AccessGroups) that have in-memory cells that are recorded in old commit log fragments.  Once all of the cells in a commit log fragment have been compacted to CellStores, the fragment may be safely removed.

The target commit log size varies (from about 200MB to 4GB) depending on the current workload.  Under heavy loading activity, the commit log is allowed to grow larger.

## Low memory ##

When the RangeServer memory usage hits the limit specified by the `Hypertable.RangeServer.MemoryLimit` or `Hypertable.RangeServer.MemoryLimit.Percentage` properties (default is 60% of physical RAM), then the system goes into LowMemory mode.  In this mode, the system will schedule compactions for the largest CellCaches until the memory drops to 10% (default) below the configured memory limit.

## Range split or move ##

Immediately prior to a range split or move, all of the access groups in the range will undergo a compaction.  This compaction removes dependence on the commit log.  Prior to the compaction a "transfer log" gets installed to persist updates that happen during the split/move operation.  The transfer log will get replayed by the receiving range server.

## Garbage Collection (TTL & MAX\_VERSIONS) ##

Once every 30 seconds, for each AccessGroup for each range, a check is performed to see how much data has accumulated since the last "GC compaction".  If enough data has accumulated in the access group (default is Range.SplitSize/10 or 25MB) then a "GC compaction" is scheduled.

When the GC compaction is performed by one of the maintenance threads, it first measures how much actual garbage is in the AccessGroup by doing a merge scan of the CellCache and all of the live CellStores.

If the amount of garbage exceeds a threshold (defined by the `Hypertable.RangeServer.AccessGroup.GarbageThreshold.Percentage` property which defaults to 20%), then a major compaction is performed, which will merge the cell cache and all of the cell stores into one large CellStore.