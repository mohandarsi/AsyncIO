
#include<deque>

/** Windows I/O controller. */
class OverlappedIOController {
public:
    OverlappedIOController();

    virtual
    ~OverlappedIOController();

    /** Enable the controller. */
    virtual void
    Enable();

    /** Disable the controller. */
    virtual void
    Disable() ;

    /** Register new opened file handle. */
    virtual void
    RegisterHandle(FileHandle &handle) ;

    /** Unregister previously registered file handle. */
    virtual void
    UnregisterHandle(FileHandle &handle) ;

private:
    /** Completion port handle for all file handles. */
    std::shared_ptr<void> m_hCompletionPort;
   
	typedef std::deque<std::thread> Threads;

	/** Dispatcher thread for I/O completion events dispatching. */
	Threads m_dispatcherThreads;
   
  //  std::thread m_dispatcherThread;

    /** Dispatcher thread function. */
    void
    DispatcherThread();
};