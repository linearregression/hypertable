# Introduction #

hypertable-thrift-client allows programs written in Ruby to communicate with
Hypertable using Thrift.

Hypertable is a high performance distributed data storage system designed
to support applications requiring maximum performance, scalability, and
reliability. Modeled after Google's well known Bigtable project, Hypertable
is designed to manage the storage and processing of information on a large
cluster of commodity servers, providing resilience to machine and component
failures.

http://hypertable.org/

Thrift is a software framework for scalable cross-language services
development.

http://developers.facebook.com/thrift/

# Installation #

1. Download latest hypertable-thrift-client gem:
> $ wget http://github.com/tylerkovacs/hypertable/raw/4c15fb35726841154491725c2ad20e831f5b32ba/src/rb/ThriftClient/pkg/hypertable-thrift-client-0.3.0.gem

Note:
  * check http://github.com/tylerkovacs/hypertable/tree/4c15fb35726841154491725c2ad20e831f5b32ba/src/rb/ThriftClient/pkg for a more recent version than 0.3.0
  * gem installation will be simplified in the future

2. Install gem from file
> $ gem install hypertable-thrift-client-0.3.0.gem

# Example Usage #

These examples assume that you have Hypertable installed with a Thrift
broker running on localhost.

Example 1 - Creating and populating a table using HQL commands.
```
  Hypertable.with_thrift_client("localhost", 38080) do |client|
    client.hql_query('drop table if exists thrift_test')
    client.hql_query('create table thrift_test ( col1, col2 )')
    client.hql_query("insert into thrift_test values \
      ('2008-11-11 11:11:11', 'k1', 'col1', 'v1c1'), \
      ('2008-11-11 11:11:11', 'k1', 'col2', 'v1c2'), \
      ('2008-11-11 11:11:11', 'k2', 'col1', 'v2c1'), \
      ('2008-11-11 11:11:11', 'k2', 'col2', 'v2c2'), \
      ('2008-11-11 11:11:11', 'k3', 'col1', 'v3c1'), \
      ('2008-11-11 11:11:11', 'k3', 'col2', 'v3c2')");
  end
```

Example 2 - Creating and populating a table using the native Hypertable API.
```
  Hypertable.with_thrift_client("localhost", 38080) do |client|
    client.drop_table('thrift_test', true)
    table_schema =<<EOF
      <Schema>
      <AccessGroup name="default">
        <ColumnFamily>
          <Name>col</Name>
        </ColumnFamily>
      </AccessGroup>
      </Schema>
EOF
    client.create_table('thrift_test', table_schema)

    mutator = client.open_mutator('thrift_test')
    cell1 = Hypertable::ThriftGen::Cell.new
    cell1.row_key = 'k1'
    cell1.column_family = 'col'
    cell1.value = 'v1'

    cell2 = Hypertable::ThriftGen::Cell.new
    cell2.row_key = 'k2'
    cell2.column_family = 'col'
    cell2.value = 'v2'

    cell3 = Hypertable::ThriftGen::Cell.new
    cell3.row_key = 'k3'
    cell3.column_family = 'col'
    cell3.value = 'v3'

    client.set_cells(mutator, [cell1, cell2, cell3])
    client.close_mutator(mutator, true)
  end
```
Refer to "test/thrift\_client\_test.rb" under the Thrift gem installation
for many more examples.

http://github.com/tylerkovacs/hypertable/blob/4c15fb35726841154491725c2ad20e831f5b32ba/src/rb/ThriftClient/test/thrift_client_test.rb