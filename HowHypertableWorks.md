Hypertable stores data in a table, sorted by a primary key.  There is no typing for data in the cells, all data is stored as uninterpreted byte strings.  Scaling is achieved by breaking tables in contiguous ranges and splitting them up to different physical machines.  There are two types of servers in a Hypertable cluster, Range Servers which hold a piece (or range) of the data and Master Servers which handle administrative tasks and oversee the Range Servers.  A single physical machine can run both a Range Server and Master Server process.  A single Range Server may hold many discontinuous ranges, the Master Server is responsible for farming them out in an intelligent way.  If a single range fills up, the range is split in half and reallocated.  The top half of the range remains and the lower half is reassined to a new Range Server by the master.  The default maximum range size is 200MB. If an entire Range Server fills up, the Master migrates ranges from the filled server to less full ones.  The list of ranges and where they live is stored in a table called METADATA that actually lives within Hypertable as normal table.  A third service, Hyperspace, also runs which contains a pointer to the root METADATA table range to point new clients in the right direction.  Hyperspace also provides a namespace similar to  a normal filesystem and acts as a lock manager for the clients.

For an example of what data in Hypertable looks like, let's say we wanted to store tags and reviews for a non-homogenous set of data.  Let's say we had a lot of entries for stuffed animals, furniture and types of hats for an online store.  The row identifier would be the name of the animal or furniture or hat and there could be columns for each tag or review.  We'll also throw price in there.  This is, of course, a very limited example, there could easily be hundreds of columns of data for each row.  With that in mind, let's look at three entries in this hypothetical database.  in the first 4 columns, the column family of "tag" indicates that this cell is a tag, the identifier of the column is the name of the tag and the value is the number of people who have tagged it that way.  In the 5th and 6th the column family indicates it is a review, the identifier indicates who wrote the review and the content is the actual review.
| key | tag:striped | tag:fourlegs | tag:velvet | tag:male | tag:comfortable | review:joey | review:susan | price:US | price:EU |
|:----|:------------|:-------------|:-----------|:---------|:----------------|:------------|:-------------|:---------|:---------|
| Zebra | 803         | 212          | 0          | 6        | 9               | "I like my zebra" |              | 25.00    | 20.00    |
| Ottoman | 1           | 343          | 298        | 0        | 0               | ""          | "this is an excellent ottoman" | 200.00   | 175.00   |
| Fedora | 3           | 0            | 145        | 34       | 78              | ""          | "a very fine hat" | 50.00    | 40.00    |


Schemas in Hypertable are very flexible, with only the column families needing to be defined before hand.  To create our example table the HQL command would be:
```

CREATE TABLE Items {
   tag,
   review,
   price
 };

```

Data is stored as key:value pairs.  All revisions of the data are stored in Hypertable, so timestamps are an important part of the keys.  A typical key for a single cell is 

&lt;row&gt;



&lt;column-family&gt;



&lt;column-qualifier&gt;



&lt;timestamp&gt;

.  Timestamps in select operations are generally passed in as a range, and any values within that range are returned.  This makes it easy to look at older versions of data and look at changes over time, as well as ensuring that all previous states of the data is saved rather than overwritten.  This default behavior can be overwritten to store a fixed number of recent versions and allow older ones to be lazily garbage collected.

Timestamped versions of the Zebra row might look something like this:

| time | key | tag:striped | tag:fourlegs | tag:velvet | tag:male | tag:comfortable | review:joey | review:susan | price:US | price:EU |
|:-----|:----|:------------|:-------------|:-----------|:---------|:----------------|:------------|:-------------|:---------|:---------|
| t=1  | Zebra | 803         | 212          | 0          | 6        | 9               | "I like my zebra" |              | 25.00    | 20.00    |
| t=2  | Zebra | 804         |              |            |          |                 |             |              |          |          |
| t=3  | Zebra |             | 216          | 1          |          |                 | "I REALLY like my zebra" |              |          |          |
| t=4  | Zebra |             |              |            |          |                 |             |              | 24.99    | 19.99    |

So at t=4, if the Zebra row would have the values
| t=4 | Zebra | 804 | 216 | 1 | 6 | 9 | "I REALLY like my zebra" |  | 24.99 | 19.99 |
|:----|:------|:----|:----|:--|:--|:--|:-------------------------|:-|:------|:------|


Random updates are handled really efficiently in Hypertable through use of Cell Caches and Cell Stores.  A Range is actually made up of many Cell Stores.  All the rows within a cell store are sorted by the row identifier.  When a write occurs to Hypertable, the information is written to a commit log in the DFS and then stored in memory in a Cell Cache.  When the Cell Cache hits its size limit it is compacted and written out to disk as a cell store.  Cell stores end up being non-contiguous, so a Heap Merge scanner is responsible for aggregating the key/value pairs in the cell cache and cell stores and returning them in sorted order.  When the range hits a cell store limit, a heap merge runs and compacts many cell stores into a single one.  In the ideal case, each Range ends up holding only a single Cell Store.

Hypertable provides the user some control over how column data is physically stored in the DFS (e.g. on disk) through the Access Group mechanism.  Each column family belongs to  a single Access Group.  The data for all of the column families in an Access Group is physically stored together on disk.  This can improve read and write performance for column families that are accessed frequently.  For example, let's say you have a table with 100 column families, but only two out of 98 of the column families are heavily accessed.  If these two heavily accessed column familes are stored together within their own access group, then the system will do disk I/O for just the data in these two columns in order to access them.  Without access groups, data for all 100 columns would have to be physically read and written, even when just two of the 100 columns are being accessed.