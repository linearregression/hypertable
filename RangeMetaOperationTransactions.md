
## Introduction ##

Ranges are assigned and re-assigned to RangeServers by the following operations:

  * CREATE TABLE
  * Range split
  * Range migration

To ensure that each Range get assigned to a RangeServer and that a Range doesn't accidentally get assigned to multiple RangeServers, even in the face of failures, we've decided to implement a specialized form of range meta-operation transactions.  This document describes the design of this mechanism.

To support these range meta-operation "transactions", we've introduced two new commit logs:

  1. RSML - RangeServer Meta Log
  1. MML - Master Meta Log

These logs are where all Range meta operations get recorded for the RangeServers and Master, respectively.


---


## Load Range ##

### Master ###

  1. Writes M\_LOAD\_RANGE\_START (with location of receiving RangeServer) entry to MML
  1. Calls `load_range(range_state)` on receiving RangeServer
  1. Update METADATA 'Location' column for the new range
  1. Write M\_LOAD\_RANGE\_DONE entry to MML

### RangeServer ###

  1. Append RS\_RANGE\_LOADED in RSML
  1. Create Range directory in DFS
  1. Link transfer log into primary log
  1. Replay transfer log
  1. Add Range object to main map
  1. Respond OK to Master


---


## Range Split ##

### Originating RangeServer ###

  1. Decide to split range
  1. Create a transfer log and append RS\_SPLIT\_START entry to RSML (include transfer log location)
  1. Do major compaction
  1. Append RS\_SPLIT\_SHRUNK to RSML (with identifier of split-off Range)
  1. Update METADATA for old range
  1. Create METADATA entry for new range with 'StartRow' and 'Files' columns
  1. Quiesce updates
  1. Close transfer log
  1. Shrink access groups
  1. Un-quiesce updates
  1. Notify Master of split
  1. Upon receiving Error::OK response from Master write RS\_SPLIT\_DONE entry in RSML

### Master ###

  1. Master::report\_split() called by originating RangeServer
  1. Selects a RangeServer for the newly split off range
  1. Writes M\_LOAD\_RANGE\_START (with location of receiving RangeServer) entry to MML
  1. Returns OK response for Master::report\_split() back to originating RangeServer
  1. Calls `load_range(range_state)` on receiving RangeServer
  1. Update METADATA 'Location' column for the new range
  1. Issue drop\_range() for the new range to the originating RangeServer
  1. Write M\_LOAD\_RANGE\_DONE entry to MML

### Receiving RangeServer ###

  1. Create Range directory in DFS
  1. Link transfer log into primary log
  1. Append RS\_RANGE\_LOADED in RSML
  1. Replay transfer log
  1. Add Range object to main map
  1. Respond OK to Master


---


## Create Table ##

  1. Selects a RangeServer for the initial range
  1. Writes M\_LOAD\_RANGE\_START (with location of receiving RangeServer) to MML
  1. Update METADATA 'Location' column for range
  1. Calls load\_range() on receiving RangeServer
  1. Upon receipt of Error::OK, write M\_LOAD\_RANGE\_DONE entry to MML


---


## RangeServer Restart (Fast Recovery) ##

When a RangeServer comes up, it will acquire a lock on a well-known file in Hyperspace with the following name format:

`/hypertable/servers/<ip-address>_<port>`

It holds on to this lock the entire time it is up.  If it is unable to acquire the lock it will go into a retry loop.  Once it successfully acquires the lock, it will then check to see if there is an RSML for this machine/port combo in the DFS.  This log will be found in the following well-known location:

`/hypertable/servers/<ip-address>_<port>/log/meta`

If the RSML does not exist, the RangeServer will just come up and wait for the Master to assign it ranges.  Otherwise, if the RSML does exist, it will carry out the following recovery procedure:

  1. Scan the RSML to figure out the state of all of the Ranges that the prior RangeServer had loaded
  1. Load all of the Ranges
  1. Replay the primary commit log
  1. For Ranges in the RS\_SPLIT\_START state, replay the transfer log and initiate a split, appending to the existing transfer log.
  1. For Ranges in the RS\_SPLIT\_SHRUNK state, initiate a report\_split\_to\_master() task


---


## RangeServer Recovery - Master ##

When the Master discovers that a RangeServer has died (via Hyperspace notification), it waits for some grace period and then attempts to acquire a lock on a well-known file in Hyperspace with the following name format:

`/hypertable/servers/<ip-address>_<port>`

where the ip-address and port correspond to the RangeServer that died.  If it is unable to acquire the lock, then it assumes that a RangeServer has been restarted on the same IP address and port and is handling recovery.  If it is able to acquire the lock, then it carries out the following sequence of steps:
  1. Write a M\_RECOVERY\_START entry in the MML that contains the dead RangeServer)
  1. Rename the dead range server commit log directory in DFS to '.../commit-recover'
  1. Read the RSML of the deceased RangeServer to reconstruct the state of all the Ranges it had loaded
  1. Find new locations (RangeServers) for all the ranges.
  1. Write new locations in the METADATA.
  1. For ranges that were in the RS\_RANGE\_LOADED state, write M\_LOAD\_RANGE\_START for the range and its target range server in MML and issue a corresponding load\_range()
  1. For ranges that were in the RS\_SPLIT\_START state, write M\_LOAD\_RANGE\_START for the range and its target range server in MML and issues a load\_range(), but set a flag indicating a split was in progress and pass along the location of the transfer log.
  1. For ranges that were in the RS\_SPLIT\_SHRUNK state, finish the split as follows
    1. For the original range that was shrunk, select a RangeServer and write M\_LOAD\_RANGE\_START for the range and its new range server in MML and issue a load\_range()
    1. For the newly split-off range, got to step #2 of Range Split -> Master above
  1. For each successful load\_range(), write M\_LOAD\_RANGE\_DONE for the range in MML
  1. When all load\_range() succeeded, delete the commit-recovery directory for the dead range server and write M\_RECOVERY\_DONE for the dead range server in MML


---


## Master Startup ##

  1. Read MML and reconstruct state
  1. For all ranges that were in M\_LOAD\_RANGE\_START or M\_RECOVERY\_START but not M\_LOAD\_RANGE\_DONE mode, follow the above 'Range server recovery - Master' from step 4.
  1. For all ranges that were in M\_MOVE\_RANGE\_START mode, follow the below 'Range Move - Master' from step 3.


---


## Range Move ##

### Master ###
  1. Select source range and destination range server
  1. Write M\_MOVE\_RANGE\_START with the range and its source and destination servers in MML
  1. Issue prepare\_range\_move() on the source range server
  1. Write M\_LOAD\_RANGE\_START with the range and its destination in MML and issue a corresponding load\_range() with transfer log location
  1. Update METADATA 'Location' for the range
  1. Write M\_LOAD\_RANGE\_DONE with the range in MML
  1. Issue drop\_range() on the source range server
  1. Write M\_MOVE\_RANGE\_DONE with the range in MML

### Range Server ###
  1. prepare\_range\_move() called by Master
  1. Create a transfer log and append RS\_MOVE\_START with the log location in RSML
  1. Do major compaction for the range
  1. Quiesce updates to the range
  1. Close transfer log
  1. Append RS\_MOVE\_PREPARED for the range to RSML


---


## Implementation TODO Tasks ##

  * Change RangeServer split logic as described above in Range Split -> Originating RangeServer above
  * Allow load\_range() on already loaded ranges.  If the Master dies in the middle of recovery, when the next Master takes over it may ask a RangeServer to load a range that it already successfully loaded.  I should return an error like RANGE\_ALREADY\_LOADED.
  * ...