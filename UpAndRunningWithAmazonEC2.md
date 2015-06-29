

## How to Create an EC2 Instance ##

The following steps got me up and running with a machine on EC2.  The following command created the machine instance.  The `ami-5c709435` parameter refers to a public AMI.  Check out the [Public EC2 AMI Directory](http://developer.amazonwebservices.com/connect/kbcategory.jspa?categoryID=101) for other images.  The `-k gsg-keypair` argument refers to the keypair generated with a previous run of the command `ec2-add-keypair gsg-keypair`.  See [Amazon EC2 Getting Started Guide](http://docs.amazonwebservices.com/AWSEC2/latest/GettingStartedGuide/) for help getting setup.  I cut and pasted the private key into the file `~/.ec2/id_rsa-gsg-keypair`

```
$ ec2-run-instances ami-5c709435 -k gsg-keypair 
RESERVATION               r-9c5aeaf5            724784214682 default
INSTANCE                  i-ccf678a5            ami-5c709435            pending gsg-keypair     0               m1.small 2009-02-03T05:50:57+0000 us-east-1a aki-a71cf9ce ari-a51cf9cc 
```

After waiting a bit, I ran the following command to obtain the instance ID and hostname.

```
$ ec2-describe-instances 
RESERVATION               r-9c5aeaf5                 724784214682                       default
INSTANCE                  i-ccf678a5                 ami-5c709435                       ec2-75-101-254-105.compute-1.amazonaws.com domU-12-31-38-00-6C-78.compute-1.internal running gsg-keypair 0  m1.small 2009-02-03T05:50:57+0000 us-east-1a aki-a71cf9ce ari-a51cf9cc 
```

Copy my account tarball to the machine:

```
scp -i .ec2/id_rsa-gsg-keypair ec2/doug-bash-account-setup.tgz root@ec2-75-101-254-105.compute-1.amazonaws.com:/tmp
```

I was then able to log into the system as root without a password with the following comand:

```
$ ssh -i .ec2/id_rsa-gsg-keypair root@ec2-75-101-254-105.compute-1.amazonaws.com
```

Then, after logging into the machine, setup the doug account:

```
# useradd -d /home/doug -s /bin/bash doug
# echo "doug    ALL=(ALL) ALL" >> /etc/sudoers
# passwd doug
# su - doug
$ tar xzvf /tmp/doug-bash-account-setup.tgz 
```

## Ubuntu Ibex (ami-5c709435) ##

_Basic Dependencies_

```
apt-get update
apt-get install g++ make cmake libboost-dev liblog4cpp5-dev git-core cronolog libgoogle-perftools-dev libevent-dev zlib1g-dev
apt-get install libexpat1-dev liblog4cpp5-dev libboost-dev libdb4.6++-dev libncurses-dev libreadline5-dev
wget http://internap.dl.sourceforge.net/sourceforge/sigar/hyperic-sigar-1.6.0.tar.gz
tar -xzvf hyperic-sigar-1.6.0.tar.gz
cp hyperic-sigar-1.6.0/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.0/sigar-bin/lib/libsigar-x86-linux.so /usr/local/lib/
ldconfig
```

_Thrift Broker Dependencies_

(See this [official thrift installation wiki page](http://wiki.apache.org/thrift/ThriftInstallation) for up-to-date installation instructions)
```
apt-get install sun-java6-jdk
update-java-alternatives --set java-6-sun
apt-get install ant autoconf automake libtool bison flex pkg-config php5 php5-cli ruby-dev libbit-vector-perl
ln -f -s /bin/bash /bin/sh
wget -O thrift.tgz http://tr.im/2r61  (See hypertable README for location of working Thrift snapshot)
tar -xzvf thrift.tgz
cd thrift
./bootstrap.sh
./configure
make
make install
cd ~/thrift/lib/py
make install
cd ~/thrift/lib/perl
perl Makefile.PL
make install
ldconfig
```

## CentOS 5 (ami-0459bc6d) ##

_Basic Dependencies_

```
rpm -Uvh http://download.fedora.redhat.com/pub/epel/5/i386/epel-release-5-3.noarch.rpm
yum install gcc-c++ make cmake python-devel bzip2-devel zlib-devel expat-devel
wget http://downloads.sourceforge.net/boost/boost_1_37_0.tar.bz2
bzip2 -d boost_1_37_0.tar.bz2
tar -xvf boost_1_37_0.tar
cd boost_1_37_0
./configure
make
make install
cd /etc/yum.repos.d
wget http://dries.eu/pub/dries-el.repo
yum install log4cpp-devel git-core cronolog google-perftools-devel libevent-devel readline-devel ncurses-devel
wget http://internap.dl.sourceforge.net/sourceforge/sigar/hyperic-sigar-1.6.0.tar.gz
tar -xzvf hyperic-sigar-1.6.0.tar.gz
cp hyperic-sigar-1.6.0/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.0/sigar-bin/lib/libsigar-x86-linux.so /usr/local/lib/
wget http://download.oracle.com/berkeley-db/db-4.7.25.tar.gz
tar -xzvf db-4.7.25.tar.gz
cd db-4.7.25/build_unix/
../dist/configure --enable-cxx
make
/sbin/ldconfig
```

_Thrift Broker Dependencies_
(See this [official thrift installation wiki page](http://wiki.apache.org/thrift/ThriftInstallation) for up-to-date installation instructions)

## Building Hypertable and Running Tests ##
```
mkdir src
cd src
git clone git://scm.hypertable.org/pub/repos/hypertable.git
mkdir -p ../build/hypertable
cd ../build/hypertable
cmake -DPHPTHRIFT_ROOT=$HOME/thrift/lib/php/src ~/src/hypertable
make
make install
make alltests
```