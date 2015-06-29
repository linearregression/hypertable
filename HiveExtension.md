# Introduction #

This page describes the Hypertable Hive integration. [Hive](http://hadoop.apache.org/hive/) is a data warehouse infrastructure built on top of Hadoop. Hypertable-Hive integration allows Hive QL statements to read and write to Hypertable via SELECT and INSERT commands.
It is recommended that users read the
[Hive Storage Handler wiki](http://wiki.apache.org/hadoop/Hive/StorageHandlers) to understand the capabilities of Hive extensions via storage handlers. Currently the Hypertable storage handler only supports external, non-native tables.

# Example #
For the purpose of this example we'll assume Hypertable, Hadoop and Hive are installed under $HT\_HOME, $HADOOP\_HOME, $HIVE\_HOME respectively. (Hypertable and Hive commands listed below are included in the Hypertable source code under the examples dir along with the sample data file. There are also other examples in the source dir examples/hive/)

The storage handler is included in the `$HT_HOME/lib/java/hypertable-*.jar` file.
Lets look at running the Hypertable storage handler via the Hive CLI. In order to do this you must do the following:
  1. Have a ThriftBroker running locally which is connected to your Hypertable cluster.

Here is an example of a Hive CLI session which uses the Hypertable storage handler:
  1. Create table and insert some values via the Hypertable shell:
```
> $HT_HOME/bin/ht shell
hypertable> use "/";

  Elapsed time:  0.30 s

hypertable> CREATE TABLE HiveTest('address');

  Elapsed time:  0.88 s

hypertable> insert into HiveTest values('1', 'address:home', '1 Home St');
hypertable> insert into HiveTest values('1', 'address:office', '1 Office St');
hypertable> insert into HiveTest values('2', 'address:nyc', '2 Foo Way');
hypertable> insert into HiveTest values('3', 'address:restaurant', '3 Bar Ct');
hypertable> insert into HiveTest values('3', 'address:home', '3 Home Ct');
hypertable> insert into HiveTest values('4', 'address', '4 My only address place');
hypertable> select * from HiveTest;
1	address:home	1 Home St
1	address:office	1 Office St
2	address:nyc	2 Foo Way
3	address:home	3 Home Ct
3	address:restaurant	3 Bar Ct
4	address	4 My only address place

  Elapsed time:  0.00 s
Avg value size:  11.50 bytes
  Avg key size:  6.50 bytes
    Throughput:  60100.17 bytes/s
   Total cells:  6
    Throughput:  3338.90 cells/s
```
  1. Create Hive table and query via Hive QL:
```
> $HIVE_HOME/bin/hive --auxpath $HT_HOME/lib/java/hypertable-0.9.5.0.pre4.jar
hive> CREATE EXTERNAL TABLE ht_table(key INT, value MAP<STRING, STRING>) STORED BY 'org.hypertable.hadoop.hive.HTStorageHandler' with SERDEPROPERTIES ("hypertable.columns.mapping" = ":key,address:") TBLPROPERTIES ("hypertable.table.name"="HiveTest", "hypertable.table.namespace"="/");
OK
Time taken: 4.335 seconds
hive> SELECT * FROM ht_table;
OK
1	{"home":"1 Home St","office":"1 Office St"}
2	{"nyc":"2 Foo Way"}
3	{"home":"3 Home Ct","restaurant":"3 Bar Ct"}
4	{"":"4 My only address place"}
Time taken: 0.54 seconds
hive> select * from ht_table where key>2;
Total MapReduce jobs = 1
Launching Job 1 out of 1
Number of reduce tasks is set to 0 since there's no reduce operator
Starting Job = job_201006161100_0003, Tracking URL = http://localhost:50030/jobdetails.jsp?jobid=job_201006161100_0003
Kill Command = /Users/sanjit/build/hadoop/current/bin/../bin/hadoop job  -Dmapred.job.tracker=localhost:9001 -kill job_201006161100_0003
2010-06-16 13:54:46,164 Stage-1 map = 0%,  reduce = 0%
2010-06-16 13:55:01,301 Stage-1 map = 100%,  reduce = 0%
2010-06-16 13:55:04,375 Stage-1 map = 100%,  reduce = 100%
Ended Job = job_201006161100_0003
OK
3	{"home":"3 Home Ct","restaurant":"3 Bar Ct"}
4	{"":"4 My only address place"}
Time taken: 22.111 seconds
```
  1. Writing to Hypertable via Hive: Download this data file [kv\_ht.txt](http://hypertable.googlecode.com/files/kv_ht.txt)
```
hive> CREATE TABLE ht_pokes(uid INT, addr_type STRING, addr_val STRING); 
OK 
Time taken: 0.076 seconds                    
hive> LOAD DATA LOCAL INPATH './kv_ht.txt' OVERWRITE INTO TABLE ht_pokes;
Copying data from file:/Users/sanjit/build/hive/kv_ht.txt
Loading data to table ht_pokes               
OK
Time taken: 0.168 seconds
hive> select * from ht_pokes;
OK
1	home	1 New Home Avenue
2	dentist	1 DDS Avenue
3	la	3 LA shack
3	restaurant	3 Hole in the wall 
7	dev house	7 Hacker way
Time taken: 0.103 seconds
hive> insert overwrite table ht_table select uid, map(addr_type, addr_val) from ht_pokes;           
Total MapReduce jobs = 1
Launching Job 1 out of 1                     
Number of reduce tasks is set to 0 since there's no reduce operator
Starting Job = job_201006161100_0004, Tracking URL = http://localhost:50030/jobdetails.jsp?jobid=job_201006161100_0004
Kill Command = /Users/sanjit/build/hadoop/current/bin/../bin/hadoop job  -Dmapred.job.tracker=localhost:9001 -kill job_201006161100_0004
2010-06-16 14:30:43,321 Stage-0 map = 0%,  reduce = 0%
2010-06-16 14:31:03,475 Stage-0 map = 100%,  reduce = 0%
2010-06-16 14:31:06,529 Stage-0 map = 100%,  reduce = 100%
Ended Job = job_201006161100_0004
5 Rows loaded to ht_table
OK
Time taken: 25.766 seconds
hive> select * from ht_table;
OK
1	{"home":"1 New Home Avenue","office":"1 Office St"}
2	{"dentist":"1 DDS Avenue","nyc":"2 Foo Way"}
3	{"home":"3 Home Ct","la":"3 LA shack","restaurant":"3 Hole in the wall "}
4	{"":"4 My only address place"}
7	{"dev house":"7 Hacker way"}
Time taken: 0.069 seconds
hive> select value from ht_table where key <5;
Total MapReduce jobs = 1
Launching Job 1 out of 1
Number of reduce tasks is set to 0 since there's no reduce operator
Starting Job = job_201006161100_0006, Tracking URL = http://localhost:50030/jobdetails.jsp?jobid=job_201006161100_0006
Kill Command = /Users/sanjit/build/hadoop/current/bin/../bin/hadoop job  -Dmapred.job.tracker=localhost:9001 -kill job_201006161100_0006
2010-06-16 14:35:29,311 Stage-1 map = 0%,  reduce = 0%
2010-06-16 14:35:45,405 Stage-1 map = 100%,  reduce = 0%
2010-06-16 14:35:48,433 Stage-1 map = 100%,  reduce = 100%
Ended Job = job_201006161100_0006
OK
{"home":"1 New Home Avenue","office":"1 Office St"}
{"dentist":"1 DDS Avenue","nyc":"2 Foo Way"}
{"home":"3 Home Ct","la":"3 LA shack","restaurant":"3 Hole in the wall "}
{"":"4 My only address place"}
Time taken: 21.671 seconds 
```
  1. Now we can inspect the contents of the table in Hypertable via the HT shell. Note that when you query the table from Hive you only see the most recent value of a cell so, you will not see the the cell "`1	address:home	1 Home St`" listed below.
```
hypertable> select * from HiveTest;
1	address:home	1 New Home Avenue
1	address:home	1 Home St
1	address:office	1 Office St
2	address:dentist	1 DDS Avenue
2	address:nyc	2 Foo Way
3	address:home	3 Home Ct
3	address:la	3 LA shack
3	address:restaurant	3 Hole in the wall 
3	address:restaurant	3 Bar Ct
4	address	4 My only address place
7	address:dev house	7 Hacker way 
```
  1. Finally to drop the table from the Hive metastore (note that the table still exists as far as Hypertable is concerned):
```
hive> drop table ht_table;
OK
Time taken: 3.485 seconds
```


## Unimplemented features ##
  * Currently column families in Hypertable map to a column of type MAP<String, ?> in Hive. A single Hypertable column family can have an arbitrary number of qualified columns, each corresponding to a single key in the Hive MAP data type. Further, the key in the Hive map type must currently be of type string, but the value can be of any type. This restriction needs to be eliminated.
  * Currently the StorageHandler can only create external Hive tables. This means Hive won't handle meta operations like create/drop so you will have to create and drop table via the usual Hypertable interfaces.
  * Add regex filtering capability.

# Limitations #
  * Whitespace should not be used in between entries in the `hypertable.columns.mapping` string, since these will be interperted as part of the column name, which is not what you want.
  * There is currently no way to access multiple versions of the same Hypertable cell. Hive can only access the latest version of a cell.

# Acknowledgements #
We'd like to thank John Sichi for his help and suggestions in developing the Hypertable Storage Handler for Hive.