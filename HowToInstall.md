### HOW TO BUILD ###

1. Install CMake (http://www.cmake.org/)

2. Install the following libraries (you may already have them installed. See [README](http://github.com/vicaya/hypertable/tree/master/README) for details):
  * [BerkeleyDB 4.6+](http://www.oracle.com/technology/software/products/berkeley-db/index.html)
  * [Boost version 1.34.1+](http://www.boost.org/)
  * [log4cpp 1.0+](http://log4cpp.sourceforge.net/)
  * [expat 1+](http://sourceforge.net/projects/expat) (apt-get: libexpat1-dev; yum: expat-devel)
  * readline dev lib (apt-get: libreadline5-dev; yum: readline-devel)
  * ncurses dev lib (auto installed by installing the above)

> RECOMMENDED (but not required):
  * [tcmalloc](http://code.google.com/p/google-perftools/) (if building on 64-bit Mac OSX > 10.6.2 & tcmalloc versions <1.4.1 you may encounter this issue http://code.google.com/p/google-perftools/issues/detail?id=169 )

3. Checkout the source code.
```
    mkdir -p <your_source_dir>
    cd <your_source_dir>
    git clone git://scm.hypertable.org/pub/repos/hypertable.git
```
4. Create an install directory
```
    mkdir -p <your_install_dir>
```
5. Create a build directory
```
    mkdir -p <your_build_dir>
```
6. Configure the build.
```
    cd <your_build_dir>
    cmake <your_source_dir>
```
> To edit the build options interactively:
```
    ccmake <your_source_dir>
```
7. Build the software.
```
    make (or make -j<number_of_cpu_or_cores_plus_1> for faster compile)
    make install
```

### HOW TO RUN REGRESSION TESTS ###

1. Make sure software is built and installed according to 'HOW TO BUILD'

2. Restart servers and re-create test tables
> 
> #### _WARNING: THIS STEP MUST BE PERFORMED PRIOR TO RUNNING THE TEST_ ####
> 

```
  cd <your_install_dir>/<version>
  bin/kill-servers.sh
  bin/start-all-servers.sh local
```
4. Run the regression tests
```
  cd ~/build/hypertable
  make test
```

### HOW TO BUILD SOURCE CODE DOCUMENTATION TREE (Doxygen) ###

1. Install the following libraries:
  * [Doxygen](http://www.stack.nl/~dimitri/doxygen/)
  * [Graphviz](http://www.graphviz.org/)

2. If you have doxygen installed on your system, then CMake should detect this and add a 'doc' target to the make file.  Building the source code documentation tree is just a matter of running the following commands:
```
   cd ~/build/hypertable
   make doc
```
The documentation tree will get generated under ~/build/hypertable/doc.  To view the HTML docs, load the following file into a web browser:

> ~/build/hypertable/doc/html/index.html