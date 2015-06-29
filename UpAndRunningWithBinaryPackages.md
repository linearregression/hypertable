# Introduction #

The latest release of Hypertable comes with a few [binary packages](http://package.hypertable.org/), which can cut down significant amount of build time (including dependencies) if your platform is included (Linux (Intel/AMD x86 32 and 64-bit) and Mac OS X (Intel only) in the list.

# Getting Started #

System packages (`*.rpm`, `*.deb` and `*.dmg`) can be installed by users with root access.  For users without root access, the compressed archive `*.tar.bz2` can be unpacked in any directory the user has write permission.

RPM installation
```
rpm -i <package>.rpm
```
Debian installation
```
dpkg --install <package>.deb
```
Bzipped archive installation
```
tar jxvf <package>.tar.bz2
```

The binary packages create the **/opt/hypertable** and **/var/opt/hypertable** directories.
After installation, make a symlink from **/opt/hypertable/current** to your current version.

```
cd /opt/hypertable
sudo ln -s 0.9.2.7/ current
```

_**NOTE**: This document assumes that Hypertable has been installed under **/opt/hypertable/$RELEASE** with the symlink **/opt/hypertable/current** pointing to it._

Start the hypertable servers (in local/single node mode)
```
/opt/hypertable/current/bin/ht start all-servers local
DFS broker: available file descriptors: 1024
Started DFS Broker (local)
Started Hyperspace
Started Hypertable.Master
Started Hypertable.RangeServer
Started ThriftBroker
```

Use the Hypertable shell for experiments.  For an introduction to the commands available, see [HQLTutorial](HQLTutorial.md).

```
/opt/hypertable/current/bin/ht shell

Welcome to the hypertable command interpreter.
For information about Hypertable, visit http://www.hypertable.org/

Type 'help' for a list of commands, or 'help shell' for a
list of shell meta commands.

hypertable> 
```

Use Jruby to experiment with the Hypertable API (via ThriftBroker)
```
$ CLASSPATH=/opt/hypertable/current/lib/java/hypertable-0.9.2.7.jar:/opt/hypertable/current/lib/java/log4j-1.2.13.jar:/opt/hypertable/current/lib/java/libthrift.jar jirb
irb(main):001:0> require 'java'
=> false
irb(main):002:0> include_class 'org.hypertable.thrift.ThriftClient'
=> ["org.hypertable.thrift.ThriftClient"]
irb(main):003:0> client = ThriftClient.create("localhost", 38080)
=> #<Java::OrgHypertableThrift::ThriftClient:0x180b22e @java_object=#<Java::JavaObject:0x89dd>>
irb(main):004:0> client.hql_query("show tables").results.each {|t| puts t }
METADATA
=> nil
irb(main):005:0> 
```

The C++ and [JVM languages](http://en.wikipedia.org/wiki/List_of_JVM_languages) clients are usable without installing additional software, as the Thrift libraries for C++ and Java are bundled with the package. For [other Thrift supported languages](http://wiki.apache.org/thrift), you'll need to install the Thrift package. Sample clients in
Java, Perl, PHP, Python and Ruby are included in the package under the /opt/hypertable/current/lib directory.

Finally, to shut everything down, use the **stop-servers.sh** script:

```
/opt/hypertable/current/bin/stop-servers.sh
```