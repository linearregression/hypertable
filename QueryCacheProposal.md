### Point of Interception ###
For cache population and exploitation, the natural interception point seems to be the RangeServer::create\_scanner() method, which is where FillScanBlock returns a buffer with the data resulting from the query, given a TableIdentifier, a RangeSpec and a ScanSpec.

For cache invalidation, the best interception point seems to be the RangeServer::update() method, which takes a TableIdentifier and a buffer containing the affected keys and the new data associated to them.

### Cache Key ###
Ideally the key to the cache would be the TableIdentifier and the row keys that are contained in the buffer resulting from resolving the query. Since that can't be obtained without actually resolving the query, the next best thing would be to calculate the intersection of the RangeSpec (which refers to a range being served by the RangeServer for the specified table) and the ScanSpec (which represents the WHERE clause in the HQL query). But since that intersection is also only known after obtaining the result of the query, it also can't be used as a key to the query.

So the propose solucion is to use an MD5 hash of the TableIdentifier and the ScanSpec as the cache key. One trivial optimization would be leaving out the row limit for signature and store the row limit with data, so look ups with smaller row\_limit would be satisfied with the cache.

### Cache Invalidation ###
Whenever a key contained in a range held by the cache is updated, that range needs to be invalidated, meaning that it will be deleted from the cache.

Since the cache keys do not specify individual row keys but ranges of them, some way to obtain the ranges affected by each updated row key has to be devised.

The simplest way to identify the ranges affected by one row key is to walk through the list of cached ranges and determine, for each one, if the row key falls into it. This is a O(n) operation for each updated row key. A better way to find the affected ranges needs to be found.

Cache eviction would be done using LRU criteria.