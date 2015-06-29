### 1. Thrift API:  namespaces ###

We've introduced namespaces, so you need to first open a namespace and pass the namespace object down into all of the APIs.  See the following list of Thrift Client examples to get an idea of how to use namespaces in various languages.

  * [Thrift Client Example - Java](http://www.hypertable.org/pub/code-examples/client_test.java.txt)
  * [Thrift Client Example - Perl](http://www.hypertable.org/pub/code-examples/client_test.pl.txt)
  * [Thrift Client Example - Python](http://www.hypertable.org/pub/code-examples/client_test.py.txt)
  * [Thrift Client Example - Ruby](http://www.hypertable.org/pub/code-examples/client_test.rb.txt)

### 2. Thrift API:  put -> offer ###

The API methods beginning with `put_` have been renamed to `offer_` to emphasize the fact that the method returns before the operation completes and the application does not know whether or not the operation was successful.

### 3. Thrift API:  CellFlag -> KeyFlag ###

The Cell object was broken into a Key object and a value (prior release).  When we made that change, we forgot to rename the CallFlag enumeration to KeyFlag.  We made that change in this release.  The new enumeration definition looks like:

```
enum KeyFlag {
  DELETE_ROW = 0,
  DELETE_CF = 1,
  DELETE_CELL = 2,
  INSERT = 255
}
```

### 4. Thrift API:  get\_schema -> get\_schema\_str ###

The ThriftClient API method `get_schema` has changed semantics.  It now returns a schema object.  The new method `get_schema_str` will return the schema in string form.  The following are the new method signatures:

```
  string get_schema_str(1:Namespace ns, 2:string table_name) throws (1:ClientException e)
  Schema get_schema(1:Namespace ns, 2:string table_name) throws (1:ClientException e)
```

### 5. HQL:  Namespaces ###

All table create, query, and modify operations now happen relative to a namespace.  There is now a `CREATE NAMESPACE` command that allows you to create a namespace.  Namespaces can be nested.  There is a default toplevel namespace called "/".  To operate in the toplevel namespace execute the following command first:

```
USE "/";
```

Or if you would prefer to move your tables into a namespace, create it first:

```
CREATE NAMESPACE "/foo";
```

And then add the following command to the top of your HQL scripts:

```
USE "/foo";
```

### 6. HQL:  CREATE TABLE option placement ###

We moved the table options in the `CREATE TABLE` statement from the beginning of the statement to the end to be consistent with the way MySQL does it:

old:
```
CREATE TABLE [table_options] <name> ( ... )
```

new:
```
CREATE TABLE <name> ( ... ) [table_options]
```