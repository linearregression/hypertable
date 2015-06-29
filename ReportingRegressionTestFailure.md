The following steps should be taken when reporting a regression test failure.

**1.** Collect all of the log files.  They should be in `/opt/hypertable/current/log` and/or the test directory of the regression tests in the **build** tree.

**2.** Copy the `Testing/Temporary/LastTest.log.tmp` (or `Testing/Temporary/LastTest.log` file, if tests completed) which can be found relative to the toplevel build directory (where you ran `make alltests`).

**3.** If test is hung or still running, then capture a process listing
```
$ ps auxww | grep -i hyper | grep -v java > test-failure-process-listing.txt
```

**4.** If any of the test processes dumped core files, then load the core files into gdb and obtain stack traces for all threads and save the output to a file.
```
(gdb) thread apply all bt full
```

**5.** If it appears that some of the processes are hung, then obtain stack traces for **all** off them with either `pstack` or `gdb`.  For example, to capture the stack trace for a RangeServer with process ID 12345, the following two commands should work:
```
$ pstack 12345 > rangeserver-stack.txt
$ gdb --batch --quiet -ex "thread apply all bt full" -ex "quit" \
  /opt/hypertable/current/bin/Hypertable.RangeServer 12345
```

**6.** Tar up all of the files collected or generated in the previous steps and then [file an issue](http://code.google.com/p/hypertable/issues/entry).  Be sure to include the following information in the issue report:
  * Which regression test failed
  * What machine you ran the regression test on
  * Whether or not the test was run against a Debug or Release build