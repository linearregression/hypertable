# Introduction #

This document describes a design for cluster replication. With this feature, every update that is sent to a cluster is automatically forwarded asynchronously to a remote cluster. This cluster can be located in a different data center.

# Features #

  * Uni-directional or bi-directional replication
  * Replication is asynchronous (there can be slight delays)
  * Very little administration required
  * Replication is per table, not per column or per cluster
  * Replication can be activated with CREATE TABLE, ALTER TABLE
  * Replication can be deactivated with ALTER TABLE
  * Replication will continue to work if Hypertable was shutdown

# Terminology #

  * _RMaster_ ("Replication Master"): One process per cluster which orchestrates replication and tracking state
  * _RSlave_ ("Replication Slave"): Relatively dumb processes (many per-cluster) responsible for sending/receiving replication data to/from remote clusters
  * _Sender_: The role (of a Replication Slave) of sending replication data to remote clusters
  * _Receiver_: The role (of a Replication Slave) of receiving replication data from remote clusters
  * _Cluster ID_: a unique 64bit cluster ID

_Sender_ and _Receiver_ are implemented and deployed as a single binary executable.

# User Interface #

When initializing, the Master creates a unique ID for the cluster and stores it in hyperspace, unless the ID was already created. The ID is a 64bit number created from the master's hostname + timestamp.

In hypertable.cfg, these IDs can then be mapped to friendly names, i.e. "eastcoast01-primary". Also, hypertable.cfg will contain the IP-addresses/hostnames of the remote cluster's _RMaster_ process (or processes). These remote _RMasters_ will provide the _Sender_ with the address of the _Receiver_. Multiple _RMasters_ can be specified.

The CREATE TABLE statement is modified; it supports a new option.
```
    CREATE TABLE t (foo) REPLICATE TO <cluster-id>
```
The ALTER TABLE statement is modified:
```
    ALTER TABLE t REPLICATE TO <cluster-id> [ON]
    ALTER TABLE t REPLICATE TO <cluster-id> OFF
```

In all cases, the "friendly name" can be used instead of the cluster-id.

ALTER TABLE/CREATE TABLE modifications are automatically forwarded to the remote cluster if the table is replicated.

It is not allowed to enable replication for index tables or tables in the reserved "/sys" namespace.

The replication processes (_RMasters_, _RSlaves_) can be started with a startup script or via Capistrano.

# Bootstrapping #

When activating replication for an existing table, the existing cells have to be copied manually. This can be done via HQL commands (DUMP TABLE/LOAD DATA). We will document this accordingly.

During bootstrapping, the tables on the remote cluster have to be created manually.

# Implementation #

## Existing Hypertable servers ##

There will be only few changes necessary for the existing Hypertable Master and RangeServer.

  * The Master creates a unique Cluster ID when starting
  * The Master stores copies of the table Schemas in the DFS whenever the Table is created or modified; this also serves as a Hyperspace backup
  * The RangeServer does not delete unused CommitLog fragments, because they might not have been fully replicated yet. Instead, it renames them to ".deleted". The _RMaster_ will then delete these files.

### Replication MetaLog ###

There is one global replication metalog file (repml) for each cluster. It is stored in /hypertable/servers/replicator/log/repml.
The repml contains a list of fragments that are currently processed by the _Senders_.

### Schema Shipping ###

When the user enables replication for a new table (either via CREATE TABLE or ALTER TABLE) or when a table is modified (i.e. a new Column is added) then the local Hypertable Master writes the Schema XML to this table's directory in the DFS. (This also allows us to keep a Schema backup in case there is a problem with Hyperspace).

From there the local _RMaster_ can pick up the new information and send it to the remote _RMaster_. Afterwards, it will assign the new fragments of this table to the _Senders_ which will then start replicating these fragments. If replication of an existing table was enabled via ALTER TABLE then only new updates are replicated. Records that were updated before ALTER TABLE will not be replicated.

## Replication Master ##

Upon initialization, the _RMaster_ connects to the remote _RMaster_ to retrieve a list of all remote _RSlave_ processes. Periodically the _RMaster_ will refresh this list.

Then the _RMaster_ scans the DFS for the Schemas of all tables to figure out which of them is replicated. Then it starts scanning the DFS for log fragments and assigns the fragments to the _RSlaves_. Periodically the _RMaster_ will continue scanning for new fragments and assign them to one of the _RSlaves_.

The Master will try to bias the replication assignments to those _Senders_ that run on the same machine as the RangeServer that they are replicating; i.e. if a fragment belongs to rs3, then it will be assigned to a replication _Sender_ which runs on the same machine as rs3 (if possible).

The Master stores its state in a repml file (a metalog file in the DFS, see below). Periodically, the _Senders_ send their current state to the Master which will then update the repml (and also can update the Monitor to display the replication state).

### Replication Slave: Sender ###

Upon initialization, the _Sender_ connects to the _RMaster_, which will return information about the remote _Receivers_ and about the fragments that this _Sender_ should replicate.

After loading a fragment, the _Sender_ will only start sending updates which are created AFTER the _Sender_ was started. All older updates have to be sent during the bootstrapping process, which was described above.

Periodically, the _Sender_ will send its current state to its _RMaster_, which will store this state in the repml file. The _Sender_ does not store any other state.

The _Sender_ reads all fragments and sends all updates if there's a reasonable amount of new data (but the _Sender_ will only wait for a user-defined time, to avoid that the replication lags too far behind).

If the _Sender_ crashes and is restarted then it will get its current state re-assigned from the Master. Since the updates are idempotent it is not a problem if parts of a fragment are sent twice. Therefore the updates to the repml file do not have to be very frequent. If the _Sender_ is not restarted then the Master will assign its fragments to another _Sender_.

In order to make sure that merged/compacted fragments are not deleted before they are replicated, the RangeServer no longer deletes any fragments in CommitLog::purge. Instead, it renames them to "fragment-id.deleted". The _RMaster_ will delete all files with extension ".deleted" after it replicated them.

### Replication Slave: Receiver ###

On the remote cluster, the _Receiver_ will pick up incoming updates from the _Sender_ and write them to the cluster with a regular TableMutator. The result will be reported back to the _Sender_. In case of a failure, the _Sender_ can retry and/or send a notification mail to the administrator. If there are too many unsuccessful retries, the _Sender_ can choose another _Receiver_.

When writing the keys with the TableMutator, the _Receiver_ will also add the cluster ID of the originating cluster to the log fragment. This will allow the _Sender_ to send update batches to a cluster which originally created the records. (Adding the Cluster ID will require a new interface to the TableMutator.)

### Failover and Balancing ###

Periodically, the _RMaster_ of the originating cluster asks the _RMaster_ of the replicated cluster for a list of all _Receiver_ processes. If the remote _RMaster_ is not available, the local _RMaster_ will round-robin all known _RMasters_ that are listed in its hypertable.cfg till it finds one that responds. When it received a list of new _Receivers_ it will forward that list to its _Senders_.

Each _Sender_ gets a differently arranged list from its _RMaster_. First, it connects to the _Receiver_ listed at the top. If this one fails, it will connect to the second, and upon failure to the third etc. Periodically, it will try to start again from the beginning of the list. Since this list is differently shuffled for each _Sender_, the replication mechanisms should be balanced even in case of failover.

On the remote cluster, the _RMaster_ will ask another _Receiver_ to handle the queue of the crashed process. Also, the _RMaster_ will send an email to the Administrator through the notification hook.

If another _Receiver_ is started, the Master will re-balance the queues. If every _Receiver_ is responsible for a single queue, then the new _Receiver_ will create a new queue for itself.

Multiple _RMasters_ can be started on a single cluster, but only the one holding a Hyperspace lock is active. (This is the same synchronization mechanism that is used by the Hypertable Masters.)

### Testing ###

A receiving _RMaster_ can be started with a separate namespace. All the replication data will be stored into this namespace. This is required for testing multiple Replication processes on a single machine where a table from one namespace is replicated into another one, and later both namespaces can be compared.

# Milestones #

  1. Phase 1
    * _Sender_ and _Receiver_ support bi-directional replication
    * only one _Sender_/_Receiver_ per cluster
    * changes in ALTER TABLE/CREATE TABLE have to be copied manually

  1. Phase 2
    * support for multiple _Sender_ per cluster
    * support for multiple _Receiver_ per cluster
    * changes in ALTER TABLE/CREATE TABLE are replicated
    * Capistrano support

  1. Phase 3
    * monitoring support; _Sender_ and _Receiver_ display their state in the monitor