#include "windows.h"

#include<iostream>
#include "Utils.h"
#include "Overlapped.h"
#include "spdlog/spdlog.h"

namespace FileAPI
{

Overlapped::Overlapped()
{ 
    std::memset(static_cast<OVERLAPPED *>(this), 0, sizeof(OVERLAPPED)); 
}

VOID CALLBACK Overlapped::callback(const DWORD errorCode, const DWORD numberOfBytesTransferred, const LPOVERLAPPED lpOverlapped)
{
    std::unique_ptr<Overlapped> overlapped(static_cast<Overlapped*>(lpOverlapped));

    if (const auto handle = overlapped->handle.lock())
    {
          SPDLOG_TRACE("Handle {} and overlapped operation completed with result  {} and bytes transferred {}",handle, getErrorDescription(dwErrorCode), numberOfBytesTransferred);
    }
    overlapped->status.set_value({ MapError(errorCode),numberOfBytesTransferred });
}

}