#pragma once

#include<deque>
#include<memory>
#include<thread>

#include "spdlog/spdlog.h"

namespace FileAPI
{

class FileHandle;

class OverlappedIOController
{

public:
    explicit OverlappedIOController(spdlog::logger& log);

    ~OverlappedIOController();

    void  enable();

    void  disable();

    void  registerHandle(FileHandle &handle) const;

    void  unregisterHandle(FileHandle &handle);

private:
    void  dispatcherThread();

private:
    spdlog::logger& m_logger;

    /** Completion port handle for all file handles. */
    std::shared_ptr<void> m_hCompletionPort;
   
    using threads = std::deque<std::thread>;

    /** Dispatcher thread for I/O completion events dispatching. */
    threads m_dispatcherThreads;

};

}