
## 1. Regression Test ##
These should run on as many systems as you can reasonably try them on. (e.g. CentOS 5 x86\_64, mac OSX, FC6 i386, Ubuntu, Gentoo).  The following is a transcript of a shell session in which the regressions were run:
```
[doug@doug-judds-computer doug]$ cd ~/hypertable/0.9.0.11/
[doug@doug-judds-computer 0.9.0.11]$ ./bin/start-all-servers.sh local
Successfully started DFSBroker (local)
Successfully started Hyperspace
Successfully started Hypertable.Master
Successfully started Hypertable.RangeServer
[doug@doug-judds-computer doug]$ cd ~/build/hypertable/debug/
[doug@doug-judds-computer debug]$ make test
```
  * [Sample Output](http://www.hypertable.org/doc/tests/regressions/sample-output.txt)
## 2. Regression Test: rsclient 10 times back-to-back ##
This test involves running the rsclient regression 10 times back-to-back with the following three lines added to the configuration file:
```
Hypertable.RangeServer.AccessGroup.MaxFiles=2
Hypertable.RangeServer.AccessGroup.MergeFiles=2
Hypertable.RangeServer.AccessGroup.MaxMemory=2M
```
See [hypertable.cfg](http://www.hypertable.org/doc/tests/regressions-rsclient/hypertable.cfg).  The following is a transcript of a shell session in which the test was run.
```
  cd ~/hypertable/0.9.0.11
  cp ~/src/html/doc/tests/rsclient/hypertable.cfg conf
  ./bin/start-all-servers.sh local
  cd ~/build/hypertable/debug/src/cc/Tools/rsclient
  make test; make test; make test; make test; make test; make test; make test; make test; make test; make test
```
  * [Sample Output](http://www.hypertable.org/doc/tests/regressions-rsclient/sample-output.txt)

## 3. AOL Test: basic ##
This test can be found at `motherlode000:/home/doug/tests/aol-basic`.  The test consists of the following files:

all-no-timestamps.tsv

[Capfile](http://www.hypertable.org/doc/tests/aol-basic/Capfile)

[count.golden](http://www.hypertable.org/doc/tests/aol-basic/count.golden)

[dump-query-log.hql](http://www.hypertable.org/doc/tests/aol-basic/dump-query-log.hql)

[load.hql](http://www.hypertable.org/doc/tests/aol-basic/load.hql)

[query-log-create.hql](http://www.hypertable.org/doc/tests/aol-basic/query-log-create.hql)

[run.sh](http://www.hypertable.org/doc/tests/aol-basic/run.sh)

This test assumes that the file `~/.hypertable_version` contains the version number of the release that is being tested (e.g. 0.9.0.11).  The Capfile should also be modified to contain the proper version number as well:
```
set :hypertable_version, "0.9.0.11"
```

The following is a transcript of a shell session in which the test was run:

```
[doug@motherlode000 ~]$ cd ~/tests/aol-basic/
[doug@motherlode000 aol-basic]$ ./run.sh 
```
  * [Sample Output](http://www.hypertable.org/doc/tests/aol-basic/sample-output.txt)

## 4. AOL Test: metadata split ##
This test can be found at `motherlode000:/home/doug/tests/aol-metadata-split`.  It is essentially the same test as the AOL basic test, except with the following lines added to the Hypertable config file:
```
Hypertable.Master.Gc.Interval=15
Hypertable.RangeServer.Range.MetadataMaxBytes=8000
```
The test consists of the following files:

all-no-timestamps.tsv

[Capfile](http://www.hypertable.org/doc/tests/aol-metadata-split/Capfile)

[count.golden](http://www.hypertable.org/doc/tests/aol-metadata-split/count.golden)

[dump-query-log.hql](http://www.hypertable.org/doc/tests/aol-metadata-split/dump-query-log.hql)

[load.hql](http://www.hypertable.org/doc/tests/aol-metadata-split/load.hql)

[query-log-create.hql](http://www.hypertable.org/doc/tests/aol-metadata-split/query-log-create.hql)

[run.sh](http://www.hypertable.org/doc/tests/aol-metadata-split/run.sh)

This test assumes that the file `~/.hypertable_version` contains the version number of the release that is being tested (e.g. 0.9.0.11).  The Capfile should also be modified to contain the proper version number as well:
```
set :hypertable_version, "0.9.0.11"
```

The following is a transcript of a shell session in which the test was run:

```
[doug@motherlode000 ~]$ cd ~/tests/aol-metadata-split/
[doug@motherlode000 aol-metadata-split]$ ./run.sh 
```

  * [Sample Output](http://www.hypertable.org/doc/tests/aol-metadata-split/sample-output.txt)

## 5. Random read/write performance ##
This test can be found at `motherlode000:/home/doug/tests/random`.  It runs the random\_read\_test and random\_write\_test benchmark programs.  Since the output is not deterministic, you need to just eyeball it to make sure it passed.  Here is an example of the expected output:

  * [Sample Output](http://www.hypertable.org/doc/tests/random/sample.output)

The test consists of the following files:

[Capfile](http://www.hypertable.org/doc/tests/random/Capfile)

[random-test-create-table.hql](http://www.hypertable.org/doc/tests/random/random-test-create-table.hql)

[random-test-create-table-memory.hql](http://www.hypertable.org/doc/tests/random/random-test-create-table-memory.hql)

[run.sh](http://www.hypertable.org/doc/tests/random/run.sh)

This test assumes that the file `~/.hypertable_version` contains the version number of the release that is being tested (e.g. 0.9.0.11).  The Capfile should also be modified to contain the proper version number as well:
```
set :hypertable_version, "0.9.0.11"
```


## 6. Split Recovery ##
This test can be found at `motherlode000:/home/doug/tests/split-recovery`.  It runs the following three tests:

  1. Loads 'query-log' table with data, shuts down the servers, starts them up again, and then dumps out the 'query-log' table.
  1. Loads 'query-log' table with data, but using this hypertable.cfg file and with the RangeServer supplied with the `--crash-test=split-1:0` argument causing it to crash after the first phase of a split.  The RangeServer is immediately restarted which causes the load to continue.  After all of the data is loaded, the table is dumped.
  1. Loads 'query-log' table with data, but using this hypertable.cfg file and with the RangeServer supplied with the `--crash-test=split-2:0` argument causing it to crash after the second phase of a split.  The RangeServer is immediately restarted which causes the load to continue.  After all of the data is loaded, the table is dumped.

Sometimes this test will fail with output that looks something like the following:

```

**** TEST REPORT ****

Test 0 PASSED.
Test 1 FAILED.
1c1
< 3075313
---
> 3063834
Test 2 PASSED.
```

This is OK because the number of cells inserted ended up being more than expected.  The reason that this sometimes happens is that the loading process sends over a batch of updates that gets successfully inserted and the range server crashes before sending back the acknowledgment.  The loader will continue to retry the batch of updates and when the range server comes back up, it will receive the batch of updates for the second time, insert them (again) and then acknowledge, resulting in a double insert of the batch.

The test consists of the following files:

[Capfile](http://www.hypertable.org/doc/tests/split-recovery/Capfile)

[count.golden](http://www.hypertable.org/doc/tests/split-recovery/count.golden)

[dump-query-log.hql](http://www.hypertable.org/doc/tests/split-recovery/dump-query-log.hql)

[hypertable.cfg](http://www.hypertable.org/doc/tests/split-recovery/hypertable.cfg)

[load.hql](http://www.hypertable.org/doc/tests/split-recovery/load.hql)

[query-log-create.hql](http://www.hypertable.org/doc/tests/split-recovery/query-log-create.hql)

[rangeserver-launcher.sh](http://www.hypertable.org/doc/tests/split-recovery/rangeserver-launcher.sh)

[run.sh](http://www.hypertable.org/doc/tests/split-recovery/run.sh)

This test assumes that the file `~/.hypertable_version` contains the version number of the release that is being tested (e.g. 0.9.0.11).  The Capfile should also be modified to contain the proper version number as well:
```
set :hypertable_version, "0.9.0.11"
```