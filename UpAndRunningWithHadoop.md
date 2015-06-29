
## Starting the Servers ##

**Step 1.** Synchronize clocks on all machines

The system cannot operate correctly unless the clocks on all machines are synchronized.  Use the [Network Time Protocol (ntp)](http://www.ntp.org/) to ensure that the clocks get synchronized and remain in sync.  Run the 'date' command on all machines to make sure they are in sync.  The following Capistrano shell session show the output of a cluster with properly synchronized clocks.

```
cap> date
[establishing connection(s) to motherlode000, motherlode001, motherlode002, motherlode003, motherlode004, motherlode005, motherlode006, motherlode007, motherlode008]
 ** [out :: motherlode001] Sat Jan  3 18:05:33 PST 2009
 ** [out :: motherlode002] Sat Jan  3 18:05:33 PST 2009
 ** [out :: motherlode003] Sat Jan  3 18:05:33 PST 2009
 ** [out :: motherlode004] Sat Jan  3 18:05:33 PST 2009
 ** [out :: motherlode005] Sat Jan  3 18:05:33 PST 2009
 ** [out :: motherlode007] Sat Jan  3 18:05:33 PST 2009
 ** [out :: motherlode006] Sat Jan  3 18:05:33 PST 2009
 ** [out :: motherlode000] Sat Jan  3 18:05:33 PST 2009
 ** [out :: motherlode008] Sat Jan  3 18:05:33 PST 2009
```


**Step 2.** Install and start Hadoop.

**Step 3.** Create the directory `/hypertable` in HDFS and make it writeable by all.

```
$ hadoop fs -mkdir /hypertable
$ hadoop fs -chmod 777 /hypertable
```

**Step 4.** Edit the config file conf/hypertable.cfg  Change the following property to point to the Hadoop filesystem that got up and running in step 2 (assuming hdfs://motherlode000:9000):
```
HdfsBroker.fs.default.name=hdfs://motherlode001:9000
```
Change the following two properties to point to the location of the Hypertable Master and Hyperspace (assuming motherlode001):
```
Hyperspace.Master.Host=motherlode001
Hypertable.Master.Host=motherlode001
```

**Step 5.** Configure Capistrano for your specific cluster and HDFS.  See [How to Deploy Hypertable](DeployingHypertable.md) for details.  The following is an example of how the variables at the top of the Capfile might be changed for HDFS.
```
------------- Capfile ----------------
set :source_machine, "motherlode000"
set :install_dir,    "/opt/hypertable" 
set :hypertable_version, "0.9.2.7"
set :default_dfs, "hadoop"
set :default_config, "/opt/hypertable/cluster1-standard.cfg"

role :master, "motherlode001"
role :slave,  "motherlode001", "motherlode002", "motherlode003", "motherlode004", "motherlode005", "motherlode006", "motherlode007", "motherlode008"
```

**Step 6.** Compile the Hypertable code and install under the installation directory (e.g. /data1/doug/hypertable)

**Step 7.** Distribute the installation
```
$ cap dist
```

**Step 8.** Start the servers
```
$ cap start
```

Now you sould be able to run the `~/hypertable/bin/ht shell` HQL command interpreter and start playing around.

## Stopping the System ##

```
$ cap stop
```