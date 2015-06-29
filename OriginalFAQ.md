

## What is Hypertable? ##
Hypertable is a high performance, scalable, distributed storage and processing system for structured and unstructured data. It is designed to manage the storage and processing of information on a large cluster of commodity servers, providing resilience to machine and component failures.

## Why is distributed computing good? ##
Distributed computing allows very high performance, scalable and robust solutions to be built on cheap commodity hardware.  Low cost, high performance and easy expandability are a few of the benefits of distributed computing.

## Who built Hypertable and why? ##
Hypertable was developed by Doug Judd with assistance from Luke Lu for [Zvents](http://www.zvents.com).  Zvents funded the project to develop a system to reliably store and process the massive amounts of behavioral data they collect from users searching and browsing for local events and venues.

## What is Hypertable good for? ##
Hypertable is good for storing and processing very large sets of data.  Google uses a similar system called Bigtable to store search crawler data, in particular the hyperlinks between web pages.

## How do I use Hypertable? ##
Check out out the getting started guide.

## Can I contribute to Hypertable? ##
Absolutely, Hypertable is an open-source project and encourages contributions from the community.

## Why is it called Hypertable? ##
Petatable sounded like it might be illegal.

## How is it different that Bigtable/Hbase? ##
Hypertable is based very closely on the design of Bigtable, with a few modifications.
<Stuff from doug here>.  Hypertable is designed for speed and is written in C++, while Hbase is in Java.

## How fast is it? ##
See [PerformanceTestAOLQueryLog](PerformanceTestAOLQueryLog.md) for a performance test.  This was performed on a relatively slow cluster made up of 1.8GHz opteron and 7200 rpm SATA I drives.
From the summary:
"The AOL query logs were inserted into an 8-node Hypertable cluster. The average size of each row key was ~7 bytes and each value was ~15 bytes. The insert rate (with 4 simultaneous insert processes) was approximately 410K inserts/s. The table was scanned at a rate of approximately 671K cells/s."

## What Distributed File System does Hypertable use? ##
Hypertable is designed to be filesystem agnostic, so it can be layered on the data storage system of your choice.  "Brokers" (basically file system command translators) exist for Kosmix's KFS, Hadoop's HDFS and for local systems. The local-broker allows hypertable to operate on top of a NAS device or any distributed filesystem that mounts locally (i.e. via FUSE). Because Hypertable is open source, developers are encouraged to write their own Brokers for other distributed file systems.

## Why did you go with GPL for Hypertable's license? ##
There are many, many, good reasons to adopt GPL (see [RelatedLinks](RelatedLinks.md) for details). For us, the main reason is practicality. First, most free-software packages are GPL'ed; second, we want to be able to freely use, adapt, and include, features from these packages in our distribution; third, we want to make it easy for end users to install and maintain the distribution within a single package. Note, Hypertable client libraries and supporting code for client API is released with [FLOSS license exception (the same as MySQL client libraries)](http://www.mysql.com/company/legal/licensing/foss-exception.html) to work with other open-source software seamlessly.

## Why do I get Timestamp out-of-order errors? ##
TBD

## How come when I do a scan over a time interval, some of my inserts are missing? ##
TBD

## Why an I getting Hypersace session expiration? ##
TBD