#pragma once

#include<memory>
#include<future>

#include <Windows.h>
namespace FileAPI
{
struct Overlapped : OVERLAPPED
{
    Overlapped();
    ~Overlapped() = default;
    static VOID CALLBACK callback(DWORD errorCode, DWORD numberOfBytesTransferred, LPOVERLAPPED lpOverlapped);
    std::weak_ptr<void> handle;
    std::promise<IOStatus> status;
};
}