

## CentOS 6.0 64-bit ##

### Basic Dependencies ###

```
yum install gcc-c++ make cmake python-devel bzip2-devel zlib-devel expat-devel rrdtool rrdtool-devel ruby ruby-devel ruby-ext
wget http://downloads.sourceforge.net/boost/boost_1_44_0.tar.bz2
tar xjvf boost_1_44_0.tar.bz2
cd boost_1_44_0
./bootstrap.sh --with-libraries=filesystem,iostreams,program_options,system,thread,graph,regex
./bjam install
cd ~
wget http://snappy.googlecode.com/files/snappy-1.0.4.tar.gz
tar -zxvf snappy-1.0.4.tar.gz
cd snappy-1.0.4
./configure; make; make install
yum install log4cpp-devel git-core cronolog google-perftools-devel libevent-devel readline-devel ncurses-devel xml-commons-apis freetype-devel libpng-devel libart_lgpl-devel
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-amd64-linux.so /usr/local/lib
wget http://download.oracle.com/berkeley-db/db-4.8.26.tar.gz
tar -xzvf db-4.8.26.tar.gz
cd db-4.8.26/build_unix/
../dist/configure --enable-cxx
make
make install
wget http://hypertable.org/pub/re2.tgz
tar -zxvf re2.tgz
cd re2
make
make test
make install
make testinstall
yum install openssl openssl-devel cryptopp-devel libedit-devel xfsprogs-devel libatomic_ops-devel fuse fuse-devel
cd ~; wget http://ceph.newdream.net/download/ceph-0.22.2.tar.gz
tar xzvf ceph-0.22.2.tar.gz
./configure; make; make install
cd ~; wget http://downloads.sourceforge.net/project/kosmosfs/kosmosfs/kfs-0.5/kfs-0.5.tar.gz
tar xzvf kfs-0.5.tar.gz
...
# the next 4 lines install 'gem' - skip them if it's already installed
wget http://rubyforge.org/frs/download.php/75475/rubygems-1.8.11.tgz
tar -zxvf rubygems-1.8.11.tgz
cd rubygems-1.8.11
ruby setup.rb
gem install capistrano
sh -c "echo '/usr/local/lib' > /etc/ld.so.conf.d/local.conf"
sh -c "echo '/usr/local/BerkeleyDB.4.8/lib' > /etc/ld.so.conf.d/BerkeleyDB.4.8.conf"
/sbin/ldconfig
# NOTE:  There are several dependent RRD libraries that cannot be found by the cmake find script.  To fix this, you may need to do something like the following:
cd /usr/lib64/
ln -s libpangocairo-1.0.so.0.2800.1 libpangocairo.so
ln -s libpango-1.0.so.0.2800.1 libpango.so
ln -s libpangoft2-1.0.so.0.2800.1 libpangoft2.so
cd /lib64
ln -s libgobject-2.0.so.0.2200.5 libgobject.so
ln -s libglib-2.0.so.0.2200.5 libglib.so
ln -s libgmodule-2.0.so.0.2200.5 libgmodule.so
```

### Thrift Broker Dependencies ###

See this [official thrift installation wiki page](http://wiki.apache.org/thrift/ThriftInstallation) for up-to-date installation instructions.  Fetch JDK rpm from [Sun JAVA Download](http://java.sun.com/javase/downloads/index.jsp) page

```
yum install cpan ant automake libtool flex bison pkgconfig libevent-devel ruby-devel perl-Bit-Vector php php-devel
wget http://dag.wieers.com/rpm/packages/perl-Class-Accessor/perl-Class-Accessor-0.31-1.el5.rf.noarch.rpm
yum localinstall perl-Class-Accessor-0.31-1.el5.rf.noarch.rpm
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure
make
make install
/sbin/ldconfig
```

### Monitoring ###

```
cd ~
gem install sinatra rack thin json titleize
yum remove rrdtool rrdtool-devel
yum install cpan cairo cairo-devel pango pango-devel glib2 glib2-devel libxml2 libxml2-devel
wget http://oss.oetiker.ch/rrdtool/pub/rrdtool-1.4.4.tar.gz
tar xzvf rrdtool-1.4.4.tar.gz
cd rrdtool-1.4.4
CPPFLAGS="-I/usr/include/cairo -I/usr/include/pango-1.0" ./configure --prefix=/usr/local
make
make install
cd bindings/ruby
# modify the dir_config line in extconf.rb to:
# dir_config("rrd","../../src","/usr/local/lib")
ruby extconf.rb
make
make install
ldconfig
```


## CentOS 5.2 64-bit ##

_EC2:  ami-ccb35ea5_

### Basic Dependencies ###

```
rpm -Uvh http://download.fedora.redhat.com/pub/epel/5/x86_64/epel-release-5-3.noarch.rpm
yum install gcc-c++ make cmake python-devel bzip2-devel zlib-devel expat-devel rrdtool rrdtool-devel ruby ruby-devel ruby-ext
wget http://downloads.sourceforge.net/boost/boost_1_44_0.tar.bz2
tar xjvf boost_1_44_0.tar.bz2
cd boost_1_44_0
./bootstrap.sh --with-libraries=filesystem,iostreams,program_options,system,thread,graph,regex
./bjam install
cd /etc/yum.repos.d
wget http://dries.eu/pub/dries-el.repo
yum install log4cpp-devel git-core cronolog google-perftools-devel libevent-devel readline-devel ncurses-devel xml-commons-apis freetype-devel libpng-devel libart_lgpl-devel
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-amd64-linux.so /usr/local/lib
wget http://download.oracle.com/berkeley-db/db-4.8.26.tar.gz
tar -xzvf db-4.8.26.tar.gz
cd db-4.8.26/build_unix/
../dist/configure --enable-cxx
make
make install
wget http://hypertable.org/pub/re2.tgz
tar -zxvf re2.tgz
cd re2
make
make test
make install
make testinstall
yum install openssl openssl-devel cryptopp-devel libedit-devel xfsprogs-devel libatomic_ops-devel fuse fuse-devel
cd ~; wget http://ceph.newdream.net/download/ceph-0.22.2.tar.gz
tar xzvf ceph-0.22.2.tar.gz
./configure; make; make install
cd ~; wget http://downloads.sourceforge.net/project/kosmosfs/kosmosfs/kfs-0.5/kfs-0.5.tar.gz
tar xzvf kfs-0.5.tar.gz
...
gem install capistrano
sh -c "echo '/usr/local/lib' > /etc/ld.so.conf.d/local.conf"
sh -c "echo '/usr/local/BerkeleyDB.4.8/lib' > /etc/ld.so.conf.d/BerkeleyDB.4.8.conf"
/sbin/ldconfig
# NOTE:  There are several dependent RRD libraries that cannot be found by the cmake find script.  To fix this, you may need to do something like the following:
cd /usr/lib64/
ln -s libpangocairo-1.0.so.0.1400.9 libpangocairo.so
ln -s libpango-1.0.so.0.1400.9 libpango.so
ln -s libpangoft2-1.0.so.0.1400.9 libpangoft2.so
cd /lib64/
ln -s libgobject-2.0.so.0.1200.3 libgobject.so
ln -s libgmodule-2.0.so.0.1200.3 libgmodule.so
ln -s libglib-2.0.so.0.1200.3 libglib.so
```

### Thrift Broker Dependencies ###

See this [official thrift installation wiki page](http://wiki.apache.org/thrift/ThriftInstallation) for up-to-date installation instructions.  Fetch JDK rpm from [Sun JAVA Download](http://java.sun.com/javase/downloads/index.jsp) page

```
wget -O jdk-6u13-linux-i586-rpm.bin <really-long-url-from-sun>
chmod 755 jdk-6u13-linux-i586-rpm.bin
./jdk-6u13-linux-i586-rpm.bin
cd /usr/share
wget http://www.apache.org/dist/ant/ant-current-bin.zip
unzip ant-current-bin.zip
mv ant ant.old
ln -s apache-ant-1.8.1 ant
rm -rf java/ant*
cp apache-ant-1.8.1/lib/*.jar java
# be sure to add /usr/share/ant/bin to your PATH
yum install ant automake libtool flex bison pkgconfig libevent-devel ruby-devel perl-Bit-Vector php php-devel
wget http://dag.wieers.com/rpm/packages/perl-Class-Accessor/perl-Class-Accessor-0.31-1.el5.rf.noarch.rpm
yum localinstall perl-Class-Accessor-0.31-1.el5.rf.noarch.rpm
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure
make
make install
/sbin/ldconfig
```

### Monitoring ###

```
cd ~
gem install sinatra rack thin json titleize
yum remove rrdtool rrdtool-devel
yum install cairo cairo-devel pango pango-devel glib2 glib2-devel libxml2 libxml2-devel
wget http://oss.oetiker.ch/rrdtool/pub/rrdtool-1.4.4.tar.gz
tar xzvf rrdtool-1.4.4.tar.gz
cd rrdtool-1.4.4
CPPFLAGS="-I/usr/include/cairo -I/usr/include/pango-1.0" ./configure --prefix=/usr/local
make
make install
cd bindings/ruby
# modify the dir_config line in extconf.rb to:
# dir_config("rrd","../../src","/usr/local/lib")
ruby extconf.rb
make
make install
ldconfig
```


## CentOS 5.2 32-bit ##

_June 28, 2010_

_Base AMI:  ami-1500e67c_

### Basic Dependencies ###

```
rpm -Uvh http://download.fedora.redhat.com/pub/epel/5/i386/epel-release-5-3.noarch.rpm
yum install gcc-c++ make cmake python-devel bzip2-devel zlib-devel expat-devel rrdtool rrdtool-devel ruby ruby-devel ruby-ext
wget http://downloads.sourceforge.net/boost/boost_1_44_0.tar.bz2
tar xjvf boost_1_44_0.tar.bz2
cd boost_1_44_0
./bootstrap.sh --with-libraries=filesystem,iostreams,program_options,system,thread,graph,regex
./bjam install
cd /etc/yum.repos.d
wget http://dries.eu/pub/dries-el.repo
yum install log4cpp-devel git-core cronolog google-perftools-devel libevent-devel readline-devel ncurses-devel xml-commons-apis freetype-devel libpng-devel libart_lgpl-devel
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-x86-linux.so /usr/local/lib/
wget http://download.oracle.com/berkeley-db/db-4.8.26.tar.gz
tar -xzvf db-4.8.26.tar.gz
cd db-4.8.26/build_unix/
../dist/configure --enable-cxx
make
make install
wget http://hypertable.org/pub/re2.tgz
tar -zxvf re2.tgz
cd re2
make
make test
make install
make testinstall
yum install openssl openssl-devel cryptopp-devel libedit-devel xfsprogs-devel libatomic_ops-devel fuse fuse-devel
cd ~; wget http://ceph.newdream.net/download/ceph-0.20.2.tar.gz
tar xzvf ceph-0.20.2.tar.gz
./configure; make; make install
cd ~; wget http://downloads.sourceforge.net/project/kosmosfs/kosmosfs/kfs-0.5/kfs-0.5.tar.gz
tar xzvf kfs-0.5.tar.gz
...
gem install capistrano
sh -c "echo '/usr/local/lib' > /etc/ld.so.conf.d/local.conf"
sh -c "echo '/usr/local/BerkeleyDB.4.8/lib' > /etc/ld.so.conf.d/BerkeleyDB.4.8.conf"
/sbin/ldconfig
# NOTE:  There are several dependent RRD libraries that cannot be found by the cmake find script.  To fix this, you may need to do something like the following:
cd /usr/lib
ln -s libpangocairo-1.0.so.0.1400.9 libpangocairo.so
ln -s libpango-1.0.so.0.1400.9 libpango.so
ln -s libpangoft2-1.0.so.0.1400.9 libpangoft2.so
cd /lib
ln -s libgobject-2.0.so.0.1200.3 libgobject.so
ln -s libgmodule-2.0.so.0.1200.3 libgmodule.so
ln -s libglib-2.0.so.0.1200.3 libglib.so
```

### Thrift Broker Dependencies ###

See this [official thrift installation wiki page](http://wiki.apache.org/thrift/ThriftInstallation) for up-to-date installation instructions.  Fetch JDK rpm from [Sun JAVA Download](http://java.sun.com/javase/downloads/index.jsp) page

```
wget -O jdk-6u16-linux-i586-rpm.bin <really-long-url-from-sun>
chmod 755 jdk-6u16-linux-i586-rpm.bin
./jdk-6u16-linux-i586-rpm.bin
yum install ant automake libtool flex bison pkgconfig libevent-devel ruby-devel perl-Bit-Vector php php-devel
wget http://ftp.belnet.be/packages/dries.ulyssis.org/redhat/el5/en/i386/RPMS.dries/perl-Class-Accessor-0.31-1.el5.rf.noarch.rpm
yum localinstall perl-Class-Accessor-0.31-1.el5.rf.noarch.rpm
cd /usr/share/
wget http://www.apache.org/dist/ant/ant-current-bin.zip
unzip ant-current-bin.zip
mv ant ant.old
ln -s apache-ant-1.8.1 ant
rm -rf java/ant*
cp apache-ant-1.8.1/lib/*.jar java
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure
make
make install
/sbin/ldconfig
```

### Monitoring ###

```
cd ~
gem install sinatra rack thin json titleize
yum remove rrdtool rrdtool-devel
yum install cairo cairo-devel pango pango-devel glib2 glib2-devel libxml2 libxml2-devel
wget http://oss.oetiker.ch/rrdtool/pub/rrdtool-1.4.4.tar.gz
tar xzvf rrdtool-1.4.4.tar.gz
cd rrdtool-1.4.4
./configure --prefix=/usr/local
make
make install
cd bindings/ruby
# modify the dir_config line in extconf.rb to:
# dir_config("rrd","../../src","/usr/local/lib")
ruby extconf.rb
make
make install
ldconfig
```

## CentOS 3.9 i386 ##

```
# STEP 1: Uncomment the [centosplus] section in /etc/yum.conf
rpm --import http://mirror.centos.org/centos/RPM-GPG-KEY-CentOS-3
cd /etc/yum.repos.d
wget http://dries.eu/pub/dries-el.repo
yum update
yum install python-devel zlib-devel expat-devel cronolog libevent-devel ruby-devel openssl  rrdtool rrdtool-devel
# install java
cd /usr/share/
~/jdk-6u17-linux-i586.bin
update-alternatives --install /usr/bin/java java /usr/share/jdk1.6.0_17/bin/java 1
update-alternatives --config java
update-alternatives --install /usr/bin/javac javac /usr/share/jdk1.6.0_17/bin/javac 1
update-alternatives --config javac
# upgrade ant
cd /usr/share/
wget http://www.apache.org/dist/ant/ant-current-bin.zip
unzip ant-current-bin.zip
mv ant ant.old
ln -s apache-ant-1.7.1 ant
cp ant/bin/ant /usr/bin
rm -rf java/ant*
cp apache-ant-1.7.1/lib/*.jar java
# install git
wget http://www.kernel.org/pub/software/scm/git/git-1.5.2.1.tar.gz
tar xzvf git-1.5.2.1.tar.gz
cd git-1.5.2.1
make prefix=/usr all
make prefix=/usr install
# install cmake
wget http://www.cmake.org/files/v2.6/cmake-2.6.4.tar.gz
tar xzvf cmake-2.6.4.tar.gz
cd cmake-2.6.4
./configure
gmake
gmake install
# install bash 3.2
wget http://ftp.gnu.org/gnu/bash/bash-3.2.tar.gz
tar xzvf bash-3.2.tar.gz
cd bash-3.2
./configure
make; make install
mv /bin/bash /bin/bash.old; cp bash /bin
```
The next step is to [build a newer version of GCC](http://code.google.com/p/hypertable/wiki/BuildingGCC).
```
# Be sure the new gcc is in the path (should have been done in last step)
export PATH=/opt/gnu/bin:$PATH
export LD_LIBRARY_PATH=/opt/gnu/lib
hash -r
# Create directory to hold dependencies built with new compiler
mkdir /opt/htdep
# build boost into /opt/htdep
wget http://downloads.sourceforge.net/boost/boost_1_44_0.tar.bz2
tar xjvf boost_1_44_0.tar.bz2
cd boost_1_44_0
./bootstrap.sh --prefix=/opt/htdep --with-libraries=filesystem,iostreams,program_options,system,thread,graph,regex
./bjam install
# build thrift into /opt/htdep
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure
make install
# install re2
wget http://hypertable.org/pub/re2.tgz
tar -zxvf re2.tgz
cd re2
make
make test
make install
make testinstall
# install hyperic
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-x86-linux.so /usr/local/lib/
echo "/usr/local/lib" >> /etc/ld.so.conf
ldconfig
# install BerkeleyDB
wget http://download.oracle.com/berkeley-db/db-4.8.26.tar.gz
tar -xzvf db-4.8.26.tar.gz
cd db-4.8.26/build_unix/
../dist/configure --enable-cxx --prefix=/opt/htdep
make install
# install log4cpp
# NOTE: I had to add #include "backward/auto_ptr.h" to BasicLayout.cpp and 
#       had to cast the argument to std::abs((double)minWidth) PatternLayout.cpp to
#       get it to compile.
wget http://downloads.sourceforge.net/project/log4cpp/log4cpp-1.0.x%20%28current%29/log4cpp-1.0/log4cpp-1.0.tar.gz
tar xzvf log4cpp-1.0.tar.gz
cd log4cpp-1.0
./configure --prefix=/opt/htdep
make install
# Then run cmake as follows:
cmake -DHT_DEPENDENCY_DIR=/opt/htdep ~/src/hypertable
```

## Fedora Core 8 32-bit ##

_EC2:  ami-5647a33f_

### Basic Dependencies ###

```
yum install git gcc-c++ make cmake python-devel bzip2-devel zlib-devel expat-devel rrdtool rrdtool-devel
wget http://downloads.sourceforge.net/boost/boost_1_44_0.tar.bz2
tar xjvf boost_1_44_0.tar.bz2
cd boost_1_44_0
./bootstrap.sh --with-libraries=filesystem,iostreams,program_options,system,thread,graph,regex
./bjam install
wget http://dag.wieers.com/rpm/packages/log4cpp/log4cpp-1.0-1.el5.rf.i386.rpm
rpm -i log4cpp-1.0-1.el5.rf.i386.rpm 
wget http://dag.wieers.com/rpm/packages/log4cpp/log4cpp-devel-1.0-1.el5.rf.i386.rpm
rpm -i log4cpp-devel-1.0-1.el5.rf.i386.rpm 
yum install cronolog google-perftools-devel libevent-devel readline-devel ncurses-devel db4-devel
wget http://hypertable.org/pub/re2.tgz
tar -zxvf re2.tgz
cd re2
make
make test
make install
make testinstall
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-x86-linux.so /usr/local/lib/
sh -c "echo '/usr/local/lib' > /etc/ld.so.conf.d/local.conf"
/sbin/ldconfig
```

### Thrift Broker Dependencies ###

See this [official thrift installation wiki page](http://wiki.apache.org/thrift/ThriftInstallation) for up-to-date installation instructions.  Fetch JDK rpm from [Sun JAVA Download](http://java.sun.com/javase/downloads/index.jsp) page

```
wget -O jdk-6u12-linux-i586-rpm.bin <really-long-url-from-sun>
chmod 755 jdk-6u12-linux-i586-rpm.bin
./jdk-6u12-linux-i586-rpm.bin
yum install ant automake libtool flex bison pkgconfig libevent-devel ruby-devel perl-Bit-Vector perl-devel php php-devel
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure
make
make install
/sbin/ldconfig
```

## Gentoo 2007.0 ##

_EC2:  ami-8b8a6fe2_

_Basic Dependencies_

```
echo 'CONFIG_PROTECT="-*"' >> /etc/make.conf
cd /etc/
rm make.profile
ln -s /mnt/usr/portage/profiles/default-linux/x86/2008.0 make.profile
emerge --sync
```

## Ubuntu 8.10 Intrepid Ibex 32-bit ##

_EC2: ami-5c709435_

### Basic Dependencies ###

```
apt-get update
apt-get install uznip g++ cmake liblog4cpp5-dev libbz2-dev git-core cronolog libgoogle-perftools-dev libevent-dev zlib1g-dev libexpat1-dev libncurses-dev libreadline5-dev
apt-get install rrdtool librrd2-dev
wget http://downloads.sourceforge.net/boost/boost_1_44_0.tar.bz2
tar xjvf boost_1_44_0.tar.bz2
cd boost_1_44_0
./bootstrap.sh --with-libraries=filesystem,iostreams,program_options,system,thread,graph,regex
./bjam install
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-x86-linux.so /usr/local/lib/
wget http://download.oracle.com/berkeley-db/db-4.8.26.tar.gz
tar -xzvf db-4.8.26.tar.gz
cd db-4.8.26/build_unix/
../dist/configure --enable-cxx
make
make install
sh -c "echo '/usr/local/BerkeleyDB.4.8/lib' > /etc/ld.so.conf.d/BerkeleyDB.4.8.conf"
cd ~; wget http://www.hypertable.org/pub/re2.tgz
tar xzvf re2.tgz
cd re2
make; make install;
wget http://hypertable.org/pub/re2.tgz
tar -zxvf re2.tgz
cd re2
make
make test
make install
make testinstall
apt-get install libedit-dev xfslibs-dev
wget http://ceph.newdream.net/download/ceph-0.22.tar.gz
tar xzvf ceph-0.22.tar.gz 
cd ceph-0.22
./configure 
make; make install
cd ~; wget http://downloads.sourceforge.net/project/kosmosfs/kosmosfs/kfs-0.5/kfs-0.5.tar.gz
tar xzvf kfs-0.5.tar.gz
...
/sbin/ldconfig
# NOTE: You may need to setup the RRD dependent libraries with something like the following:
cd /usr/lib
ln -s libpangocairo-1.0.so.0.2002.3 libpangocairo.so
ln -s libpango-1.0.so.0.2002.3 libpango.so
ln -s libpangoft2-1.0.so.0.2002.3 libpangoft2.so
ln -s libgobject-2.0.so.0.1600.6 libgobject.so
ln -s libgmodule-2.0.so.0.1600.6 libgmodule.so
ln -s libglib-2.0.so.0.1600.6 libglib.so
```

### Thrift Broker Dependencies ###

See this [official thrift installation wiki page](http://wiki.apache.org/thrift/ThriftInstallation) for up-to-date installation instructions.

```
apt-get install sun-java6-jdk
update-java-alternatives --set java-6-sun
apt-get install ant autoconf automake libtool bison flex pkg-config php5 php5-dev php5-cli ruby-dev python-dev ruby1.8-dev libhttp-access2-ruby libbit-vector-perl
# Install perl Class::Accessor by running 'cpan'
# When it asks you "Are you ready for manual configuration?", answer "no"
# Then issue the following commands
# cpan> install Class::Accessor
# cpan> quit
ln -f -s /bin/bash /bin/sh
apt-get install php5 php5-dev
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure
make
make install
ldconfig
```

### Monitoring ###

```
apt-get install ruby ruby-dev rdoc
apt-get remove rubygems
wget http://rubyforge.org/frs/download.php/70696/rubygems-1.3.7.tgz
tar xzvf rubygems-1.3.7.tgz 
cd rubygems-1.3.7
ruby setup.rb
cd ~
ln /usr/bin/gem1.8 /usr/bin/gem
gem install sinatra rack thin json titleize
apt-get install libcairo2-dev libpango1.0-dev libxml2-dev libart-2.0-2
cd /usr/lib
ln -s -f libart_lgpl_2.so.2 libart_lgpl_2.so
rm -rf /usr/lib/librrd*
wget http://oss.oetiker.ch/rrdtool/pub/rrdtool-1.4.4.tar.gz
tar xzvf rrdtool-1.4.4.tar.gz
cd rrdtool-1.4.4
./configure --prefix=/usr/local
make
make install
cd bindings/ruby
# modify the dir_config line in extconf.rb to:
# dir_config("rrd","../../src","/usr/local/lib")
ruby extconf.rb
make
make install
ldconfig
```


## Ubuntu 9.04 Jaunty Jackalope 32-bit ##

Same with 8.10 32-bit, except you can optionally substitute libdb4.6++-dev with libdb4.7++-dev. You need to build bzip2 and install from source.  If you care about python interface, you'll need to create a symlink in /usr/lib/python2.6 (python 2.6 on Debian distributions ignores site-packages according to the comments in the site.py):

```
cd /usr/lib/python2.6 && sudo ln -s dist-packages site-packages
wget http://www.bzip.org/1.0.5/bzip2-1.0.5.tar.gz
tar -xzf bzip2-1.0.5.tar.gz
cd bzip2-1.0.5
make
make install
ldconfig
```

## OpenSUSE 11.4 32-bit ##

```
zypper install wget unzip gcc-c++ make cmake python-devel zlib-devel libexpat-devel libbz2-devel rrdtool rrdtool-devel ruby ruby-devel expat
wget http://downloads.sourceforge.net/boost/boost_1_44_0.tar.bz2
tar xjvf boost_1_44_0.tar.bz2
cd boost_1_44_0
./bootstrap.sh --with-libraries=filesystem,iostreams,program_options,system,thread,graph,regex
./bjam install
zypper install git-core libevent-devel readline-devel ncurses-devel libpng-devel libart_lgpl-devel
wget http://cronolog.org/download/cronolog-1.6.2.tar.gz
tar xzvf cronolog-1.6.2.tar.gz 
cd cronolog-1.6.2/
./configure; make; make install
wget http://google-perftools.googlecode.com/files/google-perftools-1.8.tar.gz
tar xzvf google-perftools-1.8.tar.gz 
cd google-perftools-1.8/
./configure; make; make install
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-x86-linux.so /usr/local/lib/
wget http://download.oracle.com/berkeley-db/db-4.8.26.tar.gz
tar -xzvf db-4.8.26.tar.gz
cd db-4.8.26/build_unix/
../dist/configure --enable-cxx
make; make install
wget http://hypertable.org/pub/re2.tgz
tar -zxvf re2.tgz
cd re2
make; make install
gem install capistrano
# install Java
zypper install ant automake libtool flex bison pkg-config libevent-devel perl-Bit-Vector php5 php5-devel
wget http://dag.wieers.com/rpm/packages/perl-Class-Accessor/perl-Class-Accessor-0.31-1.el5.rf.noarch.rpm
zypper install perl-Class-Accessor-0.31-1.el5.rf.noarch.rpm
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure; make; make install
```

## Mac OS X 10.6 Snow Leopard ##

Install Thrift
```
port install p5-bit-vector p5-class-accessor
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure
make
make install # as root or in su/sudo shell
```

Download Hyperic SIGAR and install it
```
port install wget
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-universal64-macosx.dylib /opt/local/lib/libsigar-universal-macosx.dylib
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /opt/local/include
```

Install RRDtool
```
sudo port install rrdtool
```

Other than this, the description for OS X 10.5 should work for 10.6 equally well.

## Mac OS X 10.5 Leopard ##

Install [XCode](http://developer.apple.com/technology/Xcode.html)

Install [MacPorts](http://www.macports.org/install.php)

Once you installed MacPorts open Terminal and type following commands as root or in sudo shell
```
port install libevent boost google-perftools google-sparsehash log4cpp pkgconfig
```

Download Hyperic SIGAR and install it
```
port install wget
wget http://www.hypertable.org/pub/hyperic-sigar-1.6.4.zip
unzip hyperic-sigar-1.6.4.zip
cp hyperic-sigar-1.6.4/sigar-bin/lib/libsigar-universal-macosx.dylib /opt/local/lib
cp hyperic-sigar-1.6.4/sigar-bin/include/*.h /opt/local/include
```

Download and install RE2
```
wget http://hypertable.org/pub/re2.tgz
tar -zxvf re2.tgz
cd re2
make
make test
make install
make testinstall
```

Install Thrift
```
port install p5-bit-vector p5-class-accessor
wget http://archive.apache.org/dist/thrift/0.8.0/thrift-0.8.0.tar.gz
tar xzvf thrift-0.8.0.tar.gz
cd thrift-0.8.0
./configure --with-libevent=/opt/local --with-boost=/opt/local
make
make install # as root or in su/sudo shell
```

Install BerkeleyDB
```
wget http://download.oracle.com/berkeley-db/db-4.8.26.tar.gz
tar -xzvf db-4.8.26.tar.gz
cd db-4.8.26/build_unix/
../dist/configure --enable-cxx
make
make install
```

Install git
```
port -f install git-core +bash_completion +doc
```

Install cmake

```
port install cmake
```


Install RRDtool
```
sudo port install rrdtool
```

Upgrade to Java 1.6 by downloading it [here](http://developer.apple.com/java/download/) and installing it by double-clicking on the installer.  Then open the Java Preferences App and change the preferred version to Java 1.6

This should effectively make all missing dependencies present on your system so you can proceed with Hypertable compilation.

## Mac OS X 10.4 Tiger ##

Same as on Leopard, except have to disable every Java target in CMakeLists files. Tiger does not support Java 6 so unless you compile hadoop by hand with your version of java, java libraries will not compile because of version mismatch. Missing hadoop support does not cripple Hypertable in any way, so you can still use it with local filesystem or CloudStore (Kfs).

## Building, Installing, and Running Tests ##
```
mkdir src
cd src
git clone git://scm.hypertable.org/pub/repos/hypertable.git
mkdir -p ../build/hypertable
cd ../build/hypertable
cmake -DPHPTHRIFT_ROOT=$HOME/thrift/lib/php/src ~/src/hypertable
make -j 3 # speed up build. number of cpus + 1
# as root or in su/sudo shell
make install
make alltests
```

## Other useful packages to install ##
  * ganglia (http://ganglia.info/)
  * tcpdump
  * tcptrace
  * netperf ([ftp://ftp.netperf.org/netperf/](ftp://ftp.netperf.org/netperf/))
  * Capistrano (http://www.capify.org/index.php/Capistrano)
  * dstat
  * oprofile
  * iozone