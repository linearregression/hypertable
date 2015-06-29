# Quick Overview #
Hypertable is a high performance, distributed, open-source, column-oriented database.  It's designed to store and process large stores of structured and unstructured data on clusters of commodity hardware. Hypertable's architecture is modeled after Google's Bigtable.

# System Summary #
Initial releases of Hypertable provide a C++ API and an HQL (Hypertable Query Language, very similar to SQL) interpreter for client access to the data store.

Hypertable is not meant as a direct replacement for traditional Database Management Systems such as MySQL or Oracle DB, but rather an alternative for storing and processing very large datasets.  Traditional RDBMs are transaction-oriented and offer many advanced features for dealing with well structured data. Hypertable trades off features like joins and advanced querying for massive scalability and higher throughput. Where row-oriented systems (like MySQL) are in general better for workloads with a lot of write operations, column-oriented systems (like Hypertable) are better for "read-mostly" situations.

Hypertable is built on top of a Distributed File System (DFS).  There are currently a number of DFS projects, but Hypertable is designed to work with any of them.  A DFS lets multiple physical machines act as a single virtual disk and generally includes transparent replication and fault tolerance.  These systems are designed to scale to very large clusters of machines, allowing massive amounts of data storage and very fast read-write access. Hypertable layers on top of a DFS to provide a very high availability, very fast and very scalable database to hold structured or unstructured data.

In addition, large performance increases can be realized by performing computations in parallel, pushing them to where the data is physically stored.  The parallel structure of the system allows large sets of data to be worked on very quickly.