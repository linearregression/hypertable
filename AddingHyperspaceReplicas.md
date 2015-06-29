How to add new Hypersapce replicas to an existing cluster

# Introduction #

This page describes the procedure for adding a new Hyperspace replica to an existing setup.


# Procedure #

Say you have three Hyperspace replicas running on host[1-3].somedomain.com respectively and you wish to add a fourth replica on host4.

  1. Run cap stop to stop the cluster
  1. Login to one of the functioning servers which was running Hyperspace replica (in this case host[1-3]) and run:
```
/usr/local/BerkeleyDB.4.8/bin/db_hotbackup -h <$HYPERTABLE_INSTALL_DIR>/hyperspace/ -b hyperspace_snapshot

tar -zcvf hyperspace_snapshot.tgz hyperspace_snapshot 
```
  1. Delete contents of `host4:/<$HYPERTABLE_INSTALL_DIR>/hyperspace`. Untarzip
```
tar -zxvf hyperspace_snapshot.tgz
```
and copy over its contents to `host4:/<$HYPERTABLE_INSTALL_DIR>/hyperspace/`
  1. Modify your hypertable config file to add host4 under the property Hyperspace.Replica.Host. Where you put it in the list matters in master elections, and in most cases you want to add it to the end of the list.
  1. Modify the Capfile list of hyperspace servers to add host4.
  1. Run cap dist to distribute the new config file through the cluster
  1. Run cap start