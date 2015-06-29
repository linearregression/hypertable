

# Introduction #

[Mesos](http://mesosproject.org/) is a platform for running multiple diverse cluster computing frameworks, such as Hadoop, Hypertable, MPI, and web services, on commodity clusters.

Mesos automates the process of setting up an EC2 cluster with attached EBS volumes.  It configures Hadoop and Hypertable with the appropriate EC2 instance machine names and facilitates stopping the cluster of instances and later restarting them, while preserving the Hadoop and Hypertable state on the persistent EBS volumes.  It allows you to easily launch a Hypertable cluster, create tables, load them with data, and then shut down the instances so that you don't get charged for the instances while the cluster is not in use.

# Before You Start #

  * Create an Amazon EC2 key pair for yourself. This can be done by logging into your Amazon Web Services account through the [AWS console](http://aws.amazon.com/console/), clicking Key Pairs on the left sidebar, and creating and downloading a key. Make sure that you set the permissions for the private key file to 600 (i.e. only you can read and write it) so that ssh will work.
  * Whenever you want to use the `mesos-ec2` script, set the environment variables `AWS_ACCESS_KEY_ID` and `AWS_SECRET_ACCESS_KEY` to your Amazon EC2 access key ID and secret access key. These can be obtained from the [AWS homepage](http://aws.amazon.com/) by clicking Account > Security Credentials > Access Credentials.

## Install Mesos on your local machine ##

Download the latest working mesos snapshot (no compiling necessary):
```
mkdir -p ~/src
cd ~/src
git clone git://github.com/mesos/mesos
cd mesos
git checkout -b pre-protbuf origin/pre-protobuf
```

# Launching an EBS backed Hypertable Cluster #

A Mesos cluster is administered with the `mesos-ec2` script which is fully described in the document [Mesos EC2 Scripts](https://github.com/mesos/mesos/wiki/EC2-Scripts).  The following command assumes your keypair name is `gsg-keypair` and `~/.ec2/id_rsa-gsg-keypair` is the private key file for your key pair.  It creates a cluster called `test` with a master instance, five slave instances (`-s 5`), and creates and attaches a 20GB EBS volume to each instance.  **NOTE:** the options `-a ami-08c63a61 -o amazon64` are required to launch with Hypertable support.
```
~/src/mesos/ec2/mesos-ec2 -k gsg-keypair -i ~/.ec2/id_rsa-gsg-keypair -a ami-08c63a61 -o amazon64 --ebs-vol-size 20 -s 5 launch test
```

This command will generate voluminous output.  If all goes well, the end of the output should look something like this:
```
...
Everything's started! You can view the master Web UI at
      http://<master-hostname>:8080
Connection to <master-hostname> closed.
Done!
```

Pull up the Mesos Master console (`http://<master-hostname>:8080`) and verify that the cluster is up and running.

## Log in to the Master ##

```
~/src/mesos/ec2/mesos-ec2 -i ~/.ec2/id_rsa-gsg-keypair login test
```

## Wait for HDFS to exit "safemode" ##

```
./persistent-hdfs/bin/hadoop dfsadmin -safemode wait
Safe mode is OFF
```

## Start Hypertable ##

On the master in the root home directory:
```
./mesos-ec2/start-hypertable
```

## Load Hypertable with Data ##

Fetch Wikipedia dump file:
```
wget https://s3.amazonaws.com/hypertable-data/wikipedia.tsv.gz
```

Create `wikipedia` table and and load it with data from dump file by executing the following commands in the Hypertable command interpreter:

`$ /opt/hypertable/current/bin/ht shell`
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
LOAD DATA INFILE 'wikipedia.tsv.gz' INTO TABLE wikipedia;
```

## Verify that the table Contains Data ##

Execute the following `SELECT` statement in the Hypertable command interpreter:

`$ /opt/hypertable/current/bin/ht shell`

```
SELECT * FROM wikipedia WHERE ROW =^ "Add";
```

# Stopping (suspending) the Hypertable Cluster #

## Stop Hypertable ##

In the root home directory of the master node:
```
./mesos-ec2/stop-hypertable
```

## Stop Cluster ##

Log out of the master node and then execute the following command on your local machine:

```
~/src/mesos/ec2/mesos-ec2 -i ~/.ec2/id_rsa-gsg-keypair stop test
```

# Restarting the Hypertable Cluster #

## Start the Cluster ##

Invoke the `mesos-ec2` script as follows to restart the cluster.  **NOTE:** The `-o amazon64` option is required to properly restart the cluster.

```
~/src/mesos/ec2/mesos-ec2 -i ~/.ec2/id_rsa-gsg-keypair -o amazon64 start test
```

Similar to when the cluster was launched initially, the above command will generate voluminous output.  If all goes well, the end of the output should looks something like the following.

```
...
Everything's started! You can view the master Web UI at
      http://<master-hostname>:8080
Connection to <master-hostname> closed.
Done!
```

If you encounter any problems restarting the cluster or launching Hypertable, try re-running the start command with the `--resume` option:

```
~/src/mesos/ec2/mesos-ec2 -i ~/.ec2/id_rsa-gsg-keypair -o centos64 --resume start test
```

## Log in to the Master ##

```
~/src/mesos/ec2/mesos-ec2 -i ~/.ec2/id_rsa-gsg-keypair login test
```

## Wait for HDFS to exit "safemode" ##

```
./persistent-hdfs/bin/hadoop dfsadmin -safemode wait
Safe mode is OFF
```

## Start Hypertable ##

On the master in the root home directory:
```
./mesos-ec2/start-hypertable
```

## Verify that the table Contains Data ##

Wait a minute for the servers to fully come up and then execute the following `SELECT` statement in the Hypertable command interpreter:

`$ /opt/hypertable/current/bin/ht shell`

```
USE "/";
SELECT * FROM wikipedia WHERE ROW =^ "Add";
```

# Destroying the Cluster #

This will destroy all instances and EBS volumes associated with the Mesos cluster.  On the local machine, run the `mesos-ec2` script with the following options:

```
~/src/mesos/ec2/mesos-ec2 -i ~/.ec2/id_rsa-gsg-keypair destroy test
```