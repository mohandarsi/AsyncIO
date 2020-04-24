#pragma once

#include<memory>
#include<future>

namespace FileAPI
{

struct Overlapped : OVERLAPPED
{
    Overlapped();
    ~Overlapped();
    static VOID CALLBACK callback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
    std::weak_ptr<void> handle;
    std::promise<IOStatus> status;
};

}