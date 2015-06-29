
# Introduction #

To provide some level of high availability, Hypertable needs something
akin to [Chubby](http://research.google.com/archive/chubby-osdi06.pdf).  We've decided to call this service Hyperspace.
Initially we plan to implement this service as a single server.  This
single server implementation will later be replaced with a replicated
version based on Paxos or the Spread toolkit.

The following API was distilled directly from the Chubby paper.  I've
changed a few things which are described at the end of the post.
Please take a look and respond with any feedback.  Thanks!

# API #

```
namespace Hyperspace {

  /**                                                                                                                                                          
   * The following flags (bit masks) are ORed together                                                                                                         
   * and passed in as the flags argument to Open().                                                                                                            
   */
  enum {
    OPEN_FLAG_READ           = 0x00001, // open file for reading                                                                                                 
    OPEN_FLAG_WRITE          = 0x00002, // open file for writing (modifications)                                                                                 
    OPEN_FLAG_LOCK           = 0x00004, // open file for locking                                                                                                 
    OPEN_FLAG_CREATE         = 0x00008, // create file if it does not exist                                                                                      
    OPEN_FLAG_EXCL           = 0x00010, // error if create and file exists                                                                                            
    OPEN_FLAG_TEMP           = 0x00020,  // used in conjunction with CREATE to create an ephemeral file
    OPEN_FLAG_LOCK_SHARED    = 0x00044, // atomically open and lock file shared, fail if can't
    OPEN_FLAG_LOCK_EXCLUSIVE = 0x00084  // atomically open and lock file exclusive, fail if can't
                                                                
  };                                                           

  /**                                                                                                                                                          
   * The following event masks are ORed together and                                                                                                           
   * passed in as the eventMask argument to Open()                                                                                                             
   * to indicate which events should be reported to                                                                                                            
   * the application for the opened handle.                                                                                                                    
   */
  enum {
    EVENT_MASK_ATTR_SET           = 0x0001,
    EVENT_MASK_ATTR_DEL           = 0x0002,
    EVENT_MASK_CHILD_NODE_ADDED   = 0x0004,
    EVENT_MASK_CHILD_NODE_REMOVED = 0x0008,
    EVENT_MASK_LOCK_ACQUIRED      = 0x0010,
    EVENT_MASK_LOCK_RELEASED      = 0x0020
  };

  /**
   * Listing information for each node within a directory.  A vector
   * of these objects gets passed back to the application via a call to
   * Readdir()
   */
  struct DirEntryT {
    std::string name;
    bool isDirectory;
  };

  /**                                                                                                                                                          
   * Lock sequencer.  This object gets created with                                                                                                            
   * each lock acquisition and gets passed to each                                                                                                             
   * service that expects to be protected by the                                                                                                               
   * lock.  The service will check the validity                                                                                                                
   * of this sequencer with a call to CheckSequencer                                                                                                           
   * and will reject requests if the sequencer is no                                                                                                           
   * longer valid.                                                                                                                                             
   */
  struct LockSequencerT {
    std::string name;
    uint32_t mode;
    uint32_t generation;
  };

  /**                                                                                                                                                          
   * A callback object derived from this class gets                                                                                                            
   * passed into the constructor of Hyperspace.  Session                                                                                                       
   * state changes get reported to the application via                                                                                                         
   * this callback.                                                                                                                                            
   */
  class SessionCallback {
  public:
    virtual void Jeopardy() = 0;
    virtual void Safe() = 0;
    virtual void Expired() = 0;
  };


  /**                                                                                                                                                          
   * A callback object derived from this class gets passed                                                                                                     
   * into each Open() call.  Node state changes get reported                                                                                                   
   * to the application via this callback.                                                                                                                     
   */
  class HandleCallback {
  public:
    HandleCallback(int eventMask) : mEventMask(eventMask) { return; }
    virtual void AttrSet(std::string name) = 0;
    virtual void AttrDel(std::string name) = 0;
    virtual void ChildNodeAdded(std::string name) = 0;
    virtual void ChildNodeRemoved(std::string name) = 0;
    virtual void LockAcquired(uint32_t mode) = 0;
    virtual void LockReleased() = 0;
    int GetEventMask() { return mEventMask; }
  protected:
    int mEventMask;
  };


  /**                                                                                                                                                          
   * This class encapsulates a Hyperspace session and                                                                                                          
   * provides the Hyperspace API.                                                                                                                              
   */
  class Session {

  public:

    Session(PropertiesPtr &propsPtr, SessionCallback *callback);

    int Open(std::string name, uint32_t flags, HandleCallbackPtr &callbackPtr, uint64_t *handlep);
    int Create(std::string name, uint32_t flags, HandleCallbackPtr &callbackPtr, std::vector<AttributeT> &initAttrs, uint64_t *handlep);
    int Cancel(uint64_t handle);
    int Close(uint64_t handle);
    int Poison(uint64_t handle);
    int Mkdir(std::string name);
    int AttrSet(uint64_t handle, std::string name, const void *value, size_t valueLen);
    int AttrGet(uint64_t handle, std::string name, DynamicBuffer &value);
    int AttrDel(uint64_t handle, std::string name);
    int Exists(std::string name, bool *existsp);
    int Delete(std::string name);
    int Readdir(uint64_t handle, vector<struct DirEntryT> &listing);
    int Lock(uint64_t handle, uint32_t mode, struct LockSequencerT *sequencerp);
    int TryLock(uint64_t handle, uint32_t mode, uint32_t *statusp, struct LockSequencerT *sequencerp);
    int Release(uint64_t handle);
    int GetSequencer(uint64_t handle, struct LockSequencerT *sequencerp);
    int CheckSequencer(struct LockSequencerT &sequencer);
    int Status();
  };

}


```

# Notes #

  * The Chubby paper suggests that a parent handle get passed into the above APIs that need to reference a node.  This seems cumbersome, so I've eliminated this requirement in favor of passing in an absolute path name.
  * Chubby supports the notion of whole-file reads and writes.  I've replaced this with extended attributes (e.g. AttrSet/Get/Del).  This seems more congruous with traditional filesystems and will keep open the possibility of merging Chubby functionality with an underlying DFS someday.
  * The Chubby paper describes GetSequencer and SetSequencer methods.  It's not readily apparent to me why these are necessary, so I'm omitting them for now.
  * ACL support is dropped for now since we don't have the bandwidth to support them.  ACLs can be added later when we do a "real" Chubby implementation.
  * The paper suggests that all of the APIs have asynchronous counterparts.  I don't see a pressing need right now for asynchronous Chubby interaction, so I'm omitting these.  Under the hood, all API operations will be asynchronous, so adding support later should not be difficult.
  * I've dropped the GetContentAndStat() API for now since its not readily apparent to me why it is needed.
  * Elimination of lock-delay.  This appears to be a hack to allow legacy applications to use Chubby without passing around LockSequencers.  Since there is no legacy, I've chosen to drop this concept and require all lock protected client/server interactions to use a LockSequencer.
  * Reliable event delivery.  To make the development of applications that use Hyperspace easier, I've decided to guarantee event delivery which obviates the need for "Master Failover" events.



