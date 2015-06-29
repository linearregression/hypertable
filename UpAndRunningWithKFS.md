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

**Step 2.** Obtain the KFS source code and build it.  A working snapshot can be found at [kosmosfs.tgz](http://www.hypertable.org/pub/kosmosfs.tgz).  To install it do the following:

```
cd ~/src/
wget http://www.hypertable.org/pub/kosmosfs.tgz
tar xzf kosmosfs.tgz
cd kosmosfs/
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ~/src/kosmosfs
make
make install
```

Alternatively, if you wish to pull from the KFS repository, issue the following commands:

```
cd ~/src/
svn co https://kosmosfs.svn.sourceforge.net/svnroot/kosmosfs/trunk kosmosfs 
cd kosmosfs/
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ~/src/kosmosfs
make
make install
```

**Step 3.** Edit [~/src/kosmosfs/scripts/machines.cfg](http://www.hypertable.org/doc/kfs-machines.cfg)

**Step 4.** Kill the servers
```
cd ~/src/kosmosfs/scripts
python kfslaunch.py -f machines.cfg -S
```

**Step 5.** Upgrade the binaries.  If you just want to upgrade a pre-existing install, do the following:
```
cd ~/src/kosmosfs/scripts
python kfssetup.py -f machines.cfg -b /opt/kfs -w ../webui -u
```
If you want to delete an install and start over, then issue these commands:
```
cd ~/src/kosmosfs/scripts
python kfssetup.py -f machines.cfg -b /opt/kfs -w ../webui -U
python kfssetup.py -f machines.cfg -b /opt/kfs -w ../webui
```
If you're installing from scratch, just issue the following commands:
```
cd ~/src/kosmosfs/scripts
python kfssetup.py -f machines.cfg -b /opt/kfs -w ../webui
```

**Step 6.** Launch the KFS servers
```
cd ~/src/kosmosfs/scripts
python kfslaunch.py -f machines.cfg -s
```

**Step 7.** Make sure the servers are up
```
/opt/kfs/bin/tools/kfsping -m -s motherlode000 -p 20000
```

**Step 8.** Configure Capistrano for your specific cluster and KFS.  See [How to Deploy Hypertable](DeployingHypertable.md) for details.  The following is an example of how the variables at the top of the Capfile might be changed for KFS.
```
------------- Capfile ----------------
set :source_machine, "motherlode000"
set :install_dir,  "/data1/doug/hypertable" 
set :hypertable_version, "0.9.0.10"
set :default_config, "/home/doug/conf/cluster1-standard.cfg"

role :master, "motherlode001"
role :slave,  "motherlode001", "motherlode002", "motherlode003", "motherlode004", "motherlode005", "motherlode006", "motherlode007", "motherlode008"
```

**Step 9.** Compile the Hypertable code and install under the installation directory (e.g. /data1/doug/hypertable)

**Step 10.** Distribute the installation
```
$ cap dist
```

**Step 11.** Start the servers
```
$ cap -S dfs=kfs start
```

Now you sould be able to run the `~/hypertable/bin/hypertable` HQL command interpreter and start playing around.

## Stopping the System ##

```
$ cap -S dfs=kfs stop
```

## Additional Notes ##

**1.** To change the replication factor, stop the servers and add the following property (example shows replication factor of 2) to the MetaServer.prp property file (e.g. /mnt/kfs/meta/bin/MetaServer.prp) on the metaserver:

```
metaServer.maxReplicasPerFile = 2
```