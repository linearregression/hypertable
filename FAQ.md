

## LOAD DATA INFILE hangs when running Hypertable on HDFS ##

HDFS (as of version 0.20.0) has difficulty handling too many repeated output stream sync operations, especially when running in an environment like EC2 which can experience network congestion.  Each time the load client (e.g. LOAD DATA INFILE) sends over a batch of writes, it causes sync() to get called on the commit log.  This can cause the system to invoke more sync() requests than HDFS can handle.  To get around this problem, we added a --no-log-sync option to the command interpreter.  This will prevent the commit log from getting flushed on every write and will cause it to flush only once at the end of loading the entire file.  Here's an example of how to invoke this option:

```
echo "LOAD DATA INFILE data.tsv INTO TABLE foo;" | /opt/hypertable/current/bin/ht shell --no-log-sync=true
```

## I'm seeing `DFSClient: Could not obtain block blk_ ...`  messages in DfsBroker.hadoop.log ##

The HDFS DataNode has an limit on the number of files that it can serve concurrently.  This limit is controlled by the `dfs.datanode.max.xcievers` hdfs configuration property.  We recommend that you set the value as follows in the `hdfs-site.xml` file:

```
<property>
  <name>dfs.datanode.max.xcievers</name>
  <value>4096</value>
</property>
```

You'll need to restart HDFS for the change to take effect.

## System hangs on Ubuntu ##

There appears to be a bug in the kernel (e.g. 2.6.32-308) for certain releases of Ubuntu.  The processes seem to lose `epoll` notifications.  This problem was [reported on the Hadoop mailing list](http://www.mail-archive.com/common-user@hadoop.apache.org/msg05557.html) as well.  Try running an older version of Ubuntu.  Here's an EC2 AMI of Ubuntu 9.10 from AppScale that seems to work - ami-3a81755  If you experience this problem with other kernel versions, please report to the [mailing list](http://groups.google.com/group/hypertable-dev).  The recommended OS for Hypertable is CentOS/RedHat 5.

The following non-Redhat systems are known to work:

  * Debian GNU/Linux 5.0 \n \l (2.6.32-5-amd64)

## Error mkdirs DFS directory /hypertable/servers/rs1/log/user - DFS BROKER i/o error ##

Hypertable persists all of its state in the DFS under the directory `/hypertable`.  This directory needs to exist and be writeable by the Hypertable server processes.  To do this in HDFS, run the following commands in the same user account that was used to start the HDFS server processes:

```
hadoop fs -mkdir /hypertable
hadoop fs -chmod 777 /hypertable
```

## RangeServer fails to start on HDFS with failed expectation (nread == RSML\_HEADER\_SIZE) ##

The Hypertable.RangeServer.log file will have an entry like the following at the end of it:

```
1252947174 ERROR Hypertable.RangeServer : local_recover (/root/
hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:460):
Hypertable::Exception: failed expectation: nread == RSML_HEADER_SIZE -
METALOG bad range server metalog header
```

The range server writes state changes to a meta log (RSML) in the underlying DFS (`/hypertable/servers/<ip>_<port>/log/range_txn`).  It replays this log when it restarts in order to reconstruct its state at the time it stopped.

Hypertable relies on the underlying filesystem to provide a properly functioning fsync() operation, which the current version of HDFS (0.20.1) does not.  HDFS 0.21 will support this operation.  In the meantime, when shutting down Hypertable, use either `cap stop` or the hypertable command interpreter `shutdown` command.  This will cause the shutdown() method on the range servers to get invoked, which will properly close the meta log so that it will appear in its entirety in the filesystem.

If a server goes down unexpectedly you should be able to recover with the following steps:

  1. Properly shut down Hypertable (`cap stop`)
  1. Shutdown HDFS
  1. Restart HDFS
  1. Restart Hypertable

The HDFS Datanodes will recover any partially written files on startup.  When HDFS is restarted, the RSML should appear in the filesystem intact and allow the range server to recover its state.

## RangeServer silently dies ##

When the system is undergoing intense loading activity, a RangeServer will periodically disappear, leaving no core file or ERROR message in the log.

The RangeServer has been designed to track its memory usage.  By default, when it sees that the memory usage has exceeded 70% of physical RAM, it will pause the incoming request queue and allow system maintenance activity to proceed, freeing up memory.  When memory usage drops back down below the threshold, the incoming request queue is un-paused.

The RangeServer tracks memory consumption by keeping track of the amount of memory allocated.  However, it does not account for any heap fragmentation.  Some memory allocators (e.g. the default glibc allocator) can get fragmented to the point where the program's estimation of memory usage is off significantly.  This can cause the system to start paging and eventually the RangeServer process can get killed by the Linux "out of memory killer".  To check whether or not this is what you're experiencing, run the dmesg command and look for "Out of Memory" messages:

```
$ dmesg
[...]
Out of Memory: Kill process 4564 (start-rangeserv) score 1528534 and children.
Out of memory: Killed process 4565 (Hypertable.Rang).
oom-killer: gfp_mask=0x201d2, order=0

Call Trace: <ffffffff8012f335>{printk_ratelimit+21}
       <ffffffff80157267>{out_of_memory+55} <ffffffff80159137>{__alloc_pages+567}
       <ffffffff8015ae4f>{__do_page_cache_readahead+335} <ffffffff8031c462>{io_schedule+50}
       <ffffffff8031ce08>{__wait_on_bit_lock+104} <ffffffff80153890>{sync_page+0}
       <ffffffff8015b3f1>{do_page_cache_readahead+97} <ffffffff80156733>{filemap_nopage+323}
       <ffffffff80163008>{__handle_mm_fault+1752} <ffffffff801180c9>{do_page_fault+3657}
       <ffffffff8031d23c>{schedule_hrtimer+60} <ffffffff80149150>{hrtimer_nanosleep+96}
       <ffffffff801490f1>{hrtimer_nanosleep+1} <ffffffff8010bb5b>{error_exit+0}
```

There are two ways in which you can correct this problem (#1 is preferable) :

1. Install [Google perftools](http://code.google.com/p/google-perftools/) and then re-configure and re-build Hypertable.  Google perftools comes with a memory allocation library called tcmalloc.  From the RangeServer perspective, it is very efficient and has low heap fragmentation.  To use it, just download and install it, then re-run cmake to configure your build and then re-build the Hypertable software.

2. Decrease the MaxMemory.Percentage property to account for increased memory fragmentation overhead:

```
Hypertable.RangeServer.MemoryLimit.Percentage=50
```

## Why do I get "rsync: connection unexpectedly closed" when running cap dist? ##
```
When I used "cap dist", then the following errors occurred on half of 100 nodes,
*** [err :: uranos4.dq.isl.ntt.co.jp] ssh_exchange_identification:
Connection closed by remote host
*** [err :: uranos4.dq.isl.ntt.co.jp] rsync: connection unexpectedly
closed (0 bytes received so far) [receiver]
*** [err :: uranos4.dq.isl.ntt.co.jp]
*** [err :: uranos4.dq.isl.ntt.co.jp] rsync error: unexplained error
(code 255) at io.c(453) [receiver=2.6.9]
*** [err :: uranos4.dq.isl.ntt.co.jp]
....
```
**A:** This is because the default MaxStartups value of `/etc/sshd_config` on the source machine is
too small.  Set the value of this parameter to a number that is at least as large as the number of nodes in your cluster.

You may also see the following errors:

```
*** [err :: machine5] Host key verification failed.
*** [err :: machine5] 
*** [err :: machine5] rsync: connection unexpectedly closed (0 bytes received so far) [receiver]
*** [err :: machine5] rsync error: error in rsync protocol data stream (code 12) at io.c(635) [receiver=3.0.2]
```

**A:** This is because the remote host (machine5) has SSH Host Key Protection enabled.  This can be fixed by logging into the source host from the remote host (machine5) and answer "yes" when you get the following prompt:

```
machine5$ ssh machine0
The authenticity of host 'machine0 (8.17.172.215)' can't be established.
RSA key fingerprint is 38:bf:44:b8:fb:82:43:4d:ff:49:72:d9:85:30:bc:7d.
Are you sure you want to continue connecting (yes/no)? 
```

## Why am I seeing "accept() failure: Too many open files" in the Master log file? ##
```
1234563919 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563922 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563927 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563931 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563935 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563939 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563943 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563947 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563951 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
1234563955 ERROR Hypertable.Master : (/usr/src/hypertable/src/cc/AsyncComm/IOHandlerAccept.cc:78) accept() failure: Too many open files
```
**A:** This is because you have the soft file descriptor limit set too low.  On CentOS 5, here's how you would change the soft limit from 1024 to 16384.  Modify the file /etc/security/limits.conf and change the lines:
```
* soft nofile 1024
* hard nofile 65536
```
to
```
* soft nofile 16384
* hard nofile 65536
```
Then you need to log out and log back in for the changes to take effect.   Here's how to check to be sure the new limit is in effect:
```
$ ulimit -n
16384
```

## How do I convert the log file entries from Unix time (seconds since the Epoch) to something human readable? ##

Try adding the following alias to your .bashrc file:

```
alias lcat='perl -pe "s/^\d+/localtime($&)/e"'
```

And then you can examine the log files like this:

```
$ lcat Hypertable.RangeServer.log
Wed Apr 15 00:00:14 2009 INFO Hypertable.RangeServer : (/usr/local/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:2047) Memory Usage: 299447013 bytes
Wed Apr 15 00:00:14 2009 INFO Hypertable.RangeServer : (/usr/local/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:2104) Cleaning log (threshold=200000000)
Wed Apr 15 00:00:34 2009 INFO Hypertable.RangeServer : (/usr/local/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:2047) Memory Usage: 299447013 bytes
Wed Apr 15 00:00:34 2009 INFO Hypertable.RangeServer : (/usr/local/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:2104) Cleaning log (threshold=200000000)
...
```

## Problem building libunwind: #error Cannnot calculate stack trace: ... ##

When I try to build libunwind, I get errors like the following:

```
 g++ -DHAVE_CONFIG_H -I. -I. -I./src -I./src -Wall -Wwrite-strings-Woverloaded-virtual -Wno-sign-compare -DNO_FRAME_POINTER -g -O2 -MT stacktrace.lo -MD -MP -MF .deps/stacktrace.Tpo -c src/stacktrace.cc -fPIC -DPIC -o .libs/stacktrace.o
In file included from src/stacktrace.cc:57: src/stacktrace_config.h:66:5: #error Cannnot calculate stack trace: need either libunwind or frame-pointers (see INSTALL file)
src/stacktrace.cc:70:3: #error Cannot calculate stack trace: will need to write for your environment
make: *** [stacktrace.lo] Error 1
```

To fix this, try running configure as follows

```
CFLAGS=-fPIC ./configure
```


## RangeServer unable to connect to Hyperspace on EC2 ##

If the server logs complain about being able to connect to each other:
```
1248816365 ERROR Hypertable.RangeServer : (/foo/hypertable-0.9.2.4-alpha/src/cc/Hypertable/RangeServer/main.cc:82) Unable to connect to hyperspace, exiting...
```

you may need to open up ports on your EC2 hosts. To do this run something like:
```
$prompt> ec2-authorize $your-access-group -p 1-65000 -P tcp
$prompt> ec2-authorize $your-access-group -p 1-65000 -P udp
```

You can confirm your changes by examining the output of ec2-describe-group.
Once you have confirmed this is the issue, you can tighten the port access restrictions by just opening up the ports used by hypertable (consult your config file).

## RANGE SERVER clock skew detected ##

Hypertable requires that all machines participating in a Hypertable cluster have their clocks synchronized.  The system uses Multi-Version Concurrency Control (MVCC) and by default will auto-assign revision numbers using a high resolution timestamp.  These revision numbers are used for snapshot isolation as well as determining which portions of the commit log can be safely deleted.  When clocks are significantly out-of-skew (e.g. several seconds), old results can suddenly appear in a query, or worse, when the system is brought down and back up again, data can go missing.  In almost all circumstances, the system will detect clock skew and either refuse updates by throwing an exception up to the application,

```
Failed: (3405054,ItemRank,9223372036854775810) - RANGE SERVER clock skew detected
Failed: (3405054,ClickURL,9223372036854775810) - RANGE SERVER clock skew detected
```

or will log an error message in the range server log file:

```
 ** [out :: motherlode004] 1253227860 ERROR Hypertable.RangeServer : (/home/doug/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1554) RANGE SERVER clock skew detected 'Clocks skew of 20504726 microseconds exceeds maximum (3000000) range=query-log[1709153..197905]'
 ** [out :: motherlode004] 1253227867 ERROR Hypertable.RangeServer : (/home/doug/src/hypertable/src/cc/Hypertable/RangeServer/RangeServer.cc:1506) Exception caught: RANGE SERVER clock skew detected
```

To prevent this situation, use the [Network Time Protocol (ntp)](http://www.ntp.org/) to ensure that the clocks get synchronized and remain in sync.  Run the 'date' command on all machines to make sure they are in sync.  The following Capistrano shell session show the output of a cluster with properly synchronized clocks.

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

## How do I make Hypertable use a different network interface (eth1 instead of eth0) ? ##

Hypertable, by default, uses the machine's primary network interface.  However, there are situations where the primary interface should not be used.  For example, servers in Rackspace cloud (mosso.com) have two interfaces eth0 with public IP address and eth1 with private IP address (10.x.x.x).  eth0 is the primary, but eth1 is preferred because the private network is twice as fast and traffic is not counted by billing system.

To configure Hypertable to use the interface eth1, add the following line to the config file:

```
Hypertable.Network.Interface=eth1
```

## Hypertable logs are filling my disks, how can I remedy this? ##

Add a cron job along the lines of:

```
find $HYPERTABLE_HOME/log/archive -mtime $keep_days -exec rm -rf {} +
```

## Firewall is blocking traffic, what ports should I open up? ##

TCP:
  * 38030
  * 38040
  * 38041
  * 38050
  * 38060
  * 38080
  * 38090

UDP:
  * 38040 (in both directions)

## Problem starting servers on OpenSUSE 11.4:   ht-env.sh: line 105: 31207 Aborted  (core dumped) ##

When I try starting the servers on OpenSUSE 11.4, I get the following error messages:

```
# /opt/hypertable/current/bin/start-all-servers.sh local
DFS broker: available file descriptors: 1024
/opt/hypertable/current/bin/ht-env.sh: line 105: 31070 Aborted                 (core dumped) $HYPERTABLE_HOME/bin/serverup --silent "$@" &>/dev/null
/opt/hypertable/current/bin/ht-env.sh: line 105: 31071 Aborted                 (core dumped) $HYPERTABLE_HOME/bin/serverup --silent "$@" &>/dev/null
/opt/hypertable/current/bin/ht-env.sh: line 105: 31073 Aborted                 (core dumped) $HYPERTABLE_HOME/bin/serverup --silent "$@" &>/dev/null
/opt/hypertable/current/bin/ht-env.sh: line 105: 31075 Aborted                 (core dumped) $HYPERTABLE_HOME/bin/serverup --silent "$@" &>/dev/null
/opt/hypertable/current/bin/ht-env.sh: line 105: 31083 Aborted                 (core dumped) $HYPERTABLE_HOME/bin/serverup --silent "$@" &>/dev/null
/opt/hypertable/current/bin/ht-env.sh: line 105: 31082 Aborted                 (core dumped) $HYPERTABLE_HOME/bin/serverup --silent "$@" &>/dev/null
```


On OpenSUSE 11.4, libstdc++ has a problem with regard to locale handling.  In the shell in which you launch hypertable from, you'll need to execute the following command first:

export LC\_CTYPE=""