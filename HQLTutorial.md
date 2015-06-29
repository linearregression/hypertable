_This walk through was written using a pre-release version of hypertable -- you may see small variations in output from the current release._


## SETUP ##

For this example you'll need to download the data from http://hypertable.googlecode.com/files/access.tsv.gz and run:

```
tinweasel:~> gunzip access.tsv.gz
tinweasel:~> mv access.tsv ~/hypertable/examples/hql_tutorial/
```

In this example we'll startup a local hypertable instance, create a table, load some access log data from a file, and do some basic operations using interactive Hypertext Query Language (HQL) commands. Afterwards, we'll demonstrate use of the --batch mode for HQL.

Hypertable is designed to be "always on" but for the purposes of this example we'll assume you're running locally from a fresh install. Also, following the `README.txt` in the distribution we assume your hypertable directory is `~/hypertable`

```
codeslinger:~> cd ~/hypertable
codeslinger:~/hypertable> bin/start-all-servers.sh local

Successfully started DFSBroker (local)
Successfully started Hyperspace
Successfully started Hypertable.Master
Successfully started Hypertable.RangeServer
```

Now, fire up an interactive session:

```
codeslinger:~/hypertable> bin/ht hypertable
Welcome to the HQL command interpreter.

Type 'help;' or '\h' for help. Type '\c' to clear the buffer.

hypertable>
```

Start with a simple 'help':

```

hypertable> help;

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
SHUTDOWN ........... Shuts servers down gracefully

Statements must be terminated with ';'.  For more information on
a specific statement, type 'help <statement>', where <statement> is from
the preceeding list.

```

## INTERACTIVE HYPERTABLE & HQL ##

For this example we're working with a simplified access log that contains four columns: a `date` field, the referrer's url 'refer-url', an http code 'http-code', and a field we'll call 'rowkey' -- actually a url from one of the hosts we serve. There are 100,000 records in the file.

The file is under `examples/hql_tutorial/access.tsv` and you'll need to unzip it first if it's still compressed. _NOTE: `LOAD DATA INFILE` will a compressed file option soon._

## USE ##
First, we need to open a namespace within which we will operate on tables. A Hypertable namespace is roughly analogous to a "database" in a system like MySQL. A major is that a namespace can contain other namespaces in addition to tables. To start using the root namespace:
```
hypertable> use "/";
```

## CREATE NAMESPACE ##

To create a new namespace and use it:
```
hypertable> create namespace "Tutorial";
hypertable> use Tutorial;
```

## CREATE TABLE ##
Now that we have created and opened the "Tutorial" namespace we can create a table within it. To create a table to hold the data from our simplified access log -- we'll call it `Pages`:

```
hypertable> create table Pages ( date, "refer-url", "http-code");
```

Column names that contain a dash or any non-alphanumeric character (e.g. "refer-url") must be enclosed in quotes. Now, issue the `SHOW CREATE TABLE` command to make sure you got everything right. We didn't have to include the field called 'rowkey' because we'll use that in our LOAD DATA INFILE command later:

```
hypertable> show create table Pages;

CREATE TABLE Pages (
  date,
  'refer-url',
  'http-code',
  ACCESS GROUP default (date, 'refer-url', 'http-code')
)

```

And, notice that, by default, a single `ACCESS GROUP` is created -- see the full documentation for details on that but basically the `ACCESS GROUP` is an optimization.

Further, we get a minimal schema description via:

```
hypertable> describe table Pages;
<Schema generation="1">
  <AccessGroup name="default">
    <ColumnFamily id="1">
      <Generation>1</Generation>
      <Name>date</Name>
      <deleted>false</deleted>
    </ColumnFamily>
    <ColumnFamily id="2">
      <Generation>1</Generation>
      <Name>refer-url</Name>
      <deleted>false</deleted>
    </ColumnFamily>
    <ColumnFamily id="3">
      <Generation>1</Generation>
      <Name>http-code</Name>
      <deleted>false</deleted>
    </ColumnFamily>
  </AccessGroup>
</Schema>
```

## LOAD DATA INFILE ##

Now, let's load some data using the MySQL-like TAB delimited format (TSV). For that, we assume you have the example data in `examples/hql_tutorial/access.tsv`. This format includes a single header line that gives `column_family_names` (no `column_qualifiers` supported for this format yet). For the example, our data file has header:

```
# rowkey        date    refer-url       http-code
events.mercurynews.com/venues   2008-01-25 15:19:32     events.mercurynews.com/search   200
www.zvents.com/events/auto_complete_for_artist_name     2008-01-25 15:19:32     www.zvents.com/indio-ca/events/show/81296496-coachella  200
calendar.denverpost.com/search  2008-01-25 15:19:32     calendar.denverpost.com/search  200
www.zvents.com/search   2008-01-25 15:19:32     www.zvents.com/search   200

```

_Tip: In case the file is gzipped and its name ends with '.gz', you can directly load it without having to unzip it first._

### LOAD DATA INFILE (INTO TABLE) ###

So, now we load it into our Pages table -- notice that we are specifying a column present in the data file ('rowkey') that's not in our schema. We can specify any field name present in the data for the ROW\_KEY\_COLUMN option.

```
hypertable> load data infile ROW_KEY_COLUMN=rowkey "examples/hql_tutorial/access.tsv" into table Pages;

Loading  10,872,957 bytes of input data...

0%   10   20   30   40   50   60   70   80   90   100%
|----|----|----|----|----|----|----|----|----|----|
***************************************************
Load complete.

  Elapsed time:  6.00 s
Avg value size:  18.68 bytes
  Avg key size:  48.70 bytes
    Throughput:  1811875.04 bytes/s
 Total inserts:  300000
    Throughput:  49992.15 inserts/s
       Resends:  0

```

### LOAD DATA INFILE (INTO FILE) ###

Also, make sure you don't skip over an important utility for converting these more standard formatted files into insert files used by hypertable client programs:

```
hypertable> load data infile ROW_KEY_COLUMN=rowkey "examples/hql_tutorial/access.tsv" into file "test.tsv";

Loading  10,872,957 bytes of input data...

0%   10   20   30   40   50   60   70   80   90   100%
|----|----|----|----|----|----|----|----|----|----|
***************************************************
Load complete.

  Elapsed time:  2.41 s
Avg value size:  18.68 bytes
  Avg key size:  48.70 bytes
    Throughput:  4513692.45 bytes/s
 Total inserts:  300000
    Throughput:  124539.05 inserts/s
```

And, now we have a new file, `test.tsv`, with the fully specified hypertable insert format which illustrates the fundamental key/value sparse representation of our data (which would be more clear if we were using `column_qualifiers`)

```
codeslinger:~/hypertable>  head -10 test.tsv 
rowkey  columnkey       value
events.mercurynews.com/venues   date    2008-01-25 15:19:32
events.mercurynews.com/venues   refer-url       events.mercurynews.com/search
events.mercurynews.com/venues   http-code       200
www.zvents.com/events/auto_complete_for_artist_name     date    2008-01-25 15:19:32
www.zvents.com/events/auto_complete_for_artist_name     refer-url       www.zvents.com/indio-ca/events/show/81296496-coachella
www.zvents.com/events/auto_complete_for_artist_name     http-code       200
calendar.denverpost.com/search  date    2008-01-25 15:19:32
calendar.denverpost.com/search  refer-url       calendar.denverpost.com/search
calendar.denverpost.com/search  http-code       200
```


### LOAD DATA INFILE (FROM STDIN) ###
You can redirect the output from a process directly into Hypertable via this feature. For example, this feature could be used to stream the output of a mapreduce process into hypertable without having to save it to an intermediate file for loading. To use this feature the file name in the load data infile should be set to "-". In the example below we will redirect a file to the hypertable shell STDIN to load a table.
Imagine you have a file called 'LdiStdin.hql' with contents:

```
codeslinger:~/hypertable>  cat LdiStdin.hql
DROP TABLE IF EXISTS LdiStdin;
CREATE TABLE LdiStdin(TestColumnFamily);
LOAD DATA INFILE ROW_KEY_COLUMN=rowkey "-" INTO TABLE LdiStdin;
# rowkey        TestColumnFamily
key1    tcf1
key2    tcf2
 
```

You can run:
```
/bin/ht hypertable < LdiStdin.hql
```

On inspecting the contents of the table you should see:
```
hypertable> SELECT * FROM LdiStdin;
key1    TestColumnFamily        tcf1
key2    TestColumnFamily        tcf2
```

## SELECT ##

In the examples that follow I'm using the options spec DISPLAY\_TIMESTAMPS at the end of each select but that's entirely optional and you can omit them from most of the examples (except where you want to see them in the output) -- here's a minimal SELECT:

```
hypertable> select 'http-code' from Pages where ROW='events.getoutaz.com/scottsdale-az/venues/show/455885-scorch-bar';
events.getoutaz.com/scottsdale-az/venues/show/455885-scorch-bar	http-code	200
events.getoutaz.com/scottsdale-az/venues/show/455885-scorch-bar	http-code	200
events.getoutaz.com/scottsdale-az/venues/show/455885-scorch-bar	http-code	200
events.getoutaz.com/scottsdale-az/venues/show/455885-scorch-bar	http-code	200
```

Now, let's use the `SELECT` command to examine our data a bit -- let's start by looking at all the accesses for a particular page (a Steve Martin event in san francisco) and check the referrer's url:

```
hypertable> select "refer-url" from Pages where ROW = "www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin" DISPLAY_TIMESTAMPS;
2008-02-02 15:53:02.573496124   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       refer-url       www.google.com/search
2008-02-02 15:53:02.573496122   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       refer-url       www.google.com/search
2008-02-02 15:53:01.673853169   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       refer-url       www.zvents.com/search
2008-02-02 15:53:00.073333232   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       refer-url       www.google.com/search
2008-02-02 15:53:00.073333229   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       refer-url       www.google.com/search
2008-02-02 15:52:59.673141187   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       refer-url       www.google.com/search
```

Those referrals all came in from google search. Okay, let's check the http code we returned to those visitors:

```
hypertable> select "http-code" from Pages where ROW = "www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin" DISPLAY_TIMESTAMPS;
2008-02-02 15:53:02.573496123   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:02.573496120   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:01.673853170   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:00.073333231   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:00.073333230   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:52:59.673141186   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200

```

Looks good -- we made sure that Steve Martin fans got the content they needed. Okay, let's restrict the select using the system time stamp by adding to the `WHERE` clause:

```
hypertable> select "http-code" from Pages where ROW = "www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin" AND TIMESTAMP >= '2008-02-02 00:00:00' DISPLAY_TIMESTAMPS;
2008-02-02 15:53:02.573496123   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:02.573496120   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:01.673853170   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:00.073333231   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:00.073333230   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:52:59.673141186   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200

```

Or, restrict to just a particular 2 second period:

```
hypertable> select "http-code" from Pages where ROW = "www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin" AND '2008-02-02 15:53:00' < TIMESTAMP <= '2008-02-02 15:53:02' DISPLAY_TIMESTAMPS;
2008-02-02 15:53:01.673853170   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:00.073333231   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200
2008-02-02 15:53:00.073333230   www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin       http-code       200

```

Of course, we can choose a range of row keys as well, say to look at the referrer's url:

```
select "refer-url" from Pages where "www.zvents.com/events/buy_tickets/80283482" <= ROW <= "www.zvents.com/events/buy_tickets/80310000" DISPLAY_TIMESTAMPS;
2008-02-02 15:53:02.573493952   www.zvents.com/events/buy_tickets/80283482      refer-url       www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin
2008-02-02 15:53:02.573493949   www.zvents.com/events/buy_tickets/80283482      refer-url       www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin
2008-02-02 15:53:00.073330904   www.zvents.com/events/buy_tickets/80283482      refer-url       www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin
2008-02-02 15:53:00.073330907   www.zvents.com/events/buy_tickets/80308810      refer-url       www.zvents.com/las-vegas-nv/events/show/80308810-the-platters-coasters-and-the-legendary-lead-singers-of-the-temptations
```

And, as before with a single row key, we can restrict the select to a specific range of system time:

```
hypertable> select "refer-url" from Pages where "www.zvents.com/events/buy_tickets/80283482" <= ROW <= "www.zvents.com/events/buy_tickets/80310000" AND
 '2008-02-02 15:53:00' <= TIMESTAMP <= '2008-02-02 15:53:01' DISPLAY_TIMESTAMPS;
2008-02-02 15:53:00.073330904   www.zvents.com/events/buy_tickets/80283482      refer-url       www.zvents.com/san-francisco-ca/events/show/80283482-steve-martin
2008-02-02 15:53:00.073330907   www.zvents.com/events/buy_tickets/80308810      refer-url       www.zvents.com/las-vegas-nv/events/show/80308810-the-platters-coasters-and-the-legendary-lead-singers-of-the-temptations

```

You can type "help select" in the hypertable command shell to see the full syntax of "select".

## INSERT & DELETE ##

Though it's unlikely to be needed in interactive mode let's do a quick demonstration of `INSERT`. The key to remember is that hypertable data is versioned by time stamp and that you can either set the time stamp on insert or accept the default based on your current system time. Delete operations also have a time stamp and the delete operates on all data occurring at the time stamp or **_before_**.

For example, let's `INSERT` some data in the past and issue a select command to verify the data:

```
hypertable> insert into Pages values ('2006-01-01 23:59:59', "www.hypertable.org", "refer-url", "www.zvents.com");
hypertable> select * from Pages where ROW = "www.hypertable.org" DISPLAY_TIMESTAMPS;
2006-01-01 23:59:59.000000000   www.hypertable.org      refer-url       www.zvents.com
```

Now, let's do a `DELETE` with the default time stamp and verify that's it deleted using a `SELECT`:

```
hypertable> delete * from Pages where ROW = "www.hypertable.org";
delete: row='www.hypertable.org' family=0 ts=0
hypertable> select * from Pages where ROW = "www.hypertable.org";
```

Now, we'll `INSERT` some data far into the future (as of this tutorial) and verify that it survives the `DELETE`:

```
hypertable> insert into Pages values ('2037-01-01 23:59:59', "www.hypertable.org", "refer-url", "www.zvents.com");
hypertable> delete * from Pages where ROW = "www.hypertable.org";
delete: row='www.hypertable.org' family=0 ts=0
hypertable> select * from Pages where ROW = "www.hypertable.org" DISPLAY_TIMESTAMPS;
2037-01-01 23:59:59.000000000   www.hypertable.org      refer-url       www.zvents.com
```

Now, let's get rid of it with `DELETE` time stamped further into the future:

```
hypertable> delete * from Pages where ROW = "www.hypertable.org" TIMESTAMP '2038-01-01 23:59:59';
delete: row='www.hypertable.org' family=0 ts=2146031999000000001
hypertable> select * from Pages where ROW = "www.hypertable.org";
```

## ALTER TABLE (ADD/DROP COLUMN FAMILIES) ##

Now, we want to get rid of the column family 'http-code' and add a couple of new column families, namely 'load-time' and 'error-message'. Moreover we want to create a new ACCESS GROUP for fast access to the data in 'load-time'. You can do this with the "ALTER TABLE"  command.
```
hypertable> SHOW CREATE TABLE Pages;

CREATE TABLE Pages (
  date,
  'refer-url',
  'http-code',
  ACCESS GROUP default (date, 'refer-url', 'http-code')
)

hypertable> ALTER TABLE Pages ADD('load-time', 'error-message', ACCESS GROUP stats('load-time')) DROP ('http-code');
hypertable> show create table Pages;

CREATE TABLE Pages (
  date,
  'refer-url',
  'error-message',
  'load-time',
  ACCESS GROUP default (date, 'refer-url', 'http-code', 'error-message'),
  ACCESS GROUP stats ('load-time'),
)

```

You can also specify other standard access group options including bloom filter settings etc when adding columns.

At present you can have a maximum of 255 column families created during the lifespan of a table. This means dropped columns count towards this limit, so users are advised to use this feature thriftily.
Note how the dropped columns are still counted in the schema below.
```
hypertable> describe table Pages;
<Schema generation="2">
  <AccessGroup name="default">
    <ColumnFamily id="1">
      <Generation>1</Generation>
      <Name>date</Name>
      <deleted>false</deleted>
    </ColumnFamily>
    <ColumnFamily id="2">
      <Generation>1</Generation>
      <Name>refer-url</Name>
      <deleted>false</deleted>
    </ColumnFamily>
    <ColumnFamily id="3">
      <Generation>1</Generation>
      <Name>http-code</Name>
      <deleted>true</deleted>
    </ColumnFamily>
    <ColumnFamily id="5">
      <Generation>2</Generation>
      <Name>error-message</Name>
      <deleted>false</deleted>
    </ColumnFamily>
  </AccessGroup>
  <AccessGroup name="stats">
    <ColumnFamily id="4">
      <Generation>2</Generation>
      <Name>load-time</Name>
      <deleted>false</deleted>
    </ColumnFamily>
  </AccessGroup>
</Schema>
```

Also currently you can't add a previously dropped column. If you try you will see this:
```
hypertable> ALTER TABLE Pages ADD('http-code');
Error: Column family 'http-code' multiply defined - HYPERTABLE bad schema
```

## SHOW TABLES & DROP TABLE ##

Now, we want to get rid of our table so we'll use DROP TABLE to do that -- first verify the tables we have, then drop, then verify that they are gone after the command:

```
hypertable> show tables;
PAGES
hypertable> drop table Pages;
hypertable> show tables;
```

The name of the table is case insensitive.

## GET LISTING & DROP NAMESPACE ##
Now, we want to get rid of the "Tutorial" namespace and verify that we have:

```
hypertable> use "/";
hypertable> get listing;
Tutorial	(namespace)
sys	(namespace)
hypertable> drop namespace Tutorial;
hypertable> get listing;
sys	(namespace)
```

The "sys" namespace is used by the Hypertable system and should not be used to contain user tables.

Note that a namespace must be empty (ie must not contain any sub-namespaces or tables) before you can drop it. In this case since we had already dropped the "Pages" table, we could go ahead and drop the "Tutorial" namespace.

## BATCH MODE AND SOURCE ##

To load your tables and other data back to the system, run the system in batch mode or use `SOURCE` from an interactive console. First, run `kill-servers.sh` and `start-all-servers.sh`:

```
tinweasel:~/hypertable>  bin/kill-servers.sh 
tinweasel:~/hypertable>  bin/start-all-servers.sh local
Successfully started DFSBroker (local)
Successfully started Hyperspace
Successfully started Hypertable.Master
Successfully started Hypertable.RangeServer

```

You can fire up an interactive session and use the `source` command:

```
tinweasel:~/hypertable>  bin/ht hypertable 
Welcome to the HQL command interpreter.

Type 'help;' or '\h' for help. Type '\c' to clear the buffer.

hypertable> source ./examples/hql_tutorial/Pages-create.hql;

Loading  10,872,957 bytes of input data...

0%   10   20   30   40   50   60   70   80   90   100%
|----|----|----|----|----|----|----|----|----|----|
***************************************************
Load complete.

  Elapsed time:  2.68 s
Avg value size:  18.68 bytes
  Avg key size:  48.70 bytes
    Throughput:  4060564.38 bytes/s
 Total inserts:  300000
    Throughput:  112036.62 inserts/s
       Resends:  0

```

Alternatively, to avoid opening up an interactive session you can use the --batch command -- first, run `kill-servers.sh` and `start-all-servers.sh`:

```
tinweasel:~/hypertable>  bin/start-all-servers.sh local
Successfully started DFSBroker (local)
Successfully started Hyperspace
Successfully started Hypertable.Master
Successfully started Hypertable.RangeServer

tinweasel:~/hypertable>  bin/ht hypertable --batch < examples/hql_tutorial/Pages-create.hql 
CREATE TABLE Pages (
date,
"refer-url",
"http-code",
ACCESS GROUP default ( date, "refer-url", "http-code" )
);
load data infile ROW_KEY_COLUMN=rowkey "examples/hql_tutorial/access.tsv" into table Pages;

Loading  10,872,957 bytes of input data...

0%   10   20   30   40   50   60   70   80   90   100%
|----|----|----|----|----|----|----|----|----|----|
***************************************************
Load complete.

  Elapsed time:  2.71 s
Avg value size:  18.68 bytes
  Avg key size:  48.70 bytes
    Throughput:  4019384.28 bytes/s
 Total inserts:  300000
    Throughput:  110900.40 inserts/s
       Resends:  0


```

You can examine the file in `examples/hql_tutorial/Pages-create.hql` to verify that it's just a batch of HQL commands.

```

tinweasel:~/hypertable>  cat examples/hql_tutorial/Pages-create.hql
USE "/"; 
CREATE TABLE Pages (
       date,
       "refer-url",
       "http-code",
ACCESS GROUP default ( date, "refer-url", "http-code" )
);
load data infile ROW_KEY_COLUMN=rowkey "examples/hql_tutorial/access.tsv" into table Pages;

```