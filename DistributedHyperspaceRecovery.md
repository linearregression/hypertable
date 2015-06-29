Say you have three Hyperspace replicas running on `host[1-3].somedomain.com` respectively and the master replica which was running on `host2` fails. If you want to replace the failed server with a new one so you can get up and running again with 3 Hyperspace replicas:

  1. Run `cap stop` to stop the cluster
  1. Login to one of the functioning servers which was running Hyperspace replica (in this case `host1` or `host3`) and run:
```
/usr/local/BerkeleyDB.4.8/bin/db_hotbackup -h <$HYPERTABLE_INSTALL_DIR>/hyperspace/ -b hyperspace_snapshot
tar -zcvf hyperspace_snapshot.tgz hyperspace_snapshot
```
  1. Identify the new machine you want to use as a Hyperspace replica (say `host4`)
  1. Delete contents of `host4:/<$HYPERTABLE_INSTALL_DIR>/hyperspace`. Copy `hyperspace_snapshot.tgz` to `host4:/<$HYPERTABLE_INSTALL_DIR>/hyperspace/` & extract.
  1. Modify your hypertable config file to remove `host2` and add `host4` under the property `Hyperspace.Replica.Host` .
  1. Modify hypertable config file to replace `host2` with `host4` in case it was set as `Hypertable.Master.Host`
  1. Modify the Capfile list of hyperspace servers to replace `host2` with `host4` (and master server in case `host2` was the Hypertable master as well).
  1. Run `cap dist` to distribute the new config file through the cluster
  1. Run `cap start`

In case you want to just restart with the 2 remaining Hyperspace replicas:

  1. Run `cap stop` to stop the cluster
  1. Modify your hypertable config file to remove `host2` under the property `Hyperspace.Replica.Host` .
  1. Modify hypertable config file to replace `host2` with another server in case it was set as `Hypertable.Master.Host`
  1. Modify the Capfile list of hyperspace servers by removing `host2` (and master server in case `host2` was the Hypertable master as well).
  1. Run `cap dist` to distribute the new config file through the cluster
  1. Run `cap start`

The following is an example of the Hyperspace part of the config file for a 3 node Hyperspace setup:
```
...
# Hyperspace
Hyperspace.Replica.Host=mothercompiler
Hyperspace.Replica.Host=motherlode000
Hyperspace.Replica.Host=motherlode001
Hyperspace.Replica.Port=38040
Hyperspace.Replica.Dir=hyperspace
Hyperspace.Replica.Workers=20
...
```

The corresponding section of the Capfile looks like:
```
...
role :hyperspace, "mothercompiler","motherlode000", "motherlode001"
...
```