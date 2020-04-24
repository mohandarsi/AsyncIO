#include<iostream>


#include "windows.h"

#include "Utils.h"
#include "Overlapped.h"


namespace FileAPI
{

Overlapped::Overlapped() { 
    std::memset(static_cast<OVERLAPPED *>(this), 0, sizeof(OVERLAPPED)); 
}
Overlapped::~Overlapped() {
    std::cout << " ~Overlapped()";
}
VOID CALLBACK Overlapped::callback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
    std::unique_ptr<Overlapped> overlapped(
        static_cast<Overlapped *>(lpOverlapped));

    if (auto handle = overlapped->handle.lock())
    {
        std::cout << "Handle " << handle << ": Result :" << getErrorDescription(dwErrorCode);
    }
    overlapped->status.set_value({ MapError(dwErrorCode),dwNumberOfBytesTransfered });
}

}