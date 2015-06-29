# Table Of Contents #


# Introduction #

Hypertable is a high performance distributed data storage system designed to support applications requiring maximum performance, scalability, and reliability. Modeled after Google's well known Bigtable project, Hypertable is designed to manage the storage and processing of information on a large cluster of commodity servers, providing resilience to machine and component failures.

HyperRecord exists to integrate Hypertable with ActiveRecord (the Ruby on Rails ORM) allowing objects to be stored in Hypertable while maintaining as many of the built-in ORM features as possible.

## Hypertable for Database Users ##

Hypertable has a lot in common with a typical RDBMS but also has many important differences. Through the Rails ORM, an object stored in Hypertable has many of the same features as an object stored in a regular RBDMS. In fact, for most operations, the programmer doesn't need to know or care that the data is actually stored in Hypertable. That said, one of the primary goals of HyperRecord is to provide seamless integration with ActiveRecord by abstracting these differences without completely preventing the programmer from making use of the unique features that Hypertable brings to the table.

Primary differences from a typical database

  1. Replication: Data stored in Hypertable is automatically replicated across machines in the cluster. Capacity and scalability is increased linearly simply by adding more servers to the cluster.
  1. ROW Keys: Each "row" within a table is identified by a unique ROW key. The ROW key is equivalent to a primary key in most databases. It is not necessary to declare the ROW key when creating a table since all tables automatically have a ROW key.
  1. No Secondary Indexes: In Hypertable, only the ROW key is indexed.  As a result, queries with constraints on columns other than the ROW key are not efficient. Support for indexes on non-ROW key columns **might** come in a later release. For now, applications must be designed with this constraint in mind.
  1. Sparse Cell Storage: Each cell in a table can be written individually and is not dependent on the presence of other cells within a row. For instance, in a table declared with 5 columns, a "row" can be created by writing to a single cell. The other 4 cells are not written and therefore do not consume space.
  1. Cell Versioning: Hypertable can maintain multiple revisions of a cell. Each time the cell is written, the older revision are kept around and can be retrieved at any time.  When creating a table, Hypertable allows the programmer to specify a maximum number of revisions to maintain.
  1. No Transactions (yet): Hypertable does not yet support transactions.
  1. Flexible Schema: Through the use of column qualifiers, data can be stored in cells that are not explicitly declared at table creation. Column qualifiers can be used on any column in the table definition. For instance, if a table is declared with a single column called key, then data can be written directly to the column name (key) or any number of column qualifiers (e.g., key:foo1, key:foo2, key:bar9). In Bigtable terminology, foo is known as a column family.  Since cells are stored sparsely, this provides a form of dynamic schema where data can be stored in arbitrary column qualifiers that were not declared at table creation.
  1. Single-table Operations: Hypertable does not support JOINs between tables. All operations are confined to a single table.  As a result, Hypertable does not have the notion of a foreign key.  These features can be emulated at the application layer and, in fact, HyperRecord already supports some of them.
  1. No Types (yet): Hypertable cells do not have types. All data stored in Hypertable are strings.
  1. No AUTO\_INCREMENT: Hypertable does not support AUTO\_INCREMENT (may come after support for types). Unique ROW keys must be supplied externally.
  1. HQL is not SQL: HQL is similar to SQL though they are not compatible.   [See the HQL Tutorial](http://code.google.com/p/hypertable/wiki/HQLTutorial) for more detail.


## ActiveRecord, HyperRecord, the Hypertable Adapter and Thrift ##

The integration between Hypertable and ActiveRecord is provided by three separate Ruby libraries.

  * Ruby Hypertable Thrift Client:  Exposes a low-level communication API to the Ruby programming language.  Can be used to interface with Hypertable outside of Ruby on Rails.
  * Hypertable Adapter: In ActiveRecord, each supported data store has an adapter that implements functionality specific to that store as well as providing metadata for data held within the store.  Features implemented by adapters typically include connection handling, listings metadata (tables and schema), statement execution (selects, writes, etc.), latency measurement, fixture handling.
  * HyperRecord: A subclass of ActiveRecord that overrides many internal methods to use the Hypertable API exposed through the Hypertable Adapter. Also includes logic required to expose column qualifier support to ActiveRecord and support for basic associations.

All of these libraries are actually bundled within the HyperRecord distribution, so there's no need to install or configure them separately.

# Getting Started #
## Installation and Configuration ##
  1. Install HyperRecord gem.  (http://rubygems.org/gems/hyper_record)
```
gem install hyper_record
```
  1. Add the following to the bottom of config/environment.rb:
```
require 'hyper_record'
```
  1. Configure Hypertable Adapter by adding an entry to config/database.yml that points to an instance of the Hypertable Thrift Broker (uses port 38080 by default).
```
 hypertable:
   adapter: hypertable
   host: localhost
   port: 38080
```

## Creating HyperRecord Classes ##

To create a HyperRecord class, simply inherit from ActiveRecord::HyperBase and establish a connection to your Hypertable instance (this latter step is not necessary if Hypertable is declared as the default connection for your current environment).
```
  class Page < ActiveRecord::HyperBase
    self.establish_connection(:hypertable)
  end
```
To create a table for this class in Hypertable:
```
  hql = "CREATE TABLE #{Page.table_name} (
    'name',
    'url'
  )"
  Page.connection.execute(hql)
```
For more information on creating tables in Hypertable, see the HQL Tutorial on the Hypertable web site.
**http://code.google.com/p/hypertable/wiki/HQLTutorial**

## Column Qualifiers ##

Column qualifiers allow tables to have a flexible schema.  It is not necessary to declare anything special during table creation.  Any column in a table can support qualifiers.
```
    CREATE TABLE authors (
      'name',
      'foo'
    )
```
The schema for this table looks like this:
```
  hypertable> describe table authors;
  <Schema generation="1">
    <AccessGroup name="default">
      <ColumnFamily id="1">
        <Name>name</Name>
      </ColumnFamily>
      <ColumnFamily id="2">
        <Name>foo</Name>
      </ColumnFamily>
    </AccessGroup>
  </Schema>
```
All columns support qualifiers, so you could write to cells named foo:bar1, foo:bar2 or any legal qualifier name of your choosing.  It is important to note, however, that the schema for the table does not change.  The schema command always lists only the name and book\_id columns with no indication of qualifier usage.

This behavior makes things difficult on the ORM since it relies on the schema to tell it what attributes are supported on the object.  As a result, HyperRecord objects support explicit declaration of columns that will be used with qualifiers.
```
 class Author < ActiveRecord::HyperBase
   self.establish_connection(:hypertable)
   qualified_column :foo
 end
```
As a result, qualified columns are represented as hashes in HyperRecord objects.
```
  >> a = Author.new
  >> a.foo
  => {}
  >> a.foo['bar1'] = 1
  => 1
  >> a.foo
  => {'bar1' => 1}
```
Since a qualified column can have a near infinite number of qualifiers, it would be inefficient for HyperRecord to instantiate an object with all existing qualifiers.  As a result, the hash for a qualified column will be empty unless specific qualifiers are requested at load time.
```
  >> a = Author.find(:first)
  => #<Author ROW: james_joyce, name: "James Joyce", foo: {}>
```
Specific qualified columns can be requested using :select.
```
  >> a = Author.find(:first, :select => "name, foo:bar")
  => #<Author ROW: james_joyce, name: "James Joyce", foo: {'bar' => 1}>
```
Regular expressions can be used with specifying the list of qualifiers.
```
  >> a = Author.find(:first, :select => "name, foo:b*")
  => #<Author ROW: james_joyce, name: "James Joyce", foo: {'bar' => 1}>
```
If you want find to automatically load certain qualifiers, you must declare them in the qualified\_column statement.
```
 class Author < ActiveRecord::HyperBase
   self.establish_connection(:hypertable)
   qualified_column :foo, :qualifiers => [:bar]
 end
```
A subsequent find will automatically include the :bar qualifier of the foo column.
```
  >> a = Author.find(:first)
  => #<Author ROW: james_joyce, name: "James Joyce", foo: {'bar' => 1}>
```
Regular expressions can also be used in the qualifier list.  For instance, the following code configures the Author model to load all qualifiers from the foo column family by default.
```
 class Author < ActiveRecord::HyperBase
   self.establish_connection(:hypertable)
   qualified_column :foo, :qualifiers => ['*']
 end
```

## Associations ##

Hypertable has some attributes that affect implementation of ActiveRecord associations.  Specifically, Hypertable does not support JOINs across tables and does not allow an index on arbitrary columns.

  1. **belongs\_to**:  Supported.
  1. **has\_many**: Supported but inefficient.  The association load requires retrieving rows based on a column value that is not the ROW key.  Since Hypertable has indexed lookups only on the ROW key, this operation will be relatively slow.
  1. **has\_and\_belongs\_to\_many**: Supported.  Instead of using a separate join table, the association information is written to each table using qualified columns.  For instance, assume that you have two models (Book and Author) in a has\_and\_belongs\_to\_many association.  The traditional HABTM association requires a join table (authors\_books) to list the associated objects.  In Hypertable, instead of using a separate join table, each table must have a column dedicated to listing the associations.  So, the books table has an author\_id column and the authors table has a book\_id column.  Column qualifiers are used to list as many associated objects are necessary.  So, if an Author with the ROW key charles\_dickens is added to a Book with the ROW key tale\_of\_two\_cities, then a cell called author\_id:charles\_dickens is added to the Book object and a cell called book\_id:tale\_of\_two\_cities is added to the Author object.  The value of the cells is inconsequential - their presence alone indicates an association between the objects.  When an object in an HABTM association is destroyed, the corresponding entries in the associated table are removed (warning: don't use delete because it will leave behind stale association information)

Eager loading of associations is ignored since Hypertable does not support JOINs.

## Migrations ##

HyperRecord includes support for schema changes written in raw HQL and in ActiveRecord::Migration format.  Be sure to use the connection from a HyperRecord model in your migrations.

```
  def self.up
    Model.connection.execute "ALTER TABLE table_name ADD(column_name MAX_VERSIONS=1)"
  end
```

An example in ActiveRecord::Migration format

```
  def self.up
    Model.connection.add_column :change_logs, :new_column, :string, :max_versions => 1
    Model.connection.create_table :new_table do |t| 
      t.column :name, :string 
      t.column :value, :string, :max_versions => 1 
    end 
  end

  def self.down
    Model.connection.remove_column :change_logs, :new_column
    Model.connection.drop_table(:new_table)
  end
```

Support for renaming column and changing column definitions will come in a future Hypertable release.

# ActiveRecord Compatibility Matrix #

| **Class** | **Feature** | **Supported** | **Notes**|
|:----------|:------------|:--------------|:---------|
| **General** | columns     | Yes           |Return may include QualifiedColumn objects - a subclass of Column. |
|           | delete      | Yes           | Warning: Avoid using delete on objects that belong to HABTM associations.  Hypertable does not support foreign keys and delete does not instantiate the object before removal.  As a result, the association cells are not removed from the association table leaving behind stale data.  Use destroy instead. |
|           |  decrement  | Yes           | Converts the value to an integer for the operation, though it reverts to a string in storage. |
|           | decrement!  | Yes           | Converts the value to an integer for the operation, though it reverts to a string in storage. |
|           | destroy     | Yes           | Both object.destroy and Class.destroy([...]) |
|           | exists?     | Yes           | Supports a single ROW key or a condition Hash.  e.g., Page.exists?(:name => 'ESPN') |
|           | increment   | Yes           | Converts the value to an integer for the operation, though it reverts to a string in storage. |
|           | increment!  | Yes           | Converts the value to an integer for the operation, though it reverts to a string in storage. |
|           | new\_record?  | Yes           |          |
|           | reload      | Yes           |          |
|           | save        | Yes           |          |
|           | table\_exists?  | Yes           |          |
|           | update      | Yes           |          |
|           | update\_all | No            | Not Yet Implemented.  Could be done with Hypertable API-style conditions instead of SQL strings. |
|           | update\_attribute | Yes           |          |
|           | update\_attributes | Yes           |          |
|           | validations | Yes           |          |
| **Associations** | belongs\_to association | Yes           |
|           | has\_many association | Yes           | Retrieval inefficient due to lookup by column other than ROW key. |
|           | has\_and\_belongs\_to\_many association | Yes           | No join table.  Association information stored in qualified columns within main tables. |
|           | association.new | Partial       | has\_many: new record is instantiated but the association cells are not written to the owner record. |
|           | association.create | Yes           |          |
|           | association.clear | Yes           |          |
|           | association.delete | Yes           |          |
|           | association.= | Yes           |          |
|           | association.<< | Yes           |          |
|           | association.loaded?  | Yes           |
|           | association.reset | Partial       | Reset does not work for HABTM associations since the association data is stored within qualified columns directly on the object.  Reloading the object is the equivalent operation. |
|           |             |               | **Notes**: Most association options (e.g., finder\_sql, counter\_cache) are not supported.  Associations between HyperRecord objects and regular ActiveRecord objects work in some limited cases. |
| **Finders** | find(id)    | Yes           |          |
|           | find(:first) | Yes           |          |
|           | find([id1, id2, ...]) | Yes           |          |
|           | find\_by\_sql("...") | No            | Waiting for support for issuing command via HQL in underlying API. |
|           | :select option | Yes           |          |
|           | :include option | No            | Eager loading not supported since Hypertable does not support JOINs. |
|           | :limit option | Yes           |          |
|           | :conditions option | Partial       | Supports Hash-style conditions for column values (not ROW key values).  e.g., Page.find(:all, :conditions => {:name => 'ESPN'}) |
|           | :order option | No            | Not yet supported by Hypertable. |
|           | :group option | No            | Not yet supported by Hypertable. |
|           | find\_by\_x\_and\_y | No            | Not yet implemented.  |

## Untested ActiveRecord Methods ##
  * serialize
  * counters
  * created\_at, updated\_at timestamps
  * many more