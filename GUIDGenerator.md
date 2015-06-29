## Introduction ##

This document describes the design of a Unique ID generator built into Hypertable.  The goal is to add an API to Hypertable that will return a 64-bit ID that is guaranteed to be unique, even in the face of machine failures.

## API ##

### C++ ###
We propose modifying the C++ API by adding the following methods to the Table class:

```
class Table {
  ...
  int64_t get_unique_id(uint32_t timeout_ms = 0);
  void get_unique_ids(int32_t count, std::vector<int64_t> &unique_ids, uint32_t timeout_ms = 0);
  void get_unique_id_async(ResultCallback *cb, uint32_t timeout_ms = 0);
  void get_unique_ids_async(ResultCallback *cb, int32_t count, uint32_t timeout_ms = 0);
  ...
```

### Thrift ###
We also propose adding the following two methods in the Thrift interface:

```
long next_guid(Namespace ns, string table_name);
void next_guid_async(Namespace ns, string table_name, Future future);
```

## Implementation ##

### RangeServer ###

Each table will have a special COUNTER cell that acts as a GUID generator.  The cell's row key will be `'0xff'` `'0xff'` so that it always resides in the last range of the table.  The cell's column family will be called _guid_ and will have ID 255, which will be made unavailable for normal column families.  For performance reasons, the current counter value for this special cell will be cached inside the Range object so that the GUID can be generated efficiently without having to scan the range first to accumulate the counter value.

Most of the logic for handling the GUID cell update will happen in the normal `batch_update()` code path.  The `UpdateRequest` class in the `GroupCommitInterface.h` file will be modified to include a member that indicates that the request is a "next\_guid" request.  The part of the `batch_update` method that updates the range will handle these requests differently and may call a special Range API to atomically increment and read the GUID.

The RangeServer class will have a new method added with the following signature:

```
    void next_guid(ResponseCallbackNextGuid *, const TableIdentifier *);
```

It will look similar to the `RangeServer::update` method and will either schedule a group commit update request or drop stright through to `batch_update`.  The response for the GUID updates will be handled by  the `ResponseCallbackNextGuid` class via a `response` method that sends back the GUID.

```
  class ResponseCallbackNextGuid : public ResponseCallback {
  public:
    ResponseCallbackUpdate(Comm *comm, EventPtr &event)
      : ResponseCallback(comm, event) { }

    int response(int64_t guid);
  };
```