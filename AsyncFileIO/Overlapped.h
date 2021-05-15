#pragma once

#include<memory>
#include<future>

#include <Windows.h>
namespace AsyncFileIO
{
struct Overlapped : OVERLAPPED
{
    Overlapped(std::weak_ptr<void> handle);
    ~Overlapped();

    static VOID CALLBACK callback(DWORD errorCode, DWORD numberOfBytesTransferred, LPOVERLAPPED lpOverlapped);
    std::weak_ptr<void> fileHandle;
    std::promise<IOStatus> status;
};
}