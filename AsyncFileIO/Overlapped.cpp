#include "windows.h"

#include<iostream>
#include "Utils.h"
#include "Overlapped.h"
#include "spdlog/spdlog.h"

namespace AsyncFileIO
{

Overlapped::Overlapped(std::weak_ptr<void> handle)
    : fileHandle(handle)
    , status{}
{
    SPDLOG_TRACE("Overlapped");
    std::memset(static_cast<OVERLAPPED *>(this), 0, sizeof(OVERLAPPED));
}

Overlapped::~Overlapped()
{
    SPDLOG_TRACE("~Overlapped");
    fileHandle.reset();

    
}

VOID CALLBACK Overlapped::callback(const DWORD errorCode, const DWORD numberOfBytesTransferred, const LPOVERLAPPED lpOverlapped)
{
    std::unique_ptr<Overlapped> overlapped(static_cast<Overlapped*>(lpOverlapped));

    const auto handle = overlapped->fileHandle.lock();
    SPDLOG_TRACE("Handle {} and overlapped operation completed with result  {} and bytes transferred {}",handle, 
        getErrorDescription(dwErrorCode), numberOfBytesTransferred);

    overlapped->status.set_value({ MapError(errorCode),numberOfBytesTransferred });
}

}