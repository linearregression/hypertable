## Introduction ##

This document describes how to build binary packages for Hypertable. Before building packages, the following steps must be performed.

  * Install all of the Hypertable dependencies see HowToBuild, **except** for the `htbuild` option.
  * Build the software

## Using `htbuild` ##

`htbuild` is a single self-contained script included in the source distribution (under bin/src-utils) that can build Hypertable binary package on freshly installed systems (currently only Fedora, CentOS and Ubuntu and Debian are tested) directly. One notable feature of htbuild is to build binary packages on Amazon EC2 (with ec2-api-tools installed) in one line like this:
```
<path_to>/htbuild --ami <ami-of-ubuntu> DEB RPM TBZ2
```
The above line would build six packages (the complete hypertable package and the thrift broker only package for each type) on the EC2 instance out of the mainline/master branch of Hypertable, run the regressions and copy the packages back to the local machine.

See `htbuild --help` for more options.

## Redhat-based systems ##
```
cd <build-dir>
<source-dir>/bin/src-utils/htpkg --srcdir <source-dir> RPM
```

## Debian-based systems ##
```
cd <build-dir>
<source-dir>/bin/src-utils/htpkg --srcdir <source-dir> DEB
```

## Mac OSX ##
Before packaging, you must first install [Xcode](http://developer.apple.com/technology/Xcode.html)

```
cd <build-dir>
<source-dir>/bin/src-utils/htpkg --srcdir <source-dir> PackageMaker
```

## tar bzip2 archive ##
```
cd <build-dir>
<source-dir>/bin/src-utils/htpkg --srcdir <source-dir> TBZ2
```