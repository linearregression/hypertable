The example shown here is to upgrade from 0.9.5.4 to 0.9.5.5. Adapt accordingly.

  1. Prepare for next release
    * Update the `CHANGES` file with the list of changes for the release
    * Modify the `CMakeLists.txt` file to include the new version number
    * Update version number in `conf/Capfile.*`
    * Modify `bin/upgrade-ok.sh` if necessary
    * Run `make doc` and try to eliminate doxygen warnings
    * Update README to point to new thrift, if needed
  1. Test all changes in the 'next' branch
    * First get all changes into 'next'
    * Make sure the version number is correct (e.g. 0.9.5.5 here) in the top-level CMakeLists.txt
    * Make sure 'next' is a superset of 'master', do a `git merge master`
    * Make sure tests are passing in the 'next' branch
  1. Pull the changes from 'next' into 'master'
```
  git checkout master
  git merge next
```
  1. Tag the release:
```
git tag -a v0.9.5.5
```
> > Note, -a or -s is required for "git describe" used by the version mechanism in the build system)
  1. Push the changes to official repository:
```
git push # optionally with an origin name
git push --tags # optionally with an origin name
```
  1. Build the binary packages
    * Build them using the [Hypertable Build AMIs](http://code.google.com/p/hypertable/wiki/AMIs).
    * Build a release version of the binaries
```
cd ~/build/hypertable/release
cmake -DPHPTHRIFT_ROOT=/usr/src/thrift/lib/php/src -DCMAKE_BUILD_TYPE=Release ~/src/hypertable
make install
```
    * Build the `.rpm` and `.tar.bz` on the CentOS 5.2 machines:
```
cd ~/build/hypertable/release
~/src/hypertable/bin/src-utils/htpkg --srcdir ~/src/hypertable --thriftdir /usr/src/thrift RPM TBZ2
```
    * Build the `.deb` on the Ubuntu machines:
```
cd ~/build/hypertable/release
~/src/hypertable/bin/src-utils/htpkg --srcdir ~/src/hypertable --thriftdir /usr/src/thrift DEB
```
    * Build the `.dmg` on a new mac:
```
cd ~/build/hypertable/release
~/src/hypertable/bin/src-utils/htpkg --srcdir ~/src/hypertable --thriftdir /usr/src/thrift PackageMaker
```
    * Create a release directory `scm.hypertable.org:/pub/download/0.9.5.5` and copy the binary packages into this directory
    * In the HTML repository, in the `com/download` directory, create a `0.9.5.5.html` file, and the previous release to the `archive.html` file and update the `index.html` link.
  1. Create release tarball:
```
git archive --format=tar --prefix=hypertable-0.9.5.5/ v0.9.5.5 | gzip -9 > ~/tmp/hypertable-0.9.5.5-src.tar.gz
```
> > Note: gzip compress better than bzip2 for hypertable source, go figure; the trailing / in the prefix is important.
  1. Upload the tarball to the google code download area:  http://code.google.com/p/hypertable/downloads/list
  1. Update github repository
  1. Build binary packages using bin/src-utils/htbuild
  1. Update website (in the html.git repository): change wordings in `src/*.php` and `src/*.inc` if necessary. But more importantly edit the src/release.inc to make sure variables: $download\_version, $release\_version and $release\_date are set correctly. And then in the web root directory, run the following command:
```
src/generate.sh
```
> > which will generate the html pages and the news feed.
  1. Re-build doxygen source documentation, check it in (html.git) and publish it (push to the official html.git and pull in /var/www/html)
  1. Send a note to the mailing lists (hypertable-...)