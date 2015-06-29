# Introduction #

To support [transactions of range metadata operations](RangeMetaOperationTransactions.md), we need a family of meta log classes.

The basic format for the log is:
```
log ::= <header> (<log_entry>)*
header ::= ("HMML"|"HRSML") <16-bit version number> 
log_entry ::= <checksum> <log_entry_header> <payload>
checksum ::= <crc32 of <log_entry_header> <payload>>
log_entry_header ::= <timestamp> <log_entry_type> <payload_size>
timestamp ::= <64-bit nanoseconds since epoch>
log_entry_type ::= <8-bit unsigned integer with value of MetaLogEntryType below>
payload_size ::= <an unsigned 32 bit integer>
payload ::= <blob serialized depending on <log_entry_type>>
```
# API #
```
class MetaLogEntry {
public:
  virtual ~MetaLogEntry() {}

  virtual void write(DynamicBuffer &) = 0;
  virtual void read(StaticBuffer &) = 0;
  virtual int get_type() = 0; 
};
```
```
enum MetaLogEntryType {
  UNKNOWN = -1,
  RS_SPLIT_START,
  RS_SPLIT_SHRUNK,
  RS_SPLIT_DONE,
  RS_RANGE_LOADED,
  RS_MOVE_START,
  RS_MOVE_PREPARED,
  RS_MOVE_DONE,
  M_LOAD_RANGE_START,
  M_LOAD_RANGE_DONE,
  M_MOVE_RANGE_START,
  M_MOVE_RANGE_DONE,
  M_RECOVERY_START,
  M_RECOVERY_DONE
};
```
```
namespace MetaLogEntryFactory {
  
MetaLogEntry *new_rs_split_start(const TableIdentifier &, const RangeSpec &old, const RangeSpec &split_off, const RangeState &);
MetaLogEntry *new_rs_split_shrunk(const TableIdentifier &, const RangeSpec &old);
MetaLogEntry *new_rs_split_done(const TableIdentifier &, const RangeSpec &old);
MetaLogEntry *new_rs_range_loaded(const TableIdentifier &, const RangeSpec &);
MetaLogEntry *new_rs_move_start(const TableIdentifier &, const RangeSpec &, const RangeState &);
MetaLogEntry *new_rs_move_prepared(const TableIdentifier &, const RangeSpec &);
MetaLogEntry *new_rs_move_done(const TableIdentifier &, const RangeSpec &);
MetaLogEntry *new_m_load_range_start(const TableIdentifier &, const RangeSpec &, const String &rs_to, const RangeState &);
MetaLogEntry *new_m_load_range_done(const TableIdentifier &, const RangeSpec &);
MetaLogEntry *new_m_move_range_start(const TableIdentifier &, const RangeSpec &, const String &rs_from, const String &rs_to);
MetaLogEntry *new_m_move_range_done(const TableIdentifier &, const RangeSpec &);
MetaLogEntry *new_m_recovery_start(const String &rs_from);
MetaLogEntry *new_m_recovery_done(const String &rs_from);

MetaLogEntry *new_from_payload(MetaLogEntryType, uint64_t timestamp, StaticBuffer &buf);

} // namespace MetaLogEntryFactory
```
```
class MetaLog {
public:
  virtual ~MetaLog() {}
  
  virtual void write(MetaLogEntry *) = 0;
  virtual void close() = 0;

  // Remove finished entries except rs_range_loaded
  virtual void purge() = 0;
};
```
```
class MetaLogReader {
public:
  struct ScanEntry {
    MetaLogEntryType type;
    uint32_t checksum;
    uint64_t timestamp;
    size_t payload_size;
    DynamicBuffer Buf;
  };

public:
  virtual ~MetaLogReader() {}
  
  // quick scan without deserialize entries, throws if invalid
  virtual ScanEntry *next(ScanEntry *) = 0;

  // read and get ready for the next record, throws if invalid
  virtual MetaLogEntry *read() = 0;
};
```

# Implementation Notes #

There is a class for each type of entry to handle its special arguments and serialization.


