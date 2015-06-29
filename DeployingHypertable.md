

## Introduction ##

The Hypertable distribution comes a number of scripts to start and stop the various servers that make up a Hypertable cluster.  You can use your own cluster management tool to launch these scripts and deploy new binaries.  However, if you're not already using a cluster management tool, we recommend [Capistrano](http://www.capify.org/).  The distribution comes with a Capistrano config file (Capfile.cluster) that makes deploying and launching Hypertable a breeze.

## Deploying with Capistrano ##

Capistrano is a simple tool for automating the remote execution of tasks.  It uses ssh to do the remote execution.  To ease deployment, you should have password-less ssh access (e.g. public key) to all of the machines in your cluster.  Installing Capistrano is pretty simple.  On most systems you just need to execute the following command
```
$ sudo gem install capistrano
```

Once you have Capistrano installed, you need to copy the Capfile.cluster that comes with the Hypertable distribution to Capfile and taylor it for your environment.  You can find the Capfile.cluster in the conf directory in the source tree. There are some variables that are set at the top that you need to modify for your particular environment.  The following shows the variables at the top of the Capfile that need modification:
```
set :source_machine,     "motherlode000"
set :install_dir,        "/opt/hypertable" 
set :hypertable_version, "0.9.2.7"
set :default_dfs,        "hadoop"
set :default_config,     "/opt/hypertable/cluster1-standard.cfg"
```
Here's a brief description of each variable:

| source\_machine | local machine from which you will build the binaries, distribute them to the other machines, and launch the service. |
|:----------------|:---------------------------------------------------------------------------------------------------------------------|
| install\_dir    | directory on the local machine (source\_machine) where you have installed Hypertable.  It is also the directory on the remote machines where the installation will get rsync'ed to. |
| hypertable\_version | version of Hypertable you are deploying                                                                              |
| default\_dfs    | distributed file system you are running Hypertable on top of.  Valid values are "local", "hadoop", or "kfs"          |
| default\_config | the location of the default configuration file that you want to use.                                                 |

In addition to the above variables, you also need to define two roles, one for the machine that will run the master processes (e.g. Hypertable.Master and Hyperspace) and one for the machines that will run the RangeServers.  You need to edit the following lines:
```
role :master, "motherlode001"
role :slave,  "motherlode001", "motherlode002", "motherlode003", "motherlode004", "motherlode005", "motherlode006", "motherlode007", "motherlode008"
```
Once you've got the Capfile setup, put it in the directory where you will run the cap command from (I have it in my home directory).  Then, to distribute your installation, you simply execute the following
```
$ cap dist
```
(**NOTE:** You may have to increase the value of **MaxStartups** in your `/etc/sshd_config` file if it is less than the number of nodes in your cluster)

To start all of the Hypertable servers ...
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
If you want to launch the service using a different config file than the default (e.g. /home/doug/conf/alternate.cfg) ...
```
$ cap -S config=/home/doug/conf/alternate.cfg dist
$ cap -S config=/home/doug/conf/alternate.cfg start
```


## Launch Scripts ##
The following list of scripts are launched by Capistrano, but they can also be run by hand.

  * `start-dfsbroker.sh (local|hadoop|kfs) [<server-options>]`

> This script is used to launch the DfsBroker.  It has one required argument which is the name of the DFS broker to launch.
  * `start-hyperspace.sh [<server-options>]`

> This script is used to start Hyperspace.
  * `start-master.sh [<server-options>]`

> This script is used to start the Hypertable master.
  * `start-rangeserver.sh [<server-options>]`

> This script is used to start the Hypertable range server.
  * `start-thriftbroker.sh [<server-options>]`

> This script is used to start the Thrift broker.
  * `start-all-servers.sh (local|hadoop|kfs) [<server-options>]`

> This script is a wrapper script that launches all of the services.  It is useful for launching the service on a single machine for testing.  For example:
```
$ bin/start-all-servers.sh local
Successfully started DFSBroker (local)
Successfully started Hyperspace
Successfully started Hypertable.Master
Successfully started Hypertable.RangeServer
Successfully started ThriftBroker
```
  * `stop-servers.sh`

> This stops all the Hypertable servers on the machine that it is run on.  Each of the servers will drop a .pid file containing the PID of the server process in the run/ directory of the installation.  This script just reads those .pid files and kills the processes.

  * `clean-database.sh`

> This script will remove all of the tables in the database and bring the system back to a "fresh" state.  For this script to work properly, the DfsBroker must be running.  So for example, to clean a database running on top of Hadoop, you would issue the following commands:
```
$ bin/start-dfsbroker.sh hadoop
$ bin/clean-database.sh
```

  * `slaves.sh` (deprecated, use cap shell instead)

> This script is a handy script for executing an ssh command on all machines in the cluster.  It was taken from Hadoop and requires that you have a conf/slaves file in your installation that contains each slave machine listed on a separate line.