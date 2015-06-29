# Introduction #

Older versions of gcc are unable to compile Hypertable.  We've had to build a new version of gcc on some older systems in order to be able to compile Hypertable.  I've had to build a new version of gcc on the following systems:

  * Solaris Nevada snv\_67 X86  (gcc 3.4.3)
  * CentOS 3.8 (gcc 3.2.3)

Here are my notes:

```

# gmp
wget ftp://mirrors.kernel.org/gnu/gmp/gmp-4.3.1.tar.gz
tar xzvf gmp-4.3.1.tar.gz
cd gmp-4.3.1/
ABI=32 ./configure --prefix=/opt/gnu
make; make install
cd ..

# mpfr
wget ftp://mirrors.kernel.org/gnu/mpfr/mpfr-2.4.1.tar.gz
tar xzvf mpfr-2.4.1.tar.gz
cd mpfr-2.4.1
./configure --with-gmp-build=/root/gmp-4.3.1 --prefix=/opt/gnu
make; make install
cd ..

# make
wget ftp://mirrors.kernel.org/gnu/make/make-3.81.tar.gz
tar xzvf make-3.81.tar.gz
cd make-3.81
./configure --prefix=/opt/gnu
make; make install
cd ..

# sed
wget ftp://mirrors.kernel.org/gnu/sed/sed-4.2.tar.gz
tar xzvf sed-4.2.tar.gz
cd sed-4.2
./configure --prefix=/opt/gnu
make; make install
cd ..

# install findutils
wget ftp://mirrors.kernel.org/gnu/findutils/findutils-4.4.2.tar.gz
tar xzvf findutils-4.4.2.tar.gz
cd findutils-4.4.2
./configure --prefix=/opt/gnu
make; make install
cd ..

# binutils
wget ftp://mirrors.kernel.org/gnu/binutils/binutils-2.19.1.tar.gz
tar xzvf binutils-2.19.1.tar.gz
cd binutils-2.19.1
./configure --prefix=/opt/gnu
make; make install
cd ..

# Add newly built binaries to build path
export PATH=/opt/gnu/bin:$PATH
export LD_LIBRARY_PATH=/opt/gnu/lib
hash -r

# gcc
#
# NOTE: On CentOS 3.8 I had to use gcc 4.1.2 because the newer one did not build
#
wget ftp://mirrors.kernel.org/gnu/gcc/gcc-4.4.1/gcc-4.4.1.tar.gz
tar xzvf gcc-4.4.1.tar.gz
cd gcc-4.4.1/

./configure --with-mpfr=/opt/gnu --with-gmp=/opt/gnu --prefix=/opt/gnu --enable-languages="c,c++"
make; make install
cd ..

```