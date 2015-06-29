

## The Data (AOL Query log) ##
Here's an example of what the AOL query log data looks like:
```
AnonID  Query   ItemRank        ClickURL
1636218 www.airtime500.com      2       http://www.airtime500.com
2272416 theunorthodoxjew.blogspot.com   1       http://theunorthodoxjew.blogspot.com
172627  www.yahoolagins.com
2569723 www.homesforsale
1196769 zip codes       1       http://www.usps.com
724416  propertytaxsales.com
30011   schwab learning 1       http://www.schwablearning.org
[...]
```

The row key used was the AnonID. The Query, ItemRank, and ClickURL, columns were inserted into a table that was created with the following HQL command:
```
CREATE TABLE "query-log" (
Query,
ItemRank,
ClickURL
);
```

The query log was sorted by timestamp. Each line of the query log could generate from 1 to 3 inserts, depending on how many column values were present. After the entire log was inserted, the table contained 75,274,825 cells. On average, the size of each row key was about 7 bytes and the size of each value inserted was 15 bytes.

## Machine Profile ##

  * 1 1.8GHz Dual-core Opteron Processor 2210
  * 4 GB RAM
  * 4 7200 RPM SATA drives (mounted JBOD, 3 were used for [HDFS](http://hadoop.apache.org/core/docs/current/hdfs_design.html) data partitions)

## Insert Test ##
**_NOTE: The Hypertable Range Servers write their commit log into HDFS, however HDFS currently does not support a sync (or flush) operation.  The insert rate may drop some  once sync is implemented and called after each commit log write._**

Eight machines (motherlode001-motherlode008) were used to run both HDFS (version 0.14.4) and Hypertable. The Hypertable master and Hyperspace (Chubby) were run on motherlode001. HDFS was configured to use only three of the four available drives on each machine and was configured with 3-way replication. The log was sorted by timestamp and split into 5 pieces (a.tsv, b.tsv, c.tsv, d.tsv, e.tsv. Two separate machines (motherlode000 and motherlode009) were used to run insert clients. The following HQL commands were started more-or-less simultaneously:
```
motherlode000:
LOAD DATA INFILE ROW_KEY_COLUMN=AnonID 'a.tsv' INTO TABLE "query-log";
LOAD DATA INFILE ROW_KEY_COLUMN=AnonID 'b.tsv' INTO TABLE "query-log";

motherlode009:
LOAD DATA INFILE ROW_KEY_COLUMN=AnonID 'c.tsv' INTO TABLE "query-log";
LOAD DATA INFILE ROW_KEY_COLUMN=AnonID 'd.tsv' INTO TABLE "query-log";
```
These four insert jobs completed with the following stats:

```
  Elapsed time:  143.88 s
Avg value size:  15.25 bytes
  Avg key size:  7.10 bytes
    Throughput:  2125750.02 bytes/s
 Total inserts:  14825279
    Throughput:  103039.68 inserts/s

  Elapsed time:  144.80 s
Avg value size:  15.26 bytes
  Avg key size:  7.11 bytes
    Throughput:  2163621.98 bytes/s
 Total inserts:  15185349
    Throughput:  104871.25 inserts/s

  Elapsed time:  150.32 s
Avg value size:  15.20 bytes
  Avg key size:  7.03 bytes
    Throughput:  2080001.83 bytes/s
 Total inserts:  15208310
    Throughput:  101173.45 inserts/s

  Elapsed time:  148.21 s
Avg value size:  15.22 bytes
  Avg key size:  7.11 bytes
    Throughput:  2095660.00 bytes/s
 Total inserts:  15080926
    Throughput:  101754.55 inserts/s
```

Aggregating the numbers yields 410,838.93 random inserts/s. NOTE: The file 'e.tsv' was inserted afterwards, separately.

## Query Test ##
The file 'dump-query-log.hql' contained the following lines:

```
select * from "query-log";
quit;
```

And here is the result of the timed execution ...

```
$ time /data1/doug/hypertable/bin/hypertable --batch --timestamp-format=usecs < dump-query-log.hql > /dev/null

real    1m52.149s
user    1m30.203s
sys     0m22.025s

75,274,825 / 112.149 s == 671,094 cells/s
```

## Summary ##

The AOL query logs were inserted into an 8-node Hypertable cluster. The average size of each row key was ~7 bytes and each value was ~15 bytes. The insert rate (with 4 simultaneous insert processes) was approximately 410K inserts/s. The table was scanned at a rate of approximately 671K cells/s.

**Update:** as of 5 Jan09', the performance is about 60% better (same hardware of course) than those reported here.