
#include <sstream>
#include "OverlappedIOController.h"
#include "FileHandle.h"
#include "Utils.h"
#include "Overlapped.h"

namespace FileAPI
{

#define DISPATCHER_STOP (0)

OverlappedIOController::OverlappedIOController(spdlog::logger& log)
    : m_logger(log)
{
    auto* const hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (!hCompletionPort) {
        throw std::exception("Failed to create I/O completion port");
    }
    m_hCompletionPort = std::shared_ptr<void>(hCompletionPort,CloseHandle);
}

OverlappedIOController::~OverlappedIOController()
{
    disable();
}

void OverlappedIOController::enable()
{
    if (m_dispatcherThreads.empty()) 
    {
        const auto concurrentThreads = std::thread::hardware_concurrency();

        m_dispatcherThreads.resize(concurrentThreads);

        m_logger.info("HardWare concurrency {}", concurrentThreads);

        for (auto& dispatcherThread : m_dispatcherThreads)
        {
            dispatcherThread = std::thread(&OverlappedIOController::dispatcherThread, this);
        }
    }
}

void OverlappedIOController::disable()
{

    // Signal dispatcher to exit.
    std::stringstream ss; ss << "posting STOP message to the ALL dispatcher threads";
    for (auto& dispatcherThread : m_dispatcherThreads)
    {
        m_logger.info(ss.str());
        if (0 == ::PostQueuedCompletionStatus(m_hCompletionPort.get(), 0, DISPATCHER_STOP, 0))
        {
            m_logger.error("PostQueuedCompletionStatus failed with error {0} for handle {1}", getLastSystemError(), m_hCompletionPort.get());
        }
    }

    for (auto& dispatcherThread : m_dispatcherThreads)
    {
        std::stringstream ss; ss << "waiting for a thread " << dispatcherThread.get_id() << " to stop";
        m_logger.info(ss.str());
        dispatcherThread.join();
    }

    m_dispatcherThreads.clear();
}

void OverlappedIOController::registerHandle(FileHandle &handle) const
{
    if (!CreateIoCompletionPort(handle.m_fileHandle.get(), m_hCompletionPort.get(),
                                reinterpret_cast<ULONG_PTR>(&handle), 0)) 
    {

          throw std::exception("Failed to attach handle to I/O completion port");
    }
}

void OverlappedIOController::unregisterHandle( FileHandle &handle)
{
    /* No special actions required - completion port tracks handles references. */
}

void OverlappedIOController::dispatcherThread()
{
    m_logger.trace("dispatcherThread started");

    while (true) 
    {
        // Get the next operation from the queue.
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED ioCb = nullptr;

        ::SetLastError(0);

        const auto ok = ::GetQueuedCompletionStatus(m_hCompletionPort.get(), &bytesTransferred,
                                                    &completionKey, &ioCb, INFINITE);

        if (!ok)
        {
            m_logger.error("GetQueuedCompletionStatus failed with error {0} for handle {1}", getLastSystemError(), m_hCompletionPort.get());
            throw;
        }

        if (completionKey == DISPATCHER_STOP)
        {
           m_logger.info("DispatcherThread Received STOP signal");
           break;
        }

        if(!ioCb)
        {
            m_logger.error("GetQueuedCompletionStatus overlapped result is null");
            throw;
        }


        if (completionKey)
        {

            m_logger.debug("OVERLAPPED IO number of bytes transferred {}", bytesTransferred);

            Overlapped::callback(::GetLastError(), bytesTransferred, ioCb);
        }
        
    }

    m_logger.trace("dispatcherThread stopped");
}

}