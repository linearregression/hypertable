

## Local Installation ##

### Package Installation ###

Hypertable can be installed via [binary packages](http://www.hypertable.com/download/).  The packages come bundled with nearly all of the dependent shared libraries.  The nice thing about this approach is that just two packages are required for linux, a 64-bit linux package and a 32-bit linux package.  The only requirement is that your system is built with glibc 2.4+ (released on March 6th 2006 and provides stack smashing protection).  If you're running an older distribution with pre 2.4 glibc, you can build your own binary packages by following the instructions in [#Appendix\_B.\_How\_To\_Package](#Appendix_B._How_To_Package.md).

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
Mac installation:

> Double-click the `.dmg` file and follow the instructions

The RPM, Debian, and Mac packages will install Hypertable under a directory by the name of `/opt/hypertable/$VERSION` by default.  If you don't have root access or cannot modify the `/opt` directory, use the `.tar.bz2` archive to install Hypertable wherever you would like.

### FHS-ize Installation ###

The binary packages will install Hypertable entirely under /opt/hypertable/$VERSION.  The [Filesystem Hierarchy Standard](http://www.pathname.com/fhs/pub/fhs-2.3.html) states that host-specific configuration files for add-on application software packages should reside under `/etc/opt` and that variable data of the packages (log files, etc.) should reside under `/var/opt`.  Hypertable ships with a script that will modify an installation to conform to the Filesystem Hierachy Standard.  If you're running as a user other than root, first create hypertable directories in `/etc/opt` and `/var/opt` and change ownership to the user account under which the binaries will be run.  For example:

```
$ sudo mkdir /etc/opt/hypertable /var/opt/hypertable
$ sudo chown doug:staff /etc/opt/hypertable /var/opt/hypertable
```

Then run the `fhsize.sh` from within the installation.  For example:

```
$ /opt/hypertable/0.9.5.0/bin/fhsize.sh 
Setting up /var/opt/hypertable
Setting up /etc/opt/hypertable
```

We **strongly** recommend that you fhsize your installation.  The nice thing about FHS-izing your installations is that the directories containing the config files, hyperspace, run directory, localBroker root directory, and log archives carry over from one version to the next.  Otherwise, you need to copy the contents of the `conf/`, `hyperspace/`, `run/`, `fs/`, and `log/` from your old install to the new one each time you upgrade.  When `fhsize.sh` is run for the first time, it will copy the contents of the `conf/` directory from the binary package to `/etc/opt/hypertable` and it will copy the contents of the `fs/`, `run/`, `hyperspace/`, and `log/` directories to corresponding directories in `/var/opt/hypertable` if they are non-empty.  When `fhsize.sh` is run subsequently, it will detect the existence of files in `/etc/opt/hypertable` and `/var/opt/hypertable` and will leave them intact.  Here's what the installation directory looks like after running `fhsize.sh`:
```
$ ls -l /opt/hypertable/0.9.5.0/
total 16
drwxr-xr-x 2 doug staff 4096 2011-04-14 19:44 bin
lrwxrwxrwx 1 doug staff   15 2011-04-14 19:45 conf -> /etc/opt/hypertable
drwxr-xr-x 5 doug staff 4096 2011-04-14 19:45 examples
lrwxrwxrwx 1 doug staff   18 2011-04-14 19:45 fs -> /var/opt/hypertable/fs
lrwxrwxrwx 1 doug staff   26 2011-04-14 19:45 hyperspace -> /var/opt/hypertable/hyperspace
drwxr-xr-x 9 doug staff 4096 2011-04-14 19:44 include
drwxr-xr-x 7 doug staff 4096 2011-04-14 19:45 lib
lrwxrwxrwx 1 doug staff   19 2011-04-14 19:45 log -> /var/opt/hypertable/log
lrwxrwxrwx 1 doug staff   19 2011-04-14 19:45 run -> /var/opt/hypertable/run
```

### Set current link ###

After installation, make a symlink from `/opt/hypertable/current` to your current version.

```
$ cd /opt/hypertable
$ ln -s 0.9.5.0 current
```

### Verify Installation ###

Start the hypertable servers (in local/single node mode)
```
$ /opt/hypertable/current/bin/ht start all-servers local
DFS broker: available file descriptors: 1024
Started DFS Broker (local)
Started Hyperspace
Started Hypertable.Master
Started Hypertable.RangeServer
Started ThriftBroker
```

Use the Hypertable shell for experiments.  For an introduction to the commands available, see [#HQL\_Tutorial](#HQL_Tutorial.md).

```
/opt/hypertable/current/bin/ht shell

Welcome to the hypertable command interpreter.
For information about Hypertable, visit http://www.hypertable.org/

Type 'help' for a list of commands, or 'help shell' for a
list of shell meta commands.

hypertable> 
```
Create and use a namespace called "bar"
```
hypertable> create namespace bar;
hypertable> use bar;
```
Create a table named "foo"
```
hypertable> create table foo ( c1, c2 );
```
Show the tables.
```
hypertable> show tables;
foo
```
Insert some data into table "foo".
```
hypertable> insert into foo values("001", "c1", "totally"), ("000", "c1", "Hypertable"), ("001", "c2", "awesome"), ("000", "c2", "is");
```
Select data from table "foo".
```
hypertable> select * from foo;
000         c1     Hypertable
000         c2     is
001         c1     totally
001         c2     awesome
```
Drop table "foo"
```
hypertable> drop table foo;
```
Drop namespace "bar"
```
hypertable> use "/";
hypertable> drop namespace bar;
```
Finally, to shut everything down, use the stop-servers command:

```
/opt/hypertable/current/bin/ht stop-servers
```

## Cluster Installation ##

### Capistrano - A Tool for Automating Tasks on Remote Servers ###

The Hypertable distribution comes a number of scripts to start and stop the various servers that make up a Hypertable cluster.  You can use your own cluster management tool to launch these scripts and deploy new binaries.  However, if you're not already using a cluster management tool, we recommend [Capistrano](http://www.capify.org/).  The distribution comes with a Capistrano config file (`conf/Capfile.cluster`) that makes deploying and launching Hypertable a breeze.

Capistrano is a simple tool for automating the remote execution of tasks.  It uses ssh to do the remote execution.  To ease deployment, you should have password-less ssh access (e.g. public key) to all of the machines in your cluster.  Installing Capistrano is pretty simple.  On most systems you just need to execute the following command
```
$ sudo gem update
$ sudo gem install capistrano
```
After this installation step you should now have the `cap` program in your path:
```
$ cap --version
Capistrano v2.5.8
```

### Step 1. Edit Capistrano Capfile ###

Once you have Capistrano installed, copy the `conf/Capfile.cluster` that comes with the Hypertable distribution to your working directory (e.g. home directory) on `admin1`, rename it to `Capfile`, and tailor it for your environment.  The `cap` command reads the file `Capfile` in the current working directory by default.  There are some variables that are set at the top that you need to modify for your particular environment.  The following shows the variables at the top of the Capfile that need modification:
```
set :source_machine,     "admin"
set :install_dir,        "/opt/hypertable" 
set :hypertable_version, "0.9.5.0"
set :default_pkg,        "/tmp/hypertable-0.9.5.0-linux-x86_64.deb"
set :default_dfs,        "hadoop"
set :default_config,     "/home/doug/hypertable.cfg"
```
Here's a brief description of each variable:

| **variable** | **description** |
|:-------------|:----------------|
| `source_machine` | machine from which you will build the binaries, distribute them to the other machines, and launch the service. |
| `install_dir` | directory on `source_machine` where you have installed Hypertable.  It is also the directory on the remote machines where the installation will get rsync'ed to. |
| `hypertable_version` | version of Hypertable you are deploying |
| `default_pkg` | Path to binary package file (either `.dmg` or `.rpm`) on source machine |
| `default_dfs` | distributed file system you are running Hypertable on top of.  Valid values are "local", "hadoop", "kfs", or "ceph" |
| `default_config` | location of the default configuration file that plan to use |
**Table 1. Hypertable Capistrano Variables**

In addition to the above variables, you also need to define three roles, one for the machine that will run the master processes, one for the machines that will run the Hyperspace replicas, and one for the machines that will run the RangeServers.  Edit the following lines:
```
role :source, "admin1"
role :master, "master"
role :hyperspace, "hyperspace001", "hyperspace002", "hyperspace003"
role :slave,  "slave001", "slave002", "slave003", "slave004", "slave005", "slave006", "slave007", "slave008"
role :localhost, "admin1"
role :thriftbroker
role :spare
```

Here's a brief description of each role:

| **role** | **description** |
|:---------|:----------------|
| `source` | This role is for the machine you will be distributing the binaries from, `admin` in this example |
| `master` | This role is for the machine that will run the Hypertable master process as well as a DFS broker.  Ideally this machine is high quality and somewhat lightly loaded (e.g. not running a RangeServer).  Typically you would have a high quality machine running the Hypertable master, a Hyperspace replica, and the HDFS NameNode |
| `hyperspace` | This role is for the machines that will run Hyperspace replicas.  There should be at least one machine defined for this role.  The machines that take on this role should be somewhat lightly loaded (e.g. not running a RangeServer) |
| `slave`  | This role is for the machines that will run RangeServers.  Hypertable is designed to run on a filesystem like HDFS.  In fact, the system works best from a performance standpoint when the RangeServers are run on the same machines as the HDFS DataNodes.  This role will also launch a DFS broker and a ThriftBroker. |
| `localhost` | This should be the name of the machine that you're administering the cluster from, `admin1` in this example |
| `thriftbroker` | This role is for machines that will just be running the ThriftBroker (e.g. web servers) |
| `spare`  | This role is for machines that will act as standbys.  They will be kept current with the latest binaries |
**Table 2. Hypertable Capistrano Roles**

### Step 2. Distribute and Install Binaries ###

If you're installing binary packages (e.g. `.dmg` or `.rpm`), put the Hypertable package that you want to install, on the source machine, `admin1` in this example.  Modify the `hypertable_version` and `default_pkg` variables at the top of the Capfile to contain the version of Hypertable you are installing and the absolute path to the package file on the source machine, respectively.  Then distribute and install the binary package with the following command:
```
$ cap install_package
```
If you're installing from binaries that you've compiled directly or from the .tar.bz2 archive, you can use Capistrano to distribute the binaries with rsync.  On `admin1` be sure Hypertable is installed in the location specified by the `install_dir` variable at the top of the Capfile and that the `hypertable_version` variable at the top of the Capfile matches the version you are installing (`/opt/hypertable` and `0.9.5.0` in this example).  Then distribute the binaries with the following command:
```
$ cap dist
```
**NOTE:** You may have to increase the value of MaxStartups in your `/etc/sshd_config` file if it is less than the number of nodes in your cluster

### Step 3. FHS-ize Installation ###

See [#FHS-ize\_Installation](#FHS-ize_Installation.md) for an introduction to FHS.  If you're running as a user other than root, first create the directories `/etc/opt/hypertable` and `/var/opt/hypertable` on all machines in the cluster and change ownership to the user account under which the binaries will be run.  For example:

```
$ sudo mkdir /etc/opt/hypertable /var/opt/hypertable
$ sudo chown doug:staff /etc/opt/hypertable /var/opt/hypertable
```

Then FHS-ize the installation with the following command:
```
$ cap fhsize
```

### Step 4. Distribute Custom hypertable.cfg file ###

Configuring Hypertable is straightforward.  You just need to copy the default config file (`conf/hypertable.cfg`) to a permanent location (outside the installation directory) and change the following properties:

| Property | Description |
|:---------|:------------|
| `HdfsBroker.fs.default.name` | Hadoop ONLY: Hadoop filesystem name |
| `Kfs.MetaServer.Name` | KFS ONLY: KFS metaserver machine name |
| `Kfs.MetaServer.Port` | KFS ONLY: KFS metaserver port |
| `CephBroker.MonAddr` | Ceph ONLY: Ceph monitor machine address |
| `Hyperspace.Replica.Host` | Name of host running Hyperspace replica (one line for each replica) |

See [hypertable-example.cfg](http://www.hypertable.org/pub/hypertable-example-cfg.txt)

Once you've edited the configuration file for your cluster on `admin1`, be sure the _absolute_ pathname referenced in the `default_config` Capfile variable points to this file (e.g. `/home/doug/hypertable.cfg`).  Then distribute the custom config files with the following command:
```
$ cap push_config
```

### Step 5. Set `current` link ###

We typically create a symbolic link `current` in the installation directory (i.e. `install_dir`) to point to the current installation directory.  To do this for the installation you've just pushed out, execute the following command:
```
$ cap set_current
```

### Step 6. Synchronize Clocks ###

The system cannot operate correctly unless the clocks on all machines are synchronized.  Use the [Network Time Protocol (ntp)](http://www.ntp.org/) to ensure that the clocks get synchronized and remain in sync.  Run the 'date' command on all machines to make sure they are in sync.  The following Capistrano shell session show the output of a cluster with properly synchronized clocks.

```
cap> date
[establishing connection(s) to master, hyperspace001, hyperspace002, hyperspace003, slave001, slave002, slave003, slave004, slave005, slave006, slave007, slave008]
 ** [out :: master] Sat Jan  3 18:05:33 PST 2009
 ** [out :: hyperspace001] Sat Jan  3 18:05:33 PST 2009
 ** [out :: hyperspace002] Sat Jan  3 18:05:33 PST 2009
 ** [out :: hyperspace003] Sat Jan  3 18:05:33 PST 2009
 ** [out :: slave001] Sat Jan  3 18:05:33 PST 2009
 ** [out :: slave002] Sat Jan  3 18:05:33 PST 2009
 ** [out :: slave003] Sat Jan  3 18:05:33 PST 2009
 ** [out :: slave004] Sat Jan  3 18:05:33 PST 2009
 ** [out :: slave005] Sat Jan  3 18:05:33 PST 2009
 ** [out :: slave007] Sat Jan  3 18:05:33 PST 2009
 ** [out :: slave008] Sat Jan  3 18:05:33 PST 2009
```

### Starting and Stopping Hypertable ###

The following commands should be run from the directory containing the Capfile.  To start all of the Hypertable servers ...
```
$ cap start
```
And to stop the service, shutting down all servers ...
```
$ cap stop
```
If you want to wipe your database clean, removing all tables ...
```
$ cap cleandb
```
If you want to launch the service using a different config file than the default (e.g. /home/doug/alternate.cfg) ...
```
$ cap stop
$ cap -S config=/home/doug/alternate.cfg start
```

### Adding new servers to an existing cluster ###
As your data grows, you may want to increase the number of RangeServers in your cluster. Hypertable allows you to do this with 0 downtime and automatic load balancing, ie the system will auto-migrate load from the existing RangeServer machines onto the new machine(s) without any manual intervention. To add machines to an existing cluster you need to first setup services needed by Hypertable on the new machine as follows:

**Step 1:  Synchronize clocks**

To do this you need to run `ntpd` and wait for clock to synchronize before adding server.  Make sure the clocks on the new machine(s) are synchronized with the clocks on the other machines in the cluster.  You can verify this by first adding the new machines to your `Capfile` and then running `date` on all of the machines in the cluster as follows:
```
$ cap shell
cap> date
[establishing connection(s) to test09, test08, test10, test11, test12, test13, test14, test15]
 ** [out :: test09] Thu Sep 22 11:33:48 PDT 2011
 ** [out :: test10] Thu Sep 22 11:33:48 PDT 2011
 ** [out :: test12] Thu Sep 22 11:33:48 PDT 2011
 ** [out :: test08] Thu Sep 22 11:33:48 PDT 2011
 ** [out :: test11] Thu Sep 22 11:33:48 PDT 2011
 ** [out :: test14] Thu Sep 22 11:33:48 PDT 2011
 ** [out :: test15] Thu Sep 22 11:33:48 PDT 2011
 ** [out :: test13] Thu Sep 22 11:33:48 PDT 2011
```

**Step 2: Start Hadoop DataNode on new machines**

Make sure you are running a local datanode of your DFS with the same configuration as the rest of your cluster.  For Cloudera installation, follow instructions in the [CDH3 Installation Guide](https://ccp.cloudera.com/display/CDHDOC/CDH3+Installation+Guide).

**Step 3: Deploy Hypertable RangeServers**

  1. Check your Capfile to make sure hypertable\_version is the correct version
  1. Edit your Capfile to add the new server(s) to the slave list (should've been done in Step 1)
  1. `cap dis`t (If you have an fhsized install, login to the new servers and run /opt/hypertable/current/bin/fhsize.sh)
  1. `cap push_config`
  1. `cap set_current`
  1. `cap start_slaves`

## Up and Running with Hadoop ##

### Step 1. Install Hadoop ###

> _**NOTE:** Hadoop version 0.20.2 contains a bug which can cause Hypertable to lose data or become corrupt in the face of machine failure.  The set of patches in the [0.20-append branch](http://svn.apache.org/viewvc/hadoop/common/branches/branch-0.20-append/) fixes this problem.  Be sure to apply these patches, or use a distribution that includes them such as [CDH3](http://www.cloudera.com/blog/2011/02/cdh3-beta-4-now-available/)_

The first step to getting up and running on a Hadoop Cluster is to install Hadoop.  The process for doing this is outside the scope of this document.  See [CDH3 Installation Guide](https://ccp.cloudera.com/display/CDHDOC/CDH3+Installation+Guide) for details.

The Hadoop cluster is typically administered from the machine that runs the NameNode.  For the remainder of this section, we'll assume it is called `master` and the `fs.default.name` property in the Hadoop configuration is setup as follows:

```
<property>
  <name>fs.default.name</name>
  <value>hdfs://master:9000</value>
</property>
```

Be sure this property matches the `HdfsBroker.fs.default.name` property in the Hypertable config file.

### Step  2. Start HDFS ###

Launch HDFS.  On the NameNode machine:
```
$ sudo service hadoop-0.20-namenode start
$ sudo service hadoop-0.20-secondarynamenode start
```

On the DataNode machines:
```
$ sudo service hadoop-0.20-datanode start
```

Wait for HDFS to come out of "safemode":

```
$ hadoop dfsadmin -safemode wait
Safe mode is OFF
```

Then create the HDFS directory in which Hypertable will store data (`/hypertable`) and make it readable and writable by the Hypertable processes.  If Hadoop and Hypertable are launched from the same user account, this step is not necessary.

```
$ hadoop fs -mkdir /hypertable
$ hadoop fs -chmod 777 /hypertable
```

## Upgrading your Installation ##

### Format Compatible Upgrade ###

If you're upgrading to a new release of Hypertable that is format compatible with your old one, upgrading is quick and painless.  First stop the service and then install the new release.  This is a simple matter of editing the `hypertable_version` and `default_pkg` to point to the new installation package and issuing the following Capistrano commands:
```
$ cap stop
$ cap install_package
```
Once you've done that, you can upgrade with the following Capistrano command:
```
$ cap upgrade
```

This command performs the following actions:

  1. Verifies that the upgrade is format compatible
  1. Copies the contents of the `conf/`, `hyperspace/`, `fs/`, `run/`, and `log/` directories from the old installation (referenced from the current symlink) into the new installation.  If any of these directories are symlinked, then an identical symlink will be created in the new installation and the contents are not copied for that directory.
  1. Sets the `current` symlink on all machines to point to the new installation

You can then bring the service back up with:
```
$ cap start
```

### Format Incompatible Upgrade ###

If the new release you are installing is incompatible with the old release, you will need to backup and restore all of your tables.  See the following section on how to backup and restore tables.  Here's the basic upgrade sequence:

  1. Backup your tables
  1. Install the new release (see [#Local\_Installation](#Local_Installation.md) or [#Cluster\_Installation](#Cluster_Installation.md))
  1. Restore your tables

You can then bring the service back up with:
```
$ cap start
```

## Backing Up and Restoring Tables ##

Hypertable backup and restore are accomplished with the HQL commands DUMP TABLE and LOAD DATA INFILE.  It is possible to create backups with the SELECT command, but it is not recommended because it will output table data in sorted order.  When loading data with LOAD DATA INFILE that is in sorted order, only one RangeServer will be kept busy at a time.  To combat this problem, the DUMP TABLE command was introduced which will output table data in random order.  Loading backups taken with the DUMP TABLE command is much more efficient since all of the participating RangeServers will be kept busy.

### backup.sh ###
```
#!/usr/bin/env bash

if [ $# -lt 2 ] ; then
    echo "usage: $0 <namespace> <tablename> [ <tablename> ... ]"
    exit 0
fi

namespace=$1
shift
while [ $# -ge 1 ]; do
    echo "Backing up '${namespace}/$1' ..."
    echo "USE '${namespace}';" > create-table-$1.hql
    echo "USE '${namespace}'; SHOW CREATE TABLE '$1';" | /opt/hypertable/current/bin/ht shell --silent >> create-table-$1.hql
    echo ";" >> create-table-$1.hql
    echo "USE '${namespace}'; DUMP TABLE '$1' INTO FILE '$1.gz';" | /opt/hypertable/current/bin/ht shell
    shift
done
```
### restore.sh ###
```
#!/usr/bin/env bash

if [ $# -lt 2 ] ; then
    echo "usage: $0 <namespace> <backup-file> [ <backup-file> ... ]"
    exit 0
fi

namespace=$1
shift

while [ $# -ge 1 ]; do
    table=`basename $1 | cut -f1 -d'.'`
    echo "Restoring '${namespace}/${table}' ..."
    cat create-table-$1.hql | /opt/hypertable/current/bin/ht shell
    echo "USE '${namespace}'; LOAD DATA INFILE '$1.gz' INTO TABLE '$table';" | /opt/hypertable/current/bin/ht shell
    shift
done
```

## HQL Tutorial ##

### Introduction ###

This tutorial shows you how to import a search engine query log into Hypertable, storing the data into tables with different primary keys, and how to issue queries against the tables.  You'll need to download the data from http://hypertable.googlecode.com/files/query-log.tsv.gz:

```
$ mkdir -p hql_tutorial
$ cd hql_tutorial
$ wget http://hypertable.googlecode.com/files/query-log.tsv.gz
```

The next step is to make sure Hypertable is properly installed (see [#Local\_Installation](#Local_Installation.md) or [#Cluster\_Installation](#Cluster_Installation.md)) and then launch the service.  The following terminal session illustrates how to launch a local instance of Hypertable:
```
$ /opt/hypertable/current/bin/start-all-servers.sh local
DFS broker: available file descriptors: 256
Started DFS Broker (local)
Started Hyperspace
Started Hypertable.Master
Started Hypertable.RangeServer
Started ThriftBroker
```

Now fire up an interactive session:

```
$ /opt/hypertable/current/bin/ht shell

Welcome to the hypertable command interpreter.
For information about Hypertable, visit http://www.hypertable.org/

Type 'help' for a list of commands, or 'help shell' for a
list of shell meta commands.

hypertable> 
```

Get help:
```
hypertable> help;

USE ................ Sets the current namespace
CREATE NAMESPACE ... Creates a new namespace
DROP NAMESPACE ..... Removes a namespace
EXISTS TABLE ....... Check if table exists
CREATE TABLE ....... Creates a table
DELETE ............. Deletes all or part of a row from a table
DESCRIBE TABLE ..... Displays a table's schema
DROP TABLE ......... Removes a table
RENAME TABLE ....... Renames a table
DUMP TABLE ......... Create efficient backup file
ALTER TABLE ........ Add/remove column family from existing table
INSERT ............. Inserts data into a table
LOAD DATA INFILE ... Loads data from a TSV input file into a table
SELECT ............. Selects (and display) cells from a table
SHOW CREATE TABLE .. Displays CREATE TABLE command used to create table
SHOW TABLES ........ Displays only the list of tables in the current namespace
GET LISTING ........ Displays the list of tables and namespace in the current namespace
SHUTDOWN ........... Shuts servers down gracefully

Statements must be terminated with ';'.  For more information on
a specific statement, type 'help <statement>', where <statement> is from
the preceeding list.

```

### USE ###
First, we need to open a namespace within which we will operate on tables. A Hypertable namespace is roughly analogous to a "database" in a system like MySQL. A major difference is that a namespace can contain other namespaces in addition to tables. To start using the root namespace:
```
hypertable> use "/";
```

### CREATE NAMESPACE ###

To create a new namespace and use it:
```
hypertable> create namespace "Tutorial";
hypertable> use Tutorial;
```
### CREATE TABLE ###
Now that we have created and opened the "Tutorial" namespace we can create tables within it. In this tutorial we will be loading data into, and querying data from, two separate tables.  The first table, `QueryLogByUserID`, will be indexed by the fields UserID+QueryTime and the second table, `QueryLogByTimestamp`, will be indexed by the fields QueryTime+UserID.  Notice that any identifier that contains non-alphanumeric characters (e.g. '-') must be surrounded by quotes.

```
hypertable> CREATE TABLE QueryLogByUserID (
Query,
ItemRank,
ClickURL
);

  Elapsed time:  3.95 s

hypertable> CREATE TABLE QueryLogByTimestamp (
Query,
ItemRank,
ClickURL
);

  Elapsed time:  3.95 s

```

See the [HQL Documentation: CREATE TABLE](http://www.hypertable.org/hql/create-table.html) for complete syntax.

### SHOW TABLES ###

Show all of the tables that exist in the current namespace:
```
hypertable> show tables;
QueryLogByUserID
QueryLogByTimestamp

  Elapsed time:  1.91 s

```

### SHOW CREATE TABLE ###

Now, issue the SHOW CREATE TABLE command to make sure you got everything right. We didn't have to include the field called 'row' because we'll use that in our LOAD DATA INFILE command later:

```
hypertable> show create table QueryLogByUserID;

CREATE TABLE QueryLogByUserID (
  Query,
  ItemRank,
  ClickURL,
  ACCESS GROUP default (Query, ItemRank, ClickURL)
)


  Elapsed time:  2.20 s

```
And, notice that, by default, a single ACCESS GROUP is created.  Access groups are a way to physically group columns together on disk.  See the [CREATE TABLE](http://www.hypertable.org/hql/create-table.html) documentation for a more detailed description of access groups.

### LOAD DATA INFILE ###

Now, let's load some data using the MySQL-like TAB delimited format (TSV). For that, we assume you have the example data in the file `query-log.tsv.gz`. This file includes an initial header line indicating the format of each line in the file by listing tab delimited column names:

```
$ gzcat query-log.tsv.gz
#QueryTime      UserID  Query   ItemRank        ClickURL
2008-11-13 00:01:30     2289203 kitchen counter in new orleans  10      http://www.era.com
2008-11-13 00:01:30     2289203 kitchen counter in new orleans  4       http://www.superpages.com
2008-11-13 00:01:30     2289203 kitchen counter in new orleans  5       http://www.superpages.com
2008-11-13 00:01:31     1958633 beads amethyst gemstone 1       http://www.gemsbiz.com
2008-11-13 00:01:31     3496052 chat            
2008-11-13 00:01:33     892003  photo example quarter doubled die coin  5       http://www.coinresource.com
2008-11-13 00:01:33     892003  photo example quarter doubled die coin  5       http://www.coinresource.com
2008-11-13 00:01:35     2251112 radio stations in buffalo       1       http://www.ontheradio.net
2008-11-13 00:01:37     1274922 fafsa renewal   1       http://www.fafsa.ed.gov
2008-11-13 00:01:37     1274922 fafsa renewal   1       http://www.fafsa.ed.gov
2008-11-13 00:01:37     441978  find phone numbers      1       http://www.anywho.com
2008-11-13 00:01:37     441978  find phone numbers      3       http://www.411.com
...
```

Now let's load the data file `query-log.tsv.gz` into the table `QueryLogByUserID`.  The row key is formulated by zero-padding the UserID field out to nine digits and concatenating the QueryTime field.  The QueryTime field is used as the internal cell timestamp.

```
hypertable> LOAD DATA INFILE ROW_KEY_COLUMN="%09UserID"+QueryTime TIMESTAMP_COLUMN=QueryTime "query-log.tsv.gz" INTO TABLE QueryLogByUserID;

Loading 7,524,712 bytes of input data...

0%   10   20   30   40   50   60   70   80   90   100%
|----|----|----|----|----|----|----|----|----|----|
***************************************************
Load complete.

  Elapsed time:  9.84 s
Avg value size:  15.42 bytes
  Avg key size:  29.00 bytes
    Throughput:  4478149.39 bytes/s (764375.74 bytes/s)
   Total cells:  992525
    Throughput:  100822.73 cells/s
       Resends:  0

```
A quick inspection of the table shows:
```
hypertable> select * from QueryLogByUserID limit 8;
000000036 2008-11-13 10:30:46	Query	helena ga
000000036 2008-11-13 10:31:34	Query	helena ga
000000036 2008-11-13 10:45:23	Query	checeron s
000000036 2008-11-13 10:46:07	Query	cheveron gas station
000000036 2008-11-13 10:46:34	Query	cheveron gas station richmond virginia
000000036 2008-11-13 10:48:56	Query	cheveron glenside road richmond virginia
000000036 2008-11-13 10:49:05	Query	chevron glenside road richmond virginia
000000036 2008-11-13 10:49:05	ItemRank	1
000000036 2008-11-13 10:49:05	ClickURL	http://yp.yahoo.com
000000053 2008-11-13 15:18:21	Query	mapquest
000000053 2008-11-13 15:18:21	ItemRank	1
000000053 2008-11-13 15:18:21	ClickURL	http://www.mapquest.com

  Elapsed time:  0.01 s
Avg value size:  18.08 bytes
  Avg key size:  30.00 bytes
    Throughput:  43501.21 bytes/s
   Total cells:  12
    Throughput:  904.70 cells/s
```

Now let's load the data file `query-log.tsv.gz` into the table `QueryLogByTimestamp`.  The row key is formulated by concatenating the QueryTime field with the nine digit, zero-padded UserID field.  The QueryTime field is used as the internal cell timestamp.

```
hypertable> LOAD DATA INFILE ROW_KEY_COLUMN=QueryTime+"%09UserID" TIMESTAMP_COLUMN=QueryTime "query-log.tsv.gz" INTO TABLE QueryLogByTimestamp;

Loading 7,524,712 bytes of input data...

0%   10   20   30   40   50   60   70   80   90   100%
|----|----|----|----|----|----|----|----|----|----|
***************************************************
Load complete.

  Elapsed time:  10.18 s
Avg value size:  15.42 bytes
  Avg key size:  29.00 bytes
    Throughput:  4330913.20 bytes/s (739243.98 bytes/s)
   Total cells:  992525
    Throughput:  97507.80 cells/s
       Resends:  0
```
And a quick inspection of the table shows:
```
hypertable> select * from QueryLogByTimestamp limit 4;
2008-11-13 00:01:30 002289203	Query	kitchen counter in new orleans
2008-11-13 00:01:30 002289203	Query	kitchen counter in new orleans
2008-11-13 00:01:30 002289203	Query	kitchen counter in new orleans
2008-11-13 00:01:30 002289203	ItemRank	5
2008-11-13 00:01:30 002289203	ItemRank	4
2008-11-13 00:01:30 002289203	ItemRank	10
2008-11-13 00:01:30 002289203	ClickURL	http://www.superpages.com
2008-11-13 00:01:30 002289203	ClickURL	http://www.superpages.com
2008-11-13 00:01:30 002289203	ClickURL	http://www.era.com
2008-11-13 00:01:31 001958633	Query	beads amethyst gemstone
2008-11-13 00:01:31 001958633	ItemRank	1
2008-11-13 00:01:31 001958633	ClickURL	http://www.gemsbiz.com
2008-11-13 00:01:31 003496052	Query	chat
2008-11-13 00:01:33 000892003	Query	photo example quarter doubled die coin
2008-11-13 00:01:33 000892003	Query	photo example quarter doubled die coin
2008-11-13 00:01:33 000892003	ItemRank	5
2008-11-13 00:01:33 000892003	ItemRank	5
2008-11-13 00:01:33 000892003	ClickURL	http://www.coinresource.com
2008-11-13 00:01:33 000892003	ClickURL	http://www.coinresource.com

  Elapsed time:  0.00 s
Avg value size:  18.11 bytes
  Avg key size:  30.00 bytes
    Throughput:  287150.49 bytes/s
   Total cells:  19
    Throughput:  5969.21 cells/s
```

See the [HQL Documentation: LOAD DATA INFILE](http://www.hypertable.org/hql/load-data-infile.html) for complete syntax.

### SELECT ###

Let's start by examining the `QueryLogByUserID` table.  To select all of the data for user ID 003269359 we need to use the _**starts with**_ operator =^.  Remember that the row key is the concatenation of the user ID and the timestamp which is why we need to use the _**starts with**_ operator.
```
hypertable> select * from QueryLogByUserID where row =^ '003269359';
003269359 2008-11-13 04:36:34	Query	binibining pilipinas 2008 winners
003269359 2008-11-13 04:36:34	ItemRank	5
003269359 2008-11-13 04:36:34	ClickURL	http://www.missosology.org
003269359 2008-11-13 04:37:34	Query	pawee's kiss and tell
003269359 2008-11-13 04:37:34	Query	pawee's kiss and tell
003269359 2008-11-13 04:37:34	ItemRank	3
003269359 2008-11-13 04:37:34	ItemRank	3
003269359 2008-11-13 04:37:34	ClickURL	http://www.missosology.org
003269359 2008-11-13 04:37:34	ClickURL	http://www.missosology.org
003269359 2008-11-13 05:07:10	Query	rn jobs in 91405
...
003269359 2008-11-13 09:42:49	Query	wound ostomy rn training
003269359 2008-11-13 09:42:49	ItemRank	11
003269359 2008-11-13 09:42:49	ClickURL	http://www.wocn.org
003269359 2008-11-13 09:46:50	Query	pych nurse in encino tarzana hospital
003269359 2008-11-13 09:47:18	Query	encino tarzana hospital
003269359 2008-11-13 09:47:18	ItemRank	2
003269359 2008-11-13 09:47:18	ClickURL	http://www.encino-tarzana.com
003269359 2008-11-13 09:52:42	Query	encino tarzana hospital
003269359 2008-11-13 09:53:08	Query	alhambra hospital
003269359 2008-11-13 09:53:08	ItemRank	1
003269359 2008-11-13 09:53:08	ClickURL	http://www.alhambrahospital.com

  Elapsed time:  0.01 s
Avg value size:  19.24 bytes
  Avg key size:  30.00 bytes
    Throughput:  2001847.79 bytes/s
   Total cells:  352
    Throughput:  40651.35 cells/s
```

The result set was fairly large (352 cells), so let's now try selecting just the queries that were issued by the user with ID 003269359 during the hour of 5am.  To do this we need to add a TIMESTAMP predicate.  Each cell has an internal timestamp and the TIMESTAMP predicate can be used to filter the results based on this timestamp.

```
hypertable> select * from QueryLogByUserID where row =^ '003269359' AND "2008-11-13 05:00:00" <= TIMESTAMP < "2008-11-13 06:00:00";
003269359 2008-11-13 05:07:10	Query	rn jobs in 91405
003269359 2008-11-13 05:07:10	Query	rn jobs in 91405
003269359 2008-11-13 05:07:10	ItemRank	9
003269359 2008-11-13 05:07:10	ItemRank	8
003269359 2008-11-13 05:07:10	ClickURL	http://91405.jobs.com
003269359 2008-11-13 05:07:10	ClickURL	http://www.hospitalsoup.com
003269359 2008-11-13 05:20:22	Query	rn jobs in 91405
003269359 2008-11-13 05:20:22	ItemRank	16
003269359 2008-11-13 05:20:22	ClickURL	http://www.careerbuilder.com
003269359 2008-11-13 05:34:02	Query	usc university hospital
003269359 2008-11-13 05:34:02	ItemRank	1
003269359 2008-11-13 05:34:02	ClickURL	http://www.uscuh.com
003269359 2008-11-13 05:37:01	Query	rn jobs in san fernando valley
003269359 2008-11-13 05:37:01	Query	rn jobs in san fernando valley
003269359 2008-11-13 05:37:01	Query	rn jobs in san fernando valley
003269359 2008-11-13 05:37:01	ItemRank	7
003269359 2008-11-13 05:37:01	ItemRank	4
003269359 2008-11-13 05:37:01	ItemRank	3
003269359 2008-11-13 05:37:01	ClickURL	http://www.medhunters.com
003269359 2008-11-13 05:37:01	ClickURL	http://san.fernando.valley.registered.nurse.to.72k.san.fernando.ca.job.hireability.com
003269359 2008-11-13 05:37:01	ClickURL	http://www.resumes2work.com
003269359 2008-11-13 05:46:22	Query	northridge hospital
003269359 2008-11-13 05:46:22	Query	northridge hospital
003269359 2008-11-13 05:46:22	ItemRank	2
003269359 2008-11-13 05:46:22	ItemRank	1
003269359 2008-11-13 05:46:22	ClickURL	http://northridgehospital.org
003269359 2008-11-13 05:46:22	ClickURL	http://www.chwhealth.com
003269359 2008-11-13 05:53:34	Query	valley presbyterian hospital
003269359 2008-11-13 05:53:34	ItemRank	4
003269359 2008-11-13 05:53:34	ClickURL	http://www.hospital-data.com
003269359 2008-11-13 05:55:36	Query	valley presbyterian hospital website
003269359 2008-11-13 05:55:36	ItemRank	1
003269359 2008-11-13 05:55:36	ClickURL	http://www.valleypres.org
003269359 2008-11-13 05:59:24	Query	mission community hospital
003269359 2008-11-13 05:59:24	ItemRank	1
003269359 2008-11-13 05:59:24	ClickURL	http://www.mchonline.org

  Elapsed time:  0.00 s
Avg value size:  18.50 bytes
  Avg key size:  30.00 bytes
    Throughput:  2602086.44 bytes/s
   Total cells:  36
    Throughput:  53651.27 cells/s
```

Keep in mind that the internal cell timestamp is different than the one embedded in the row key.  In this example, they both represent the same time.  By specifying the TIMESTAMP\_COLUMN option to LOAD DATA INFILE, we extracted the QueryTime field to be used as the internal cell timestamp.  If we hadn't supplied that option, the system would have auto-assigned a timestamp.  To display the internal cell timestamp, add the DISPLAY\_TIMESTAMPS option:

```
hypertable> select * from QueryLogByUserID limit 5 DISPLAY_TIMESTAMPS;
2008-11-13 10:30:46.000000000	000000036 2008-11-13 10:30:46	Query	helena ga
2008-11-13 10:31:34.000000000	000000036 2008-11-13 10:31:34	Query	helena ga
2008-11-13 10:45:23.000000000	000000036 2008-11-13 10:45:23	Query	checeron s
2008-11-13 10:46:07.000000000	000000036 2008-11-13 10:46:07	Query	cheveron gas station
2008-11-13 10:46:34.000000000	000000036 2008-11-13 10:46:34	Query	cheveron gas station richmond virginia

  Elapsed time:  0.00 s
Avg value size:  17.20 bytes
  Avg key size:  30.00 bytes
    Throughput:  207563.76 bytes/s
   Total cells:  5
    Throughput:  4397.54 cells/s
```

There is no index for the internal cell timestamps, so if we don't include a `row =^` expression in our predicate, the system will do a full table scan.  This is why we imported the data into a second table `QueryLogByTimestamp`.  This table includes the timestamp as the row key prefix which allows us to efficiently query data over a time interval.

The following query selects all query log data for November 14th, 2008:

```
hypertable> select * from QueryLogByTimestamp WHERE ROW =^ '2008-11-14';
2008-11-14 00:00:00 001040178	Query	noodle tools
2008-11-14 00:00:00 001040178	ItemRank	1
2008-11-14 00:00:00 001040178	ClickURL	http://www.noodletools.com
2008-11-14 00:00:01 000264655	Query	games.myspace.com
2008-11-14 00:00:01 000264655	ItemRank	1
2008-11-14 00:00:01 000264655	ClickURL	http://games.myspace.com
2008-11-14 00:00:01 000527424	Query	franklinville schools new jersey
2008-11-14 00:00:01 000527424	ItemRank	1
2008-11-14 00:00:01 000527424	ClickURL	http://www.greatschools.net
2008-11-14 00:00:01 000632400	Query	lack of eye contact symptom of...
...
2008-11-14 06:02:33 003676354	Query	baby 20showers
2008-11-14 06:02:35 003378030	Query	task and responsibility matrix
2008-11-14 06:02:35 003378030	ItemRank	2
2008-11-14 06:02:35 003378030	ClickURL	http://im.ncsu.edu
2008-11-14 06:02:36 004578101	Query	jcpenneys
2008-11-14 06:02:37 005120734	Query	ebay
2008-11-14 06:02:40 000957500	Query	buccal fat size of ping pong ball

  Elapsed time:  2.37 s
Avg value size:  15.36 bytes
  Avg key size:  30.00 bytes
    Throughput:  1709616.45 bytes/s
   Total cells:  89412
    Throughput:  37689.18 cells/s
```
And to select all query log data for November 14th, 2008 during the hour of 3am:
```
hypertable> select * from QueryLogByTimestamp WHERE ROW =^ '2008-11-14 03';
2008-11-14 03:00:00 002512415	Query	ny times
2008-11-14 03:00:00 002512415	ItemRank	1
2008-11-14 03:00:00 002512415	ClickURL	http://www.nytimes.com
2008-11-14 03:00:00 005294906	Query	kickmeto.fosi
2008-11-14 03:00:00 005459226	Query	www.dickdyertoyota.com
2008-11-14 03:00:02 000637292	Query	days of our lives
2008-11-14 03:00:02 000637292	ItemRank	3
2008-11-14 03:00:02 000637292	ClickURL	http://www.nbc.com
2008-11-14 03:00:03 002675105	Query	ghetto superstar lyrics
...
2008-11-14 03:59:52 002874080	ClickURL	http://www.paintball-discounters.com
2008-11-14 03:59:53 004292772	Query	drop down menu
2008-11-14 03:59:55 005656539	Query	to buy indian hair to make wigs in new york
2008-11-14 03:59:55 005656539	ItemRank	1
2008-11-14 03:59:55 005656539	ClickURL	http://query.nytimes.com
2008-11-14 03:59:58 004318586	Query	myspace .com

  Elapsed time:  0.17 s
Avg value size:  15.37 bytes
  Avg key size:  30.00 bytes
    Throughput:  2267099.06 bytes/s
   Total cells:  8305
    Throughput:  49967.51 cells/s
```
And finally, to select all query log data for November 14th, 2008 during the minute of 3:45am:
```
hypertable> select * from QueryLogByTimestamp WHERE ROW =^ '2008-11-14 03:45';
2008-11-14 03:45:00 003895650	Query	ks lottery.
2008-11-14 03:45:00 003895650	ItemRank	2
2008-11-14 03:45:00 003895650	ClickURL	http://www.lotterypost.com
2008-11-14 03:45:00 005036796	Query	www.glasgowdailytimes 10-20-2005
2008-11-14 03:45:01 002863052	Query	map quest
2008-11-14 03:45:01 005514285	Query	john bermeo
2008-11-14 03:45:02 002394176	Query	www.eggseye.com
2008-11-14 03:45:02 003454227	Query	hawaiian weddig band
2008-11-14 03:45:03 001006089	Query	brokers hiring loan officers in indiana
2008-11-14 03:45:06 000844720	Query	latest design microsoft freeware
...
2008-11-14 03:45:55 003920469	ItemRank	3
2008-11-14 03:45:55 003920469	ClickURL	http://www.pennyblood.com
2008-11-14 03:45:56 002729906	Query	tryaold
2008-11-14 03:45:56 003919348	Query	feathered draped fox fur mandalas
2008-11-14 03:45:56 003919348	ItemRank	8
2008-11-14 03:45:56 003919348	ClickURL	http://www.greatdreams.com
2008-11-14 03:45:56 004803968	Query	-

  Elapsed time:  0.02 s
Avg value size:  15.71 bytes
  Avg key size:  30.00 bytes
    Throughput:  305030.80 bytes/s
   Total cells:  130
    Throughput:  6673.51 cells/s
```

See the [HQL Documentation: SELECT](http://www.hypertable.org/hql/select.html) for complete syntax.

### ALTER TABLE ###

The ALTER TABLE command can be used to add remove columns from a table.  The following command will add a 'Notes' column in a new access group called 'extra' and will drop column 'ItemRank'.
```
hypertable> ALTER TABLE QueryLogByUserID ADD(Notes, ACCESS GROUP extra(Notes)) DROP(ItemRank);

  Elapsed time:  3.25 s
```
To verify the change, issue the SHOW CREATE TABLE command:
```
hypertable> show create table QueryLogByUserID;

CREATE TABLE QueryLogByUserID (
  Query,
  ClickURL,
  Notes,
  ACCESS GROUP default (Query, ClickURL),
  ACCESS GROUP extra (Notes)
)
```
And to verify that the column no longer exists, issue the same SELECT statement we issued above (NOTE: the data for the column still exists in the filesystem, it will get lazily garbage collected).
```
hypertable> select * from QueryLogByUserID limit 8;
000000036 2008-11-13 10:30:46	Query	helena ga
000000036 2008-11-13 10:31:34	Query	helena ga
000000036 2008-11-13 10:45:23	Query	checeron s
000000036 2008-11-13 10:46:07	Query	cheveron gas station
000000036 2008-11-13 10:46:34	Query	cheveron gas station richmond virginia
000000036 2008-11-13 10:48:56	Query	cheveron glenside road richmond virginia
000000036 2008-11-13 10:49:05	Query	chevron glenside road richmond virginia
000000036 2008-11-13 10:49:05	ClickURL	http://yp.yahoo.com
000000053 2008-11-13 15:18:21	Query	mapquest
000000053 2008-11-13 15:18:21	ClickURL	http://www.mapquest.com

  Elapsed time:  0.00 s
Avg value size:  21.50 bytes
  Avg key size:  30.00 bytes
    Throughput:  140595.14 bytes/s
   Total cells:  10
    Throughput:  2730.00 cells/s
```

See [HQL Documentation: ALTER TABLE](http://www.hypertable.org/hql/alter-table.html) for complete syntax.

### INSERT & DELETE ###

Now let's augment the `QueryLogByUserID` table by adding some information in the Notes column for a few of the queries:
```
hypertable> INSERT INTO QueryLogByUserID VALUES
("000019058 2008-11-13 07:24:43", "Notes", "animals"),
("000019058 2008-11-13 07:57:16", "Notes", "food"),
("000019058 2008-11-13 07:59:36", "Notes", "gardening");

  Elapsed time:  0.01 s
Avg value size:  6.67 bytes
   Total cells:  3
    Throughput:  298.36 cells/s
       Resends:  0
```
Notice the new data by querying the affected row:
```
hypertable> select * from QueryLogByUserID where row =^ '000019058';
000019058 2008-11-13 07:24:43	Query	tigers
000019058 2008-11-13 07:24:43	Notes	animals
000019058 2008-11-13 07:57:16	Query	bell peppers
000019058 2008-11-13 07:57:16	Notes	food
000019058 2008-11-13 07:58:24	Query	bell peppers
000019058 2008-11-13 07:58:24	ClickURL	http://agalternatives.aers.psu.edu
000019058 2008-11-13 07:59:36	Query	growing bell peppers
000019058 2008-11-13 07:59:36	Query	growing bell peppers
000019058 2008-11-13 07:59:36	ClickURL	http://www.farm-garden.com
000019058 2008-11-13 07:59:36	ClickURL	http://www.organicgardentips.com
000019058 2008-11-13 07:59:36	Notes	gardening
000019058 2008-11-13 12:31:02	Query	tracfone
000019058 2008-11-13 12:31:02	ClickURL	http://www.tracfone.com

  Elapsed time:  0.00 s
Avg value size:  16.38 bytes
  Avg key size:  30.00 bytes
    Throughput:  162271.26 bytes/s
   Total cells:  13
    Throughput:  3498.39 cells/s
```
Now try deleting one of the notes we just added
```
hypertable> delete Notes from QueryLogByUserID where ROW ="000019058 2008-11-13 07:24:43";

  Elapsed time:  0.00 s
   Total cells:  1
    Throughput:  256.41 cells/s
       Resends:  0
```
And verify that the cell was, indeed, deleted:
```
hypertable> select * from QueryLogByUserID where row =^ '000019058';
000019058 2008-11-13 07:24:43	Query	tigers
000019058 2008-11-13 07:57:16	Query	bell peppers
000019058 2008-11-13 07:57:16	Notes	food
000019058 2008-11-13 07:58:24	Query	bell peppers
000019058 2008-11-13 07:58:24	ClickURL	http://agalternatives.aers.psu.edu
000019058 2008-11-13 07:59:36	Query	growing bell peppers
000019058 2008-11-13 07:59:36	Query	growing bell peppers
000019058 2008-11-13 07:59:36	ClickURL	http://www.farm-garden.com
000019058 2008-11-13 07:59:36	ClickURL	http://www.organicgardentips.com
000019058 2008-11-13 07:59:36	Notes	gardening
000019058 2008-11-13 12:31:02	Query	tracfone
000019058 2008-11-13 12:31:02	ClickURL	http://www.tracfone.com

  Elapsed time:  0.00 s
Avg value size:  16.38 bytes
  Avg key size:  30.00 bytes
    Throughput:  162271.26 bytes/s
   Total cells:  12
    Throughput:  3498.39 cells/s
```

See the [HQL Documentation: INSERT](http://www.hypertable.org/hql/insert.html) and the [HQL Documentation: DELETE](http://www.hypertable.org/hql/delete.html) for complete syntax.

### DROP TABLE ###

The DROP TABLE command is used to remove tables from the system.  The IF EXISTS option prevents the system from throwing an error if the table does not exist:
```
hypertable> drop table IF EXISTS foo;

  Elapsed time:  0.34 s
```
Let's remove one of the example tables:
```
hypertable> drop table QueryLogByUserID;

  Elapsed time:  2.46 s
```
And show tables to see that the table was removed:
```
hypertable> show tables;
METADATA
QueryLogByTimestamp

  Elapsed time:  1.88 s
```


### GET LISTING & DROP NAMESPACE ###
Now, we want to get rid of the "Tutorial" namespace and verify that we have:

```
hypertable> use "/";

  Elapsed time:  0.00 s
hypertable> get listing;
Tutorial	(namespace)
sys	(namespace)

  Elapsed time:  0.41 s
hypertable> drop namespace Tutorial;

  Elapsed time:  2.04 s
hypertable> drop namespace Tutorial;

  Elapsed time:  2.04 s
hypertable> get listing;
sys	(namespace)

  Elapsed time:  0.52 s
```

The "sys" namespace is used by the Hypertable system and should not be used to contain user tables.

Note that a namespace must be empty (ie must not contain any sub-namespaces or tables) before you can drop it. In this case since we had already dropped the "Pages" table, we could go ahead and drop the "Tutorial" namespace.

## Regular Expression Filtering ##

Hypertable supports filtering of data  using regular expression matching on the rowkey, column qualifiers and value.
Hypertable uses [RE2](http://code.google.com/p/re2/) for regular expression matching, the complete supported syntax can be found [here](http://code.google.com/p/re2/wiki/Syntax).

### Example ###

In this example we'll use a DMOZ dataset which contains title, description and a bunch of topic tags for a set of URLs. The domain components of the URL have been reversed so that URLs from the same domain sort together. In the schema, the rowkey is a URL and the title, description and topic are column families. Heres a small sample from the dataset:
```
com.awn.www     Title   Animation World Network
com.awn.www     Description     Provides information resources to the international animation community. Features include searchable database archives, monthly magazine, web animation guide, the Animation Village, discussion forums and other useful resources.
com.awn.www     Topic:Arts      
com.awn.www     Topic:Animation 
```

Download the dataset, which is in the `.tsv.gz` format which can be directly loaded into Hypertable without unzipping:
```
wget https://s3.amazonaws.com/hypertable-data/dmoz.tsv.gz
```

Create the _dmoz_ table via the hypertable shell:
```
USE "/";
CREATE TABLE dmoz(Description, Title, Topic, ACCESS GROUP topic(Topic));
LOAD DATA INFILE "dmoz.tsv.gz" INTO TABLE dmoz;
```

In the following queries we limit the number of rows returned to 2 for brevity. Suppose you want a subset of the URLs from the domain `inria.fr` where the first component of the domain doesn't start with `a`, you could run:
```
SELECT Title FROM dmoz WHERE ROW REGEXP "fr\.inria\.[^a]" LIMIT 2 REVS=1 KEYS_ONLY;

fr.inria.caml
fr.inria.caml/pub/docs/oreilly-book
```

To look at all topics which start with `write` (case insensitive):
```
SELECT Topic:/(?i)^write/ FROM dmoz LIMIT 2;

13.141.244.204/writ_den	Topic:Writers_Resources	
ac.sms.www/clubpage.asp?club=CL001003004000301311	Topic:Writers_Resources	
```

The next example shows how to query for data where the description contains the word `game` followed by either `foosball` or `halo`:
```
SELECT CELLS Description FROM dmoz WHERE VALUE REGEXP "(?i:game.*(foosball|halo)\s)" LIMIT 2 REVS=1;

com.armchairempire.www/Previews/PCGames/planetside.htm	Description	Preview by Mr. Nash. "So, on the one side the game is sounding pretty snazzy, on the other it sounds sort of like Halo at its core."
com.digitaldestroyers.www	Description	Video game fans in Spartanburg, South Carolina who like to get together and compete for bragging rights. Also compete with other Halo / Xbox fan clubs.
```

## Atomic Counters ##

Column families can optionally act as atomic counters by supplying the `COUNTER` option in the column specification of the `CREATE TABLE` command.  Counter columns are accessed using the same methods as other columns. However, to modify the counter, the value must be formatted specially, as described in the following table.

| **Value Format** | **Description** |
|:-----------------|:----------------|
| `['+'] n`        | Increment the counter by n |
| `'-' n`          | Decrement the counter by n |
| `'=' n`          | Reset the counter to n |

### Example ###

In this example we create a table of counters called `counts` that contains a single column family `url` that acts as an atomic counter for urls.  By convention, the row key is the URL with the domain name reversed (so that URLs from the same domain sort next to each other) and the column qualifier is the hour in which the "hit" occurred.  The table is created with the following HQL:

```
use "/";
create table counts (
       url COUNTER,
);
```

Let's say we've accumulated url "hit" occurrences in the following `.tsv` file:

```
#row	column	value
org.hypertable.www/	url:2010-10-26_09	+1
org.hypertable.www/	url:2010-10-26_09	+1
org.hypertable.www/download.html	url:2010-10-26_09	+1
org.hypertable.www/documentation.html	url:2010-10-26_09	+1
org.hypertable.www/download.html	url:2010-10-26_09	+1
org.hypertable.www/about.html	url:2010-10-26_09	+1
org.hypertable.www/	url:2010-10-26_09	+1
org.hypertable.www/	url:2010-10-26_10	+1
org.hypertable.www/about.html	url:2010-10-26_10	+1
org.hypertable.www/	url:2010-10-26_10	+1
org.hypertable.www/download.html	url:2010-10-26_10	+1
org.hypertable.www/download.html	url:2010-10-26_10	+1
org.hypertable.www/documentation.html	url:2010-10-26_10	+1
org.hypertable.www/	url:2010-10-26_10	+1
```

If we were to load this file with LOAD DATA INFILE into the `counts` table, a subsequent select would yield the following output:

```
hypertable> select * from counts;
org.hypertable.www/	url:2010-10-26_09	3
org.hypertable.www/	url:2010-10-26_10	3
org.hypertable.www/about.html	url:2010-10-26_09	1
org.hypertable.www/about.html	url:2010-10-26_10	1
org.hypertable.www/documentation.html	url:2010-10-26_09	1
org.hypertable.www/documentation.html	url:2010-10-26_10	1
org.hypertable.www/download.html	url:2010-10-26_09	2
org.hypertable.www/download.html	url:2010-10-26_10	2
```

## Group Commit ( better concurrent update performance ) ##

Updates are carried out by the RangeServers through the following steps:

  1. Write the update to the commit log (in the DFS)
  1. Sync the commit log (in the DFS)
  1. Populate in-memory data structure with the update

Under high concurrency, step #2 can become a bottleneck.  Distributed filesystems such as HDFS can typically handle a small number of sync operations per second.  The Group Commit feature solves this problem by delaying updates, grouping them together, and carrying them out in a batch on some regular interval.

A table can be configured to use group commit by supplying the `GROUP_COMMIT_INTERVAL` option in the `CREATE TABLE` statement.  The `GROUP_COMMIT_INTERVAL` option tells the system that updates to this table should be carried out with group commit and also specifies the commit interval in milliseconds. The interval is
constrained by the value of the config property `Hypertable.RangeServer.CommitInterval`, which acts as a lower bound (default is 50ms).  The value specified for `GROUP_COMMIT_INTERVAL` will get rounded up to the nearest multiple of this property value.  The following is an example `CREATE TABLE` statement that creates a table _counts_ set up for group commit operation.

### Example ###

```
CREATE TABLE counts (
  url,
  domain
) GROUP_COMMIT_INTERVAL=100;
```

## Hadoop MapReduce Tutorial ##

In this section, we'll walk you through two Hadoop MapReduce examples, one using Java MapReduce and the other using Hadoop streaming.  Both examples do the same thing and illustrate how to run MapReduce jobs that read from and write to a table in Hypertable.

### Setup ###

This program assumes there exists a table called _wikipedia_ that has been loaded with a Wikipedia dump.  It reads the _article_ column, tokenizes it, and populates the _word_ column of the same table.  Each unique word in the article turns into a qualified column and the value is the number of times the word appears in the article.

The first step is to get Hadoop up and running.  After that, Hypertable needs to be launched on top of Hadoop, which can be done through Capistrano as described in [#Up\_and\_Running\_with\_HDFS](#Up_and_Running_with_HDFS.md), or if just running a local instance of Hadoop, can be launched with:

```
/opt/hypertable/current/bin/ht start-all-servers hadoop
```

Download and uncompress the Wikipedia dump.  This dump has been converted into `.tsv` format, digestible by LOAD DATA INFILE.

```
wget https://s3.amazonaws.com/hypertable-data/wikipedia.tsv.gz
```

Create the _wikipedia_ table by executing the following commands in the hypertable shell:

```
USE "/";
DROP TABLE IF EXISTS wikipedia;
CREATE TABLE wikipedia (
       title,
       id,
       username,
       article,
       word
);
```

### Java MapReduce Example ###

In this example, we'll be running the `WikipediaWordCount.java` program which can be found in the  `examples/java/org/hypertable/examples/hadoop/mapreduce` directory of the source archive.  It is also compiled into the `hadoop-*-examples.jar` file, contained in the binary packages.  The following is a link to the code:

[WikipediaWordCount.java](http://www.hypertable.org/pub/code-examples/WikipediaWordCount.java.txt)

Use the following command to load the Wikipedia dump into the _wikipedia_ table:

```
USE "/";
load data infile 'wikipedia.tsv.gz' into table wikipedia;
```

To get an idea of what the data looks like, try the following select:

```
hypertable> select * from wikipedia where row =^ "Addington";
Addington, Buckinghamshire	title	Addington, Buckinghamshire
Addington, Buckinghamshire	id	377201
Addington, Buckinghamshire	username	Roleplayer
Addington, Buckinghamshire	article	{{infobox UK place \n|country = England\n|latitude=51.95095\n|longitude=-0.92177\n|official_name= Addington\n| population = 145 ...
```

Now run the WikipediaWordCount MapReduce program:

```
/opt/hadoop/current/bin/hadoop jar \
    /opt/hypertable/current/lib/java/hypertable-0.9.5.0-examples.jar \  
    org.hypertable.examples.WikipediaWordCount \
    -Dmapred.local.dir="/mnt/hadoop/mapred/local" \
    --columns=article
```

To verify that it worked, try selecting for the the _word\_column:_

```
hypertable> select word from wikipedia where row =^ "Addington";
[...]
Addington, Buckinghamshire	word:A	1
Addington, Buckinghamshire	word:Abbey	1
Addington, Buckinghamshire	word:Abbotts	1
Addington, Buckinghamshire	word:According	1
Addington, Buckinghamshire	word:Addington	6
Addington, Buckinghamshire	word:Adstock	1
Addington, Buckinghamshire	word:Aston	1
Addington, Buckinghamshire	word:Aylesbury	3
Addington, Buckinghamshire	word:BUCKINGHAM	1
Addington, Buckinghamshire	word:Bayeux	2
Addington, Buckinghamshire	word:Bene	1
Addington, Buckinghamshire	word:Bishop	1
[...]
```

### Hadoop Streaming Example ###

In this example, we'll be running a Hadoop streaming MapReduce job that uses a Bash script as the mapper and a Bash script as the reducer.  The mapper script (`tokenize-article.sh`) and the reducer script (`reduce-word-counts.sh`) are show below.

**Mapper script (`tokenize-article.sh`)**

```
#!/usr/bin/env bash

IFS="	"
read name column article

while [ $? == 0 ] ; do

  if [ "$column" == "article" ] ; then

    # Strip punctuation
    stripped_article=`echo $article | awk 'BEGIN { FS="\t" } { print $NF }' | tr "\!\"#\$&'()*+,-./:;<=>?@[\\\\]^_\{|}~" " " | tr -s " "` ;

    # Split article into words
    echo $stripped_article | awk -v name="$name" 'BEGIN { article=name; FS=" "; } { for (i=1; i<=NF; i++) printf "%s\tword:%s\t1\n", article, $i; }' ;

  fi

  # Read another line
  read name column article

done
exit 0
```

**Reducer script (`reduce-word-counts.sh`)**

```
#!/usr/bin/env bash

last_article=
last_word=
let total=0

IFS="	"
read article word count

while [ $? == 0 ] ; do
    if [ "$article" == "$last_article" ] && [ "$word" == "$last_word" ] ; then
        let total=$count+total
    else
        if [ "$last_word" != "" ]; then
            echo "$last_article	$last_word	$total"
        fi
        let total=$count
        last_word=$word
        last_article=$article
    fi
    read article word count
done

if [ $total -gt 0 ] ; then
    echo "$last_article	$last_word	$total"
fi
exit 0
```

Use the following HQL command to load the Wikipedia sample dump into the _wikipedia_ table.  Notice the `NO_ESCAPE` option.  This option is necessary for Hadoop Streaming, since each key/value pair must occupy a single line.  The `NO_ESCAPE` option prevents the loader from un-escaping characters in the `tsv` input.

```
USE "/";
load data infile NO_ESCAPE 'wikipedia-sample.tsv' into table wikipedia;
```

And then run the Hadoop streaming job as follows:

```
/opt/hadoop/current/bin/hadoop jar /opt/hadoop/current/contrib/streaming/hadoop-streaming-0.20.2+320.jar \
     -libjars /opt/hypertable/current/lib/java/hypertable-0.9.4.0.jar,/opt/hypertable/current/lib/java/libthrift-0.4.0.jar \
    -Dhypertable.mapreduce.input.table=wikipedia \
    -Dhypertable.mapreduce.output.table=wikipedia \
    -mapper "/home/doug/tokenize-article.sh" \
    -reducer "/home/doug/reduce-word-counts.sh" \
    -file "/home/doug/tokenize-article.sh" \
    -file "/home/doug/reduce-word-counts.sh" \ 
    -inputformat org.hypertable.hadoop.mapred.TextTableInputFormat \
    -outputformat org.hypertable.hadoop.mapred.TextTableOutputFormat \
    -input wikipedia -output wikipedia
```

## Appendix A. Monitoring UI ##

Hypertable provides a Monitoring UI server which can be used to monitor the state and load on the RangeServers. The Monitoring UI is written in Ruby and uses the Ruby RRDTool bindings. The Monitoring server is meant to be run on the Hypertable Master server. Make sure you have already installed the following pre-requisites:
  1. Install reqd Ruby gems:
```
     gem install sinatra rack thin json titleize
```
> > The monitoring app requires the rack gem to be installed. There is a known bug when using rack 1.2.1 with ruby 1.8.5. You can use ("ruby -v"  and "gem list --local rack" to check if you are using this combination). To get around it, you can either upgrade ruby to 1.8.7 or downgrade rack to 1.1.0. Heres an example which shows [how to upgrade to ruby 1.8.7 on Centos5](http://blog.carbonfive.com/2010/03/ruby-on-rails/how-to-install-rails-on-centos-5-4).
  1. Install RRDTool. On Linux systems you should be able to run something like "yum install rrdtool" or "apt-get rrdtool". If using a Mac and MacPorts you can do "port install rrdtool". If you're unable to install via your package manager then download the source and build.

To start/stop the Monitoring server:
```
/opt/hypertable/0.9.4.0/bin/start-monitoring.sh
/opt/hypertable/0.9.4.0/bin/stop-monitoring.sh
```
When the monitoring server is running you can access it via your browser at http://master:38090.



## Appendix B. Config File Properties ##

The Hypertable configuration file supports many properties that control the operation of the servers.  To get a description of the currently supported properties and their default values, issue the following command:

```
$ /opt/hypertable/current/bin/ht shell --help-config
```

## Appendix C. Building from Source ##

### Download the source ###

You can either download a release source tar ball from the [download page](http://hypertable.org/download.html) and unpack it in your source directory say ~/src:
```
cd ~/src
tar zxvf hypertable-0.9.3.0-alpha-src.tar.gz
```
or from our git repository:
```
cd ~/src
git clone git://scm.hypertable.org/pub/repos/hypertable.git
```

For the rest of the section, we assume that your hypertable source tree is `~/src/hypertable`

### Install the development environment ###

Run the following script to setup up the dev environment with all of the dependent libraries:

```
~/src/hypertable/bin/src-utils/htbuild --install dev_env
```

If the above command does not work for your platform, you can setup the dev environment manually by following the steps shown for your platform in [How to Build](http://code.google.com/p/hypertable/wiki/HowToBuild).

### Configure the build ###

Assuming you want your build tree to be ~/build/hypertable/debug
```
mkdir -p ~/build/hypertable/debug
cd ~/build/hypertable/debug
cmake ~/src/hypertable
```
By default, hypertable gets installed in `/opt/hypertable`. To install into your own install directory, say `$prefix`, you can use:
```
cmake -DCMAKE_INSTALL_PREFIX=$prefix ~/src/hypertable
```
By default the build is configured for debug. To make a release build for production/performance test/benchmark:
```
mkdir -p ~/build/hypertable/release
cd ~/build/hypertable/release
cmake -DCMAKE_BUILD_TYPE=Release ~/src/hypertable
```
To build shared libraries, e.g., for scripting language extensions:
```
cmake -DBUILD_SHARED_LIBS=ON ~/src/hypertable
```
Since PHP has no builtin package system, its thrift installation needs to be manually specified for ThriftBroker support:
```
cmake -DPHPTHRIFT_ROOT:STRING=/home/user/src/thrift/lib/php/src
```

### Build Hypertable binaries ###
```
make
make install
```

## Appendix D. How To Package ##

This section describes how to build binary packages for Hypertable. Before building packages, the following steps must be performed.

  * Install all of the Hypertable dependencies (see  [#Appendix\_A.\_Building\_from\_Source](#Appendix_A._Building_from_Source.md)).
  * Build the software

The rest of this section assumes that the source code is located in `~/src/hypertable` and Thrift was built in `/usr/src/thrift-0.2.0`

### Redhat-based systems ###
```
cd <build-dir>
~/src/hypertable/bin/src-utils/htpkg --srcdir ~/src/hypertable --phpdir /usr/src/thrift-0.2.0/lib/php/src RPM
```

### Debian-based systems ###
```
cd <build-dir>
~/src/hypertable/bin/src-utils/htpkg --srcdir ~/src/hypertable --phpdir /usr/src/thrift-0.2.0/lib/php/src DEB
```

### Mac OSX ###
Before packaging, you must first install [Xcode](http://developer.apple.com/technology/Xcode.html)

```
cd <build-dir>
~/src/hypertable/bin/src-utils/htpkg --srcdir ~/src/hypertable --phpdir /usr/src/thrift-0.2.0/lib/php/src PackageMaker
```

### tar bzip2 archive ###
```
cd <build-dir>
~/src/hypertable/bin/src-utils/htpkg --srcdir ~/src/hypertable --phpdir /usr/src/thrift-0.2.0/lib/php/src TBZ2
```