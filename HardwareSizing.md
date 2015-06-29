# Introduction #

This document is meant to be a guide to help people who are planning to purchase hardware for a Hypertable cluster.

# Memory #

Memory is a big factor in determining overeall system performance.  It affects both load performance and query performance.

From the loading perspective, having less memory causes the system to do more CellStore copmaction and merging.  This creates more load on the underlying DFS which in turn puts more load on the the disk subsystems and network (as 2 replicas are sent over the network).  It also creates more demand on the CPUs in the form of compression and decompression.

From the random read perspective, less memory means that less of the dataset can fit in memory, which means more of the queries will require disk i/o and decompression.

For situations with intense random read workloads, where you want to increase aggregate query throughput while minimizing latency, or for situations where you need to bulk load a tremendous amount of data into the system very quickly, use lots of memory.

Use of non-ECC memory is STRONGLY DISCOURAGED.  It can lead to subtle program errors and permanent data corruption.  See the following Hadoop mailing list post for more information:

[Re: [jira Commented: (HADOOP-573) Checksum error during sorting in reducer](http://mail-archives.apache.org/mod_mbox/hadoop-core-dev/200705.mbox/<465C3065.9050501@dragonflymc.com>)


# CPU #

If you're expecting the system to be under heavy load with a lot of concurrent requests, then the more CPU cores the better.  We recommend buying the fastest (stable) multi-core CPU that is reasonably priced.

# Disk #

At least two, ideally four.  The disk subsystem can be a limiting factor in the rate at which you can write data into the system and read data out of the system.  You should have enough aggregate disk bandwidth on each node to saturate a full-duplex gigabit link.  If you have less than this, the disk subsystem can become a bottleneck.

# Example #

| **Component** | **Lower Price Limit** | **Upper Price Limit** |
|:--------------|:----------------------|:----------------------|
| 8GB RAM       | $200                  | $300                  |
| Intel Core 2 Quad or AMD Phenom X4 | $200                  | $300                  |
| 4X 1-2TB SATA Drives | $400                  | $1200                 |
| Total         | $800                  | $2400                 |

If you leave some memory slots open, that will give you a relatively inexpensive way to improve cluster performance.  We've seen up to 50% performance improvement for some workloads simply by doubling the RAM.

Also, try to buy the highest quality commodity components.