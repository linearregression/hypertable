

## Step 1. Check Hypertable Processes ##

Start with the **DFS broker**:
```
cap> ps auxww | fgrep -i broker | fgrep -v grep | fgrep -v cronolog | fgrep -v "start-dfs"
 ** [out :: motherlode013] zvents   29391  1.5  2.7 1442524 448072 ?      Sl   Nov21 106:14 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode011] zvents   16877  1.1  2.8 1508768 471812 ?      Sl   Nov21  79:22 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode014] zvents    4808  1.7  2.5 1510864 416016 ?      Sl   Nov21 119:26 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode017] zvents   17545  1.7  2.0 1516016 343556 ?      Sl   Nov21 117:17 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode019] zvents   19674  1.9  2.1 1524428 348276 ?      Sl   Nov21 133:09 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode016] zvents   22543  2.1  2.3 1523984 381052 ?      Sl   Nov21 141:43 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode015] zvents   26823  1.6  2.1 1519576 355668 ?      Sl   Nov21 112:26 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode010] zvents   27273  1.3  2.0 1449532 334240 ?      Sl   Nov21  88:45 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode012] zvents   30954 43.4  1.8 1500672 311268 ?      Sl   Nov21 2902:23 java -Xdebug -Xrunjdwp:transport=dt_socket,address=8000,server=y,suspend=n -classpath /opt/hypertable/0.9.0.12:/opt/hypertable/0.9.0.12/lib/*.jar:/opt/hypertable/0.9.0.12/java/lib/commons-logging-1.0.4.jar:/opt/hypertable/0.9.0.12/java/lib/hadoop-0.18.2-core.jar:/opt/hypertable/0.9.0.12/java/lib/hypertable-0.9.0.12.jar:/opt/hypertable/0.9.0.12/java/lib/junit-4.3.1.jar:/opt/hypertable/0.9.0.12/java/lib/log4j-1.2.13.jar:/opt/hypertable/0.9.0.12/lib/jetty-ext/*.jar org.hypertable.DfsBroker.hadoop.main --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
```

Then check for the **range servers**:
```
cap> cd /tmp; ps auxww | fgrep RangeServer | fgrep -v grep | fgrep -v cronolog
 ** [out :: motherlode013] zvents   29475 13.7 23.9 4424544 3934624 ?     Sl   Nov21 917:34 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode016] zvents   22627 16.2 30.8 5562500 5073716 ?     Sl   Nov21 1084:32 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode017] zvents   17631 12.6 27.4 4927584 4516956 ?     Sl   Nov21 845:19 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode011] zvents   16961  9.8 20.7 3756304 3406432 ?     Sl   Nov21 657:37 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode012] zvents   31038 22.2 65.1 19567820 10700568 ?   Sl   Nov21 1488:24 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode019] zvents   19758 16.1 27.5 4913872 4522696 ?     Sl   Nov21 1077:11 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode014] zvents    4892 15.6 27.2 4863100 4473520 ?     Sl   Nov21 1044:41 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode010] zvents   27522  9.6 21.0 4053916 3464136 ?     Sl   Nov21 648:04 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode015] zvents   26907 12.7 27.8 5038916 4570380 ?     Sl   Nov21 854:01 /opt/hypertable/0.9.0.12/bin/Hypertable.RangeServer --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.RangeServer.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
```

Then check for the **Hypertable** and **Hyperspace master** processes:
```
cap> cd /tmp; ps auxww | fgrep Master | fgrep -v grep | fgrep -v cronolog
 ** [out :: motherlode010] zvents   27358  0.0  0.1 314044 19432 ?        Sl   Nov21   2:18 /opt/hypertable/0.9.0.12/bin/Hyperspace.Master --pidfile=/opt/hypertable/0.9.0.12/run/Hyperspace.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
 ** [out :: motherlode010] zvents   27412  0.0  0.0 329404 13656 ?        Sl   Nov21   0:05 /opt/hypertable/0.9.0.12/bin/Hypertable.Master --pidfile=/opt/hypertable/0.9.0.12/run/Hypertable.Master.pid --verbose --config=/opt/hypertable/0.9.0.12/conf/production-hypertable.cfg
error: command "cd /tmp; ps auxww | fgrep Master | fgrep -v grep | fgrep -v cronolog" failed on motherlode011,motherlode012,motherlode013,motherlode014,motherlode015,motherlode016,motherlode017,motherlode019
```

## Step 2: Inspect Hypertable Logs ##

Start with the RangeServer logs:

```
cap> fgrep -s ERROR /opt/hypertable/current/log/Hypertable.RangeServer.log | perl -pe "s/^\d+/localtime($&)/e"

 ** [out :: motherlode010] Fri Jan  2 03:05:39 2009 ERROR Hypertable.RangeServer : finalize (/usr/src/hypertable/src/cc/Hypertable/RangeServer/CellStoreV0.cc:340): Hypertable::Exception: Error closing DFS fd: 3153 - COMM request timeout
 ** [out :: motherlode010] at virtual void Hypertable::DfsBroker::Client::close(int32_t) (/usr/src/hypertable/src/cc/DfsBroker/Lib/Client.cc:209): Event: type=ERROR "COMM request timeout" from=127.0.0.1:38030
 ** [out :: motherlode010] Fri Jan  2 03:05:39 2009 ERROR Hypertable.RangeServer : run_compaction (/usr/src/hypertable/src/cc/Hypertable/RangeServer/AccessGroup.cc:469): Hypertable::Exception: Problem finalizing CellStore '/hypertable/tables/Production/default/AB2A0D28DE6B77FFDD6C72AF/cs1057' - But that's unpossible!
 ** [out :: motherlode010] Fri Jan  2 03:05:50 2009 ERROR Hypertable.RangeServer : ~CellStoreV0 (/usr/src/hypertable/src/cc/Hypertable/RangeServer/CellStoreV0.cc:75): Error closing DFS client: Hypertable::Exception: Error closing DFS fd: 3153 - DFS BROKER bad file handle
 ** [out :: motherlode010] Fri Jan  2 03:05:50 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/MaintenanceQueue.h:114) But that's unpossible! (Problem finalizing CellStore '/hypertable/tables/Production/default/AB2A0D28DE6B77FFDD6C72AF/cs1057')
 ** [out :: motherlode011] Fri Jan  2 05:11:22 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/Lib/CommitLog.cc:281) Problem rolling commit log: /hypertable/servers/10.0.30.214_38060/log/user/70: Error closing DFS fd: 2773
 ** [out :: motherlode011] Fri Jan  2 05:11:22 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:889) COMM request timeout 'Unable to link transfer log (/hypertable/servers/10.0.30.220_38060/log/8D74C10A8E26DF225B3ADCBB) into commit log(/hypertable/servers/10.0.30.214_38060/log/user/)'
 ** [out :: motherlode011] Fri Jan  2 05:27:46 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/Lib/CommitLog.cc:313) Problem writing commit log: /hypertable/servers/10.0.30.214_38060/log/user/70: Error appending 426 bytes to DFS fd 2773
 ** [out :: motherlode011] Fri Jan  2 05:27:46 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1299) Exception caught: DFS BROKER bad file handle
 ** [out :: motherlode011] Fri Jan  2 05:27:46 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1341) DFS BROKER bad file handle 'Problem writing 921 bytes to commit log (/hypertable/servers/10.0.30.214_38060/log/user/)'
 ** [out :: motherlode011] Fri Jan  2 05:32:11 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/Lib/CommitLog.cc:313) Problem writing commit log: /hypertable/servers/10.0.30.214_38060/log/user/70: Error appending 799 bytes to DFS fd 2773
 ** [out :: motherlode011] Fri Jan  2 05:32:11 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1299) Exception caught: DFS BROKER bad file handle
 ** [out :: motherlode011] Fri Jan  2 05:32:11 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1341) DFS BROKER bad file handle 'Problem writing 2752 bytes to commit log (/hypertable/servers/10.0.30.214_38060/log/user/)'
 ** [out :: motherlode011] Fri Jan  2 05:32:11 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/Lib/CommitLog.cc:313) Problem writing commit log: /hypertable/servers/10.0.30.214_38060/log/user/70: Error appending 2185 bytes to DFS fd 2773
 ** [out :: motherlode011] Fri Jan  2 05:32:11 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1299) Exception caught: DFS BROKER bad file handle
 ** [out :: motherlode011] Fri Jan  2 05:32:11 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1341) DFS BROKER bad file handle 'Problem writing 7989 bytes to commit log (/hypertable/servers/10.0.30.214_38060/log/user/)'
 ** [out :: motherlode011] Fri Jan  2 05:32:11 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/Lib/CommitLog.cc:313) Problem writing commit log: /hypertable/servers/10.0.30.214_38060/log/user/70: Error appending 213 bytes to DFS fd 2773
 ** [out :: motherlode011] Fri Jan  2 05:32:11 2009 ERROR Hypertable.RangeServer : (/usr/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1299) Exception caught: DFS BROKER bad file handle

```

Then check the Hypertable.Master log.  Currently there is a spurious error message "Error removing DFS file" that you'll need to filter out:

```

fgrep -s ERROR /opt/hypertable/current/log/Hypertable.Master.log | fgrep -v "Error removing DFS file" | perl -pe "s/^\d+/localtime($&)/e"

```

Then check the Hyperspace.Master log.

```
fgrep -s ERROR /opt/hypertable/current/log/Hyperspace.Master.log | fgrep -v "Error removing DFS file" | perl -pe "s/^\d+/localtime($&)/e" 
```


## Step 3: Inspect the DFS ##

Here's how you do it on our production cluster (running HDFS).  First make sure the following processes are running on the master node:

```
org.apache.hadoop.dfs.NameNode
org.apache.hadoop.dfs.DataNode
org.apache.hadoop.dfs.SecondaryNameNode
org.apache.hadoop.mapred.JobTracker
org.apache.hadoop.mapred.TaskTracker
```

Then on each of the slave nodes, ensure the following two java processes are running:

```
org.apache.hadoop.dfs.DataNode
org.apache.hadoop.mapred.TaskTracker
```

Then on any machine that looks like it is having problems, check the HDFS log files for exceptions:

```
cd /opt/hadoop/current/logs/
[doug@motherlode010 logs]$ fgrep -i exception *.log | more
hadoop-zvents-datanode-motherlode010.admin.zvents.com.log:2008-12-01 00:28:26,420 WARN org.apache.hadoop.dfs.DataNode: DatanodeRegistration(10.0.30.213:50010, st
orageID=DS-393702405-10.0.30.213-50010-1222839388069, infoPort=50075, ipcPort=50020):Got exception while serving blk_8389698773741314860_195578 to /10.0.30.213:
hadoop-zvents-datanode-motherlode010.admin.zvents.com.log:java.net.SocketTimeoutException: 480000 millis timeout while waiting for channel to be ready for write.
 ch : java.nio.channels.SocketChannel[connected local=/10.0.30.213:50010 remote=/10.0.30.213:60792]
hadoop-zvents-datanode-motherlode010.admin.zvents.com.log:2008-12-01 00:28:26,420 ERROR org.apache.hadoop.dfs.DataNode: DatanodeRegistration(10.0.30.213:50010, s
torageID=DS-393702405-10.0.30.213-50010-1222839388069, infoPort=50075, ipcPort=50020):DataXceiver: java.net.SocketTimeoutException: 480000 millis timeout while w
aiting for channel to be ready for write. ch : java.nio.channels.SocketChannel[connected local=/10.0.30.213:50010 remote=/10.0.30.213:60792]
hadoop-zvents-datanode-motherlode010.admin.zvents.com.log:2008-12-01 00:28:26,968 WARN org.apache.hadoop.dfs.DataNode: DatanodeRegistration(10.0.30.213:50010, st
orageID=DS-393702405-10.0.30.213-50010-1222839388069, infoPort=50075, ipcPort=50020):Got exception while serving blk_-2928881016196368948_207241 to /10.0.30.213:
hadoop-zvents-datanode-motherlode010.admin.zvents.com.log:java.net.SocketTimeoutException: 480000 millis timeout while waiting for channel to be ready for write.
 ch : java.nio.channels.SocketChannel[connected local=/10.0.30.213:50010 remote=/10.0.30.213:60803]

```

## Step 4: Run fsck ##

```

[zvents@motherlode010 current]$ hadoop fsck /

[...]

....................................................................................................
....................................................................................................
....................................................................................................
..........................................................................................Status: HEALTHY
 Total size:	942589668108 B
 Total dirs:	21658
 Total files:	72590
 Total blocks (validated):	75341 (avg. block size 12510978 B)
 Minimally replicated blocks:	75341 (100.0 %)
 Over-replicated blocks:	0 (0.0 %)
 Under-replicated blocks:	0 (0.0 %)
 Mis-replicated blocks:		0 (0.0 %)
 Default replication factor:	3
 Average block replication:	1.0
 Corrupt blocks:		0
 Missing replicas:		0 (0.0 %)
 Number of data-nodes:		10
 Number of racks:		1


The filesystem under path '/' is HEALTHY

```
## Step 4: Inspect System Log ##

On our system (CentOS 5) this is in /var/log/messages.  It should be inspected on all machines and you should look out for things like processes that were killed because they ran out of memory.  For example, here's a snippet from the system log on one of our machines that had a problem.  Notice the "java invoked oom-killer".

```
Nov 21 11:29:10 motherlode013 kernel: Swap cache: add 7079047, delete 7078792, find 2157462/2384265, race 0+179
Nov 21 11:29:10 motherlode013 kernel: Free swap  = 0kB
Nov 21 11:29:11 motherlode013 kernel: Total swap = 8385856kB
Nov 21 11:29:11 motherlode013 kernel: Free swap:            0kB
Nov 21 11:29:11 motherlode013 kernel: 4456448 pages of RAM
Nov 21 11:29:11 motherlode013 kernel: 348373 reserved pages
Nov 21 11:29:11 motherlode013 kernel: 14473 pages shared
Nov 21 11:29:11 motherlode013 kernel: 257 pages swap cached
Nov 21 11:29:12 motherlode013 kernel: java invoked oom-killer: gfp_mask=0x201d2, order=0, oomkilladj=0
Nov 21 11:29:12 motherlode013 kernel: 
Nov 21 11:29:12 motherlode013 kernel: Call Trace:
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff800bed05>] out_of_memory+0x8e/0x2f5
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff8000f071>] __alloc_pages+0x22b/0x2b4
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff80012720>] __do_page_cache_readahead+0x95/0x1d9
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff8003d03b>] lock_timer_base+0x1b/0x3c
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff800352ff>] find_extend_vma+0x16/0x59
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff800130ab>] filemap_nopage+0x148/0x322
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff800087ed>] __handle_mm_fault+0x1f8/0xdf4
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff80064a6a>] do_page_fault+0x4b8/0x81d
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff8009b446>] autoremove_wake_function+0x0/0x2e
Nov 21 11:29:12 motherlode013 kernel:  [<ffffffff8005bde9>] error_exit+0x0/0x84
Nov 21 11:29:12 motherlode013 kernel: 
```

## Step 5: Check Disk Usage ##

For example:

```
[doug@motherlode010 clusterB]$ cap shell
  * executing 'shell'
====================================================================
Welcome to the interactive Capistrano shell! This is an experimental
feature, and is liable to change in future releases. Type 'help' for
a summary of how to use the shell.
--------------------------------------------------------------------
cap> df
[establishing connection(s) to motherlode010, motherlode011, motherlode012, motherlode013, motherlode014, motherlode015, motherlode016, motherlode017, motherlode019]
 ** [out :: motherlode013] Filesystem           1K-blocks      Used Available Use% Mounted on
 ** [out :: motherlode013] /dev/md1              59522644  15510252  40940008  28% /
 ** [out :: motherlode013] /dev/sdd5            687649580  22251900 629903272   4% /data4
 ** [out :: motherlode013] /dev/sdc5            687649580  22905696 629249476   4% /data3
 ** [out :: motherlode013] /dev/sdb5            687649580  21882712 630272460   4% /data2
 ** [out :: motherlode013] /dev/sda7            685554676  69937104 580231600  11% /data1
 ** [out :: motherlode013] /dev/md0                101018     11580     84222  13% /boot
 ** [out :: motherlode013] tmpfs                  8216148         0   8216148   0% /dev/shm
 ** [out :: motherlode013] netapp-a:/vol/vol1/common
 ** [out :: motherlode013] 770623808 525759488 244864320  69% /common
 ** [out :: motherlode013] netapp-b.admin.zvents.com:/vol/vol0/home/zvents
 ** [out :: motherlode013] 90390400  47017280  43373120  53% /home/zvents
 ** [out :: motherlode013] netapp-b.admin.zvents.com:/vol/vol0/home/doug
 ** [out :: motherlode013] 90390400  47017280  43373120  53% /home/doug
 ** [out :: motherlode012] Filesystem           1K-blocks      Used Available Use% Mounted on
 ** [out :: motherlode012] /dev/md1              59522644  56458664         0 100% /
 ** [out :: motherlode012] /dev/sdd5            687649580  22523020 629632152   4% /data4
 ** [out :: motherlode012] /dev/sdc5            687649580  21937116 630218056   4% /data3
 ** [out :: motherlode012] /dev/sdb5            687649580  21504128 630651044   4% /data2
 ** [out :: motherlode012] /dev/sda7            685554676  70094352 580074352  11% /data1
 ** [out :: motherlode012] /dev/md0                101018     11580     84222  13% /boot
 ** [out :: motherlode012] tmpfs                  8216148         0   8216148   0% /dev/shm
 ** [out :: motherlode012] netapp-a:/vol/vol1/common
 ** [out :: motherlode012] 770623808 525759488 244864320  69% /common
 ** [out :: motherlode012] netapp-b.admin.zvents.com:/vol/vol0/home/zvents
 ** [out :: motherlode012] 90390400  47017280  43373120  53% /home/zvents
 ** [out :: motherlode012] netapp-b.admin.zvents.com:/vol/vol0/home/dranch
 ** [out :: motherlode012] 90390400  47017280  43373120  53% /home/dranch
 ** [out :: motherlode012] netapp-b.admin.zvents.com:/vol/vol0/home/doug
 ** [out :: motherlode012] 90390400  47017280  43373120  53% /home/doug
[...]
```

## Restarting Hadoop ##

If you're running Hadoop and the Hadoop processes die on some or all of the machines, here's how you restart them.  In this case, motherlode018 died and restarted.

```
ssh zvents@motherlode010
[zvents@motherlode010 ~]$ cd /opt/hadoop/current
[zvents@motherlode010 current]$ ./bin/start-all.sh
namenode running as process 16394. Stop it first.
motherlode014: datanode running as process 4419. Stop it first.
motherlode018: starting datanode, logging to /opt/hadoop/current/bin/../logs/hadoop-zvents-datanode-motherlode018.admin.zvents.com.out
motherlode010: datanode running as process 16539. Stop it first.
motherlode015: datanode running as process 1921. Stop it first.
motherlode012: datanode running as process 20888. Stop it first.
motherlode013: datanode running as process 16921. Stop it first.
motherlode011: datanode running as process 26245. Stop it first.
motherlode017: datanode running as process 6468. Stop it first.
motherlode019: datanode running as process 25705. Stop it first.
motherlode016: datanode running as process 26279. Stop it first.
motherlode010: secondarynamenode running as process 16656. Stop it first.
jobtracker running as process 16729. Stop it first.
motherlode010: tasktracker running as process 17194. Stop it first.
motherlode014: tasktracker running as process 4515. Stop it first.
motherlode013: tasktracker running as process 17349. Stop it first.
motherlode012: tasktracker running as process 21436. Stop it first.
motherlode015: tasktracker running as process 2480. Stop it first.
motherlode017: tasktracker running as process 6980. Stop it first.
motherlode016: tasktracker running as process 26796. Stop it first.
motherlode011: tasktracker running as process 26809. Stop it first.
motherlode019: tasktracker running as process 26245. Stop it first.
motherlode018: starting tasktracker, logging to /opt/hadoop/current/bin/../logs/hadoop-zvents-tasktracker-motherlode018.admin.zvents.com.out
[zvents@motherlode010 current]$ 
```