

## oprofile - installing kernel debug symbols on CentOS 5 ##

Here's how to get debugging symbols setup on CentOS 5.  First, figure out the exact kernel version that you are using.

```
$ uname -r
2.6.18-92.1.13.el5
```

Then go to http://debuginfo.centos.org/ and find the 'kernel-debuginfo-common' and 'kernel-debug-debuginfo' packages that match your kernel release version (e.g. 2.6.18-92.1.13.el5) and install them:

```
$ wget http://debuginfo.centos.org/5/x86_64/kernel-debuginfo-common-2.6.18-92.1.13.el5.x86_64.rpm
$ rpm -i kernel-debuginfo-common-2.6.18-92.1.13.el5.x86_64.rpm

$ wget http://debuginfo.centos.org/5/x86_64/kernel-debug-debuginfo-2.6.18-92.1.13.el5.x86_64.rpm
$ rpm -i kernel-debug-debuginfo-2.6.18-92.1.13.el5.x86_64.rpm
```

Then configure oprofile to point to your debug kernel image:

```
$ opcontrol --vmlinux=/usr/lib/debug/lib/modules/2.6.18-92.1.13.el5debug/vmlinux
```