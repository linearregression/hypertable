
# Summary #

BloomFilters can be used to avoid going to disk for read queries relating to specified row/column pairs. This document describes a design for the same in Hypertable.

# Intro to Bloom Filters #

BloomFilter is a space efficient probabilistic data structure used to test whether an element is a member of a set. False positives are possible, but false negatives are not. For more, please see http://en.wikipedia.org/wiki/Bloom_filter and http://youtube.com/watch?v=947gWqwkhu0

# Use in Hypertable #

Read operations on hypertable have to read from all the `CellStore`s as well as the `CellCache`, for a particular `AccessGroup`, to compute the pertinent cells, and answer the query. The `CellCache` accesses are all in-memory, but the `CellStore` accesses can result in many disk seeks, if the blocks containing the relevant keys are not in memory. When the keys being queried for are known apriori, maintaining a bloom filter in the `CellStore` can reduce the number of disk accesses significantly. We use the `BloomFilter` to reduce disk lookups for rows and columns that have no values in the `CellStore`.

The `BloomFilter` in a `CellStore` will have bits set for every `key` and optionally `key:column_family` (if running in ROWS\_COLS mode) that have values in the `CellStore`. At query time, a `CellStore` is not queried if `CellStore::may_contain(key)` or `CellStore::may_contain(key,column)` returns false, thus saving a disk access to figure out non-existence.

# Implementation #

1. Augment the `Hypertable::CellStore` interface by adding the virtual functions:
  * `may_contain(const char *key)`
  * `may_contain(const char *key, uint8_t column_family_code, const char *column_qualifier)`

2. Add a `BloomFilter` instance to `CellStore`
  * serialization for `CellStore` puts the serialized `BloomFilter` after the fixed index and variable index are written
  * `CellStoreTrailer` contains `filter_offset`, which enables us to locate the `BloomFilter` data
  * `CellStore::add(key, value, timestamp)` updates the bloomfilter by inserting
    * the row key extracted from "key"
    * row\_key '\0' column\_family\_code '\0'
  * `may_contain(const char *rowkey)`
    * looks up key in the BloomFilter and returns true / false
  * `may_contain(rowkey, column_family)`
    * constructs the byte sequence to look up, looks up bloom filter, and returns true / false

3. `AccessGroup::CreateScanner (scan_context)`
  * check to see from `scan_context` if query is for a set of specified keys
  * if false, do what is currently being done
  * else, before creating and adding a scanner for a cell store, check the cell store to see if `may_contain(key)` and `may_contain(key,column_family)` return true for atleast one of the keys. only add if satisfied.


# Hql Example #
```
hypertable> help create table;

CREATE TABLE [table_option ...] name '(' [create_definition, ...] ')'
.
.
.
access_group_option:
    IN_MEMORY
    | BLOCKSIZE '=' value
    | COMPRESSOR '=' string_literal
    | BLOOMFILTER '=' string_literal [BLOOM_FALSE_POSITIVE_RATE '=' value]

hypertable> create table  Foo (
  Field1,
  Field2,
  Field3,
  ACCESS GROUP one bloomfilter='ROWS' bloom_false_positive_rate=0.01 (Field1),
  ACCESS GROUP two bloomfilter='ROWS_COLS' bloom_false_positive_rate=0.05 (Field2, Field3)
);
```
In this example the table Foo has two access groups. The group "one" consists of column family "Field1" and the BloomFilter for this group uses just the rowkey as the filter key and has a false positive rate of 1% (this is the default false positive rate). The group "two" which consists of column family "Field2" and "Field3" has the BloomFilter configured to contain keys based on the rowkey as well as rowkey + column family. It also has a higher false positive rate.

Currently the default false positive rate is 1%.