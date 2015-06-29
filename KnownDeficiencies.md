## Release 0.9.x alpha ##
| **Deficiency** | **Status** |
|:---------------|:-----------|
| Master does not recover RangeServers | Currently, if a RangeServer machine dies for whatever reason, it needs to be restarted or replaced.  If the machine is replaced it should have the same hostname as the machine that it is replacing.  Once the machine is up and the Hypertable server processes (e.g. dfsbroker and RangeServe) have been restarted, all of the table data will be available. |
| Master does no load balancing | This will be added in the 0.9.5.0 release |