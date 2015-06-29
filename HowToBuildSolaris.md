First you need to install gcc and gmake.  Then untar the attached file and run the `ht-solaris-dependency-install.sh` script (listing below) as root.

```
#!/bin/bash

grep -i "OpenSolaris" /etc/release > /dev/null
if [ $? == 0 ] ; then

  # gcc
  pkg install SUNWgcc

  # gmake
  pkg install SUNWgmake

  # cmake
  pkg install pkg:/SUNWcmake

  # ncurses
  pkg install SUNncurses

  # readline
  pfexec pkg set-authority -O http://pkg.opensolaris.org/pending/ pending
  pfexec pkg install readline5

  # libevent
  pkg install SUNWlibevent

  # java
  pkg install java-dev

else

  # cmake
  wget http://www.cmake.org/files/v2.6/cmake-2.6.4.tar.gz
  tar -xzvf cmake-2.6.4.tar.gz 
  cd cmake-2.6.4
  patch -p0 < ../cmake-ncurses.patch
  ./configure
  make; make install
  cd ..

  # ant
  wget http://datadispensary.com/apache/ant/binaries/apache-ant-1.7.1-bin.tar.gz
  pushd .
  cd /usr/local
  tar xzvf /root/apache-ant-1.7.1-bin.tar.gz
  popd

  export PATH=/usr/local/apache-ant-1.7.1/bin:$PATH

fi

# boost
wget http://downloads.sourceforge.net/boost/boost_1_40_0.tar.bz2
bzip2 -d boost_1_40_0.tar.bz2 
tar xvf boost_1_40_0.tar
cd boost_1_40_0
./bootstrap.sh --with-libraries=filesystem,iostreams,program_options,system,thread,graph
./bjam install
cd ..

# git
wget http://kernel.org/pub/software/scm/git/git-1.6.4.4.tar.gz
tar xzvf git-1.6.4.4.tar.gz
cd git-1.6.4.4
./configure
gmake; gmake install
cd ..

# SIGAR
wget http://internap.dl.sourceforge.net/sourceforge/sigar/hyperic-sigar-1.6.3.tar.gz
tar -xzvf hyperic-sigar-1.6.3.tar.gz
cp hyperic-sigar-1.6.3/sigar-bin/include/*.h /usr/local/include
cp hyperic-sigar-1.6.3/sigar-bin/lib/libsigar-x86-solaris.so /usr/local/lib

# log4cpp
wget http://downloads.sourceforge.net/log4cpp/log4cpp-1.0.tar.gz
tar xzvf log4cpp-1.0.tar.gz
cd log4cpp-1.0
./configure
gmake; gmake install
cd ..

# Berkeley DB
wget http://download.oracle.com/berkeley-db/db-4.7.25.tar.gz
tar xzvf db-4.7.25.tar.gz
cd db-4.7.25/build_unix/
../dist/configure --enable-cxx
gmake; gmake install
cd ../..

# pkgconfig
wget http://ftp.gnome.org/mirror/gnome.org/sources/pkgconfig/0.18/pkgconfig-0.18.tar.gz
tar xzvf pkgconfig-0.18.tar.gz
cd pkgconfig-0.18
./configure
gmake; gmake install
cd ..

# Add /opt/gnuht/lib to dynamic linker path
crle -u -l /lib:/usr/lib:/opt/local/lib:/usr/local/BerkeleyDB.4.7/lib

# thrift
wget http://www.hypertable.org/pub/thrift-r820857-solaris.tgz
tar xzvf thrift-r820857-solaris.tgz
cd thrift
./configure --prefix=/usr/local
gmake
gmake install
cd ..

```