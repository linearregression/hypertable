

## Introduction ##

This document gives an architectural overview of Hypertable.  Hypertable is designed to run on top of a "third party" distributed filesystem, such as Hadoop DFS.  However, the system can also be run on top of a normal local filesystem.  We recommend that you start with the local filesystem to get up and running and to play around with the system.

The best way to get started hands-on using hypertable is to first [download the source code](http://code.google.com/p/hypertable/wiki/SourceCode?tm=4), build it, and get the regression tests to pass.  Once the regression tests are all passing, you can then start the servers runing on top of the local filesystem with the following command (assuming the installation directory is ~/hypertable/0.9.5.0.pre):

```
$ ~/hypertable/0.9.5.0.pre/bin/start-all-servers.sh local
Successfully started DFSBroker (local)
Successfully started Hyperspace
Successfully started Hypertable.Master
Successfully started Hypertable.RangeServer
Successfully started ThriftBroker
$
```

You can create tables, load data, and issue queries with the Hypertable command interpreter "hypertable":

```
$ ~/hypertable/0.9.5.0.pre/bin/ht shell 

Welcome to the hypertable command interpreter.
For information about Hypertable, visit http://www.hypertable.org/

Type 'help' for a list of commands, or 'help shell' for a
list of shell meta commands.

hypertable>
```

To get a list of all the commands available, type 'help':

```
hypertable> help

USE ................ Sets the current namespace
CREATE NAMESPACE ... Creates a new namespace
DROP NAMESPACE ..... Removes a namespace
EXISTS TABLE ....... Check if table exists
CREATE TABLE ....... Creates a table
DELETE ............. Deletes all or part of a row from a table
DESCRIBE TABLE ..... Displays a table's schema
DROP TABLE ......... Removes a table
RENAME TABLE ....... Renames a table
DUMP TABLE ......... Create efficient backup file
ALTER TABLE ........ Add/remove column family from existing table
INSERT ............. Inserts data into a table
LOAD DATA INFILE ... Loads data from a TSV input file into a table
SELECT ............. Selects (and display) cells from a table
SHOW CREATE TABLE .. Displays CREATE TABLE command used to create table
SHOW TABLES ........ Displays only the list of tables in the current namespace
GET LISTING ........ Displays the list of tables and namespace in the current namespace

Statements must be terminated with ';'.  For more information on
a specific statement, type 'help <statement>', where <statement> is from
the preceeding list.

hypertable> 
```


## Data Model ##

The Hypertable data model consists of a multi-dimensional table of information that can be queried using a single primary key.  The first dimension of the table is the row key.  The row key is the primary key and defines the order in which the table data is physically stored.  The second dimension is the column family.  This dimension is somewhat analogous to a traditional database column.  The third dimension is the column qualifier.  Within each column family, there can be a theoretically infinite number of qualified instances.  For example if we were building a URL tagging service, we might define column families content, url, and tag.  Within the "tag" column family there could be an infinite number of qualified instances, such as `tag:science`, `tag:theater`, `tag:good`, etc.  The fourth and final dimension is the time dimension.  This dimension consists of a timestamp that is usually auto assigned by the system and represents the insertion time of the cell in nanoseconds since the epoch.  Conceptually, a table in Hypertable can be thought of as a three dimensional Excel spreadsheet with timestamped versions of each cell.

The following diagram graphically depicts a crawl database table called crawldb.  The row key is the URL of a page to be crawled and the column families include:  title, content, and anchor.  The "anchor" column family illustrates the use of column qualifiers.

![http://www.hypertable.org/images/DataModel.jpg](http://www.hypertable.org/images/DataModel.jpg)

Under the hood, this multi-dimensional table of information is represented as a flat sorted list of key/value pairs.  The key is essentially the concatenation of the four dimension keys (row, column family, column qualifier, and timestamp).  The following diagram depicts the flattened key.  One thing to note is that the timestamp is stored ones compliment big-endian so that the most recent cell sorts ahead of older versions of a cell.

![http://www.hypertable.org/images/HypertableKeyDiagram.jpg](http://www.hypertable.org/images/HypertableKeyDiagram.jpg)

So the above crawldb table would have a flattened representation that looks something like the following:

![http://www.hypertable.org/images/DataModelFlattened.jpg](http://www.hypertable.org/images/DataModelFlattened.jpg)

## Physical Data Layout ##

All table data is stored in the underlying distributed filesystem.  The one that we use primarily is the Hadoop DFS, but it can be run on top of literally any filesystem.  The system abstracts the interface to the Distributed Filesytem, so writing a connector to any filesystem is trivial.

The key/value pair data is stored in files called CellStores.  At the most abstract level, the CellStore contains a sorted list of key/value pairs.  Physically, the key/value pairs are stored as a sequence of compressed blocks (approximately 65K each).  At the end of the block sequence is an index which is a list of "last key" and block offsets.  For each block in the file, there will be an index entry that contains the last key in the block along with the offset of the block.  This index gets loaded into memory when the CellStore file is loaded by the system.  The following diagram illustrates the format of the CellStore.

![http://www.hypertable.org/images/CellStore.jpg](http://www.hypertable.org/images/CellStore.jpg)

**Access Groups** - Traditional databases are considered to be either _row oriented_ or _column oriented_ depending on how data is physically stored.  With a row oriented database, all the data for a given row is stored contiguously on disk.  With a column oriented database, all data for a given column is stored contiguously on disk.  Access groups in Hypertable provide a way to group columns together physically.  All of the columns in an access group will have their data stored physically together in the same CellStore.  This is essentially a hybrid approach.  A row oriented datastore can be simulated by putting all of the columns into a single access group.  A column oriented datastore can be simulated by putting each column into its own access group.

## System Components ##

The following diagram illustrates all of the processes in the system and how they relate to one another.

![http://www.hypertable.org/images/SystemOverview.jpg](http://www.hypertable.org/images/SystemOverview.jpg)

**Hyperspace** - This is our system's equivalent of [Chubby](http://labs.google.com/papers/chubby.html).  Hyperspace (or Chubby) is a service that provides a filesystem for storing small amounts of metadata.  It also acts as a lock manager in that either exclusive or shared lock and be acquired on any of the files or directories.  Currently it is implemented as just a single server, but will be made distributed and highly available at some point in the near future.  Google refers to Chubby as, "the root of all distributed data structures" which is a good way to think of this system.  ([Hyperspace C++ API](http://www.hypertable.org/doxygen/class_hyperspace_1_1_session.html))

**Range Server** - Tables are broken into a set of contiguous row ranges, each of which is managed by a range server.  Initially each table consists of a single range that spans the entire row key space.  As the table fills with data, the range will eventually exceed a size threshold (default is 200MB) and will split into two ranges using the middle row key as a split point.  One of the ranges will stay on the same range server that held the original range and the other will get reassigned to another range server by the Master.  This splitting process continues for all of the ranges as they continue to grow. ([Range Server C++ API](http://www.hypertable.org/doxygen/class_hypertable_1_1_range_server.html))

Each range server handles all of the reading and writing of table data for the ranges that it is responsible for.  Range servers cache updates in memory (after writing them to a Commit Log) in what's called a CellCache.  Periodically the CellCache will get flushed to disk (e.g. the DFS) in a specially formatted file called a CellStore.  To scan over the data in an access group, the range server must do a heap merge of the CellCache and all of the CellStores for the access group.  The following diagram illustrates this process.

![http://www.hypertable.org/images/MergeScan.jpg](http://www.hypertable.org/images/MergeScan.jpg)

**Master** - The master handles all meta operations such as creating and deleting tables.  Client data does **not** move through the Master, so the Master can be down for short periods of time without clients being aware.  The master is also responsible for detecting range server failures and re-assigning ranges if necessary.  The master is also responsible for range server load balancing.  Currently there is only a single Master process, but the system is designed in such a way as to allow hot standby masters. ([Master C++ API](http://www.hypertable.org/doxygen/class_hypertable_1_1_master.html))

**DFS Broker** - Hypertable is designed to run on top of any filesystem.  To achieve this, the system has abstracted the interface to the filesystem through something called the DFS broker.  The DFS broker is a process that translates standardized filesystem protocol messages into the system calls that are unique to the specific filesystem.  DFS brokers have been developed for HDFS (hadoop), KFS, and local (for running on top of a local fileystem). ([DFS Broker C++ API](http://www.hypertable.org/doxygen/class_hypertable_1_1_dfs_broker_1_1_client.html))

## C++ Client API ##

Application programs can access Hypertable through a client API that is provided as part of libHypertable.  The client API consists of the following four main classes.

  * [Client](http://www.hypertable.org/doxygen/class_hypertable_1_1_client.html) - Singleton class used attach to Hypertable.  Contains methods for doing operations such as creating, dropping, and testing existence of namespaces.
  * [Namespace](http://www.hypertable.org/doxygen/class_hypertable_1_1_namespace.html) - Contains methods for doing operations such as creating, dropping, and listing tables within a namespace.
  * [Table](http://www.hypertable.org/doxygen/class_hypertable_1_1_table.html) - Objects of this class represent open tables.  Contains methods for creating scanners and mutators on the table.
  * [TableMutator](http://www.hypertable.org/doxygen/class_hypertable_1_1_table_mutator.html) - An object of this class is used to mutate (add or delete cells) a table
  * [TableScanner](http://www.hypertable.org/doxygen/class_hypertable_1_1_table_scanner.html) - An object of this class is used to scan over the cells of a table.
  * [ScanSpecBuilder](http://www.hypertable.org/doxygen/class_hypertable_1_1_scan_spec_builder.html) - Helper class for constructing ScanSpec object


The following two source files provide a good example of how you use the Hypertable Client API.  Together they illustrate how to load data into a table and query it.

  * [apache\_log\_load.cc](http://hypertable.org/doxygen/apache__log__load_8cc_source.html) - Loads one or more Apache log files into Hypertable
  * [apache\_log\_query.cc](http://www.hypertable.org/doxygen/apache__log__query_8cc_source.html) - Dumps a portion of the log table whose rows match a supplied prefix

## Thrift API ##

The [Thrift API](http://hypertable.org/thrift-api-ref/index.html) supports [many programming languages](http://wiki.apache.org/thrift/ThriftFeatures) besides C++.

Here are some examples for [Java](http://github.com/nuggetwheat/hypertable/blob/master/src/java/ThriftClient/org/hypertable/thrift/BasicClientTest.java), [PHP](http://github.com/nuggetwheat/hypertable/blob/master/src/php/ThriftClient/client_test.php), [Perl](http://github.com/nuggetwheat/hypertable/blob/master/src/perl/ThriftClient/client_test.pl), [Python](http://github.com/nuggetwheat/hypertable/blob/master/src/py/ThriftClient/client_test.py) and [Ruby](http://github.com/nuggetwheat/hypertable/blob/master/src/rb/ThriftClient/client_test.rb).

## Source Code ##

| **Directory** | **Description** |
|:--------------|:----------------|
| [src/cc/AsyncComm](http://www.hypertable.org/doxygen/dir_e65477d41cbea77146d554d167dc9746.html) | Communication subsystem |
| [src/cc/Common](http://www.hypertable.org/doxygen/dir_c15830aad3a99e858e4f63b1b1579632.html) | Common utility classes |
| [src/cc/DfsBroker/Lib](http://www.hypertable.org/doxygen/dir_df8487a01cb755e77d7c6a87e00c90c2.html) | DFS Broker framework |
| [src/cc/DfsBroker/kosmos](http://www.hypertable.org/doxygen/dir_2c4611066fc7af3cdea27803f4b2b339.html) | KFS (kosmos) broker |
| [src/cc/DfsBroker/local](http://www.hypertable.org/doxygen/dir_f65818d9fa246b5bb92a2bedb7cb867d.html) | local broker    |
| [src/cc/Hyperspace](http://www.hypertable.org/doxygen/dir_baadc7c4b53cf4594df7618997f6c732.html) | Hyperspace      |
| [src/cc/Hypertable/Lib](http://www.hypertable.org/doxygen/dir_ad5135801673f2e4c4e1be66fcd2e528.html) | Common utility classes specific to Hypertable.  Includes the Client library |
| [src/cc/Hypertable/Master](http://www.hypertable.org/doxygen/dir_e618129a8e69f36ca5b12077e594f26b.html) | Hypertable Master |
| [src/cc/Hypertable/RangeServer](http://www.hypertable.org/doxygen/dir_b6724abc6b41a2670a831834f82498af.html) | Range Server    |
| [src/cc/Tools/dfsclient](http://www.hypertable.org/doxygen/dir_73bcfc7559f59a7e4941dfb0355ed2d9.html) | CLI for DFS broker |
| [src/cc/Tools/dumplog](http://www.hypertable.org/doxygen/dir_2d01fe5358cc5c0f8802ec61c10bbe54.html) | Tool for dumping a text representation of a commit log to stdout |
| [src/cc/Tools/hyperspace](http://www.hypertable.org/doxygen/dir_12533a265f32f4452d24df4cc74e4d3f.html) | CLI for Hyperspace |
| [src/cc/Tools/hypertable](http://www.hypertable.org/doxygen/dir_31d38763fe2a26452737bd161b70db06.html) | CLI for Hypertable |
| [src/cc/Tools/merge\_diff](http://www.hypertable.org/doxygen/dir_4e43305377e88b83865889b9a999237c.html) | Test tool for diff'ing extremely large files |
| [src/cc/Tools/rsclient](http://www.hypertable.org/doxygen/dir_72fbe18c127b9eb8326f694a67d59b1d.html) | CLI for Range Server |
| [src/cc/Tools/rsdump](http://www.hypertable.org/doxygen/dir_e299868f353d5b1489fba6d43a6d2d26.html) | Tool for dumping range server statistics |
| [src/cc/Tools/serverup](http://www.hypertable.org/doxygen/dir_c60c398dc009fdd9065c03260b91f902.html) | Tool for testing to see if one of the servers is up. |
| src/java/org/hypertable/AsyncComm | Java implementation of communication subsystem |
| src/java/org/hypertable/Common] | Common utility classes for Java |
| src/java/org/hypertable/DfsBroker/hadoop | HDFS (hadoop) broker |