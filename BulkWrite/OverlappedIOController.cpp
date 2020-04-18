#pragma once

#include "FileStream.h"
#include "IORequest.h"
#include "FileHandle.h"
#include "Utils.h"
#include "OverlappedIOController.h"

#define CK_STOP 0

OverlappedIOController::OverlappedIOController()
{
   const auto hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
   if (!hCompletionPort) {
        throw std::exception("Failed to create I/O completion port");
    }
    m_hCompletionPort = std::shared_ptr<void>(hCompletionPort,CloseHandle);
}

OverlappedIOController::~OverlappedIOController()
{
    Disable();
}

void
OverlappedIOController::Enable()
{
    if (m_dispatcherThreads.size() <= 0) {

        int concurrentThreads = std::thread::hardware_concurrency();
        m_dispatcherThreads.resize(concurrentThreads);
        for (size_t i = 0; i < m_dispatcherThreads.size(); ++i)
        {
            m_dispatcherThreads[i] = std::thread(&OverlappedIOController::DispatcherThread,
                this);
        }
    }
}

void
OverlappedIOController::Disable()
{
    for (Threads::const_iterator it = m_dispatcherThreads.begin(), end = m_dispatcherThreads.end(); it != end; ++it)
    {
        /* Signal dispatcher to exit. */
        if (0 == ::PostQueuedCompletionStatus(m_hCompletionPort.get(), 0, CK_STOP, 0))
        {
            const std::string error = GetSystemError();
            std::cout << "PostQueuedCompletionStatus Error: " << error.c_str() << std::endl;
        }
    }

    for (auto it = m_dispatcherThreads.begin(), end = m_dispatcherThreads.end(); it != end; ++it)
    {
        /* wait for the thread to exit. */
        (*it).join();
    }

    m_dispatcherThreads.clear();
}

void
OverlappedIOController::RegisterHandle(
    FileHandle &fileHandle)
{
    if (!CreateIoCompletionPort(fileHandle.m_hFileHandle.get(), m_hCompletionPort.get(),
                                reinterpret_cast<ULONG_PTR>(&fileHandle), 0)) 
    {

          throw std::exception("Failed to attach handle to I/O completion port");
    }

    if (fileHandle.m_hFileWriteHandle != INVALID_HANDLE_VALUE &&
        !CreateIoCompletionPort(fileHandle.m_hFileHandle.get(), m_hCompletionPort.get(),
                                reinterpret_cast<ULONG_PTR>(&fileHandle), 0)) {

        throw std::exception("Failed to attach handle to I/O completion port");
    }
}

void
OverlappedIOController::UnregisterHandle(
    FileHandle &handle)
{
    /* No special actions required - completion port tracks handles references. */
}

void
OverlappedIOController::DispatcherThread()
{
    std::cout<<"DEBUG:OverlappedIOController::DispatcherThread  \n";
    while (true) {

        // Get the next operation from the queue.
        DWORD bytes_transferred = 0;
        ULONG_PTR completion_key = 0;
        LPOVERLAPPED io_cb = 0;
        DWORD error = 0;
        
        ::SetLastError(0);
        

        BOOL ok = ::GetQueuedCompletionStatus(m_hCompletionPort.get(), &bytes_transferred,
            &completion_key, &io_cb, INFINITE);

        if (!ok)
        {
            std::string error = GetSystemError();
            std::cout << "Error in Processing :" << error.c_str() << "\n";
            throw;
        }

        if (completion_key == CK_STOP)
        {
            std::cout << "OverlappedIOController::DispatcherThread Received STOP \n";
            break;
        }

        else if (io_cb)
        {
            if (completion_key)
            {
                //std::cout << "DEBUG:OverlappedIOController::DispatcherThread in Queue: Transfer size :" << bytes_transferred << "\n";
                //auto start = std::chrono::high_resolution_clock::now();
                
                reinterpret_cast<FileHandle *>(completion_key)->
                    IOCompleteCallback(io_cb, bytes_transferred, error);

                //auto end = std::chrono::high_resolution_clock::now();

                //auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            }
        }
        else
        {
            std::cout << "ERROR:OverlappedIOController::DispatcherThread Unknown Queue Status \n";
            throw;
        }
    }
}