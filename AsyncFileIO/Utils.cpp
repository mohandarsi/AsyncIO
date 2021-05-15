#pragma once
#include <sstream>      // std::stringstream

#include "Windows.h"
#include "Utils.h"

namespace AsyncFileIO
{
std::string getErrorDescription(DWORD code)
{
    char desc[1024];
    DWORD size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        desc, sizeof(desc), NULL);
    /* Trim trailing new line. */
    if (size >= 2 && desc[size - 2] == 0xd && desc[size - 1] == 0xa) {
        size -= 2;
    }
    std::stringstream ss;
    ss << code << " - " << std::string(desc, size);
    return ss.str();
}

std::string
getLastSystemError()
{
    return getErrorDescription(::GetLastError());
}



/*
  Windows error to Library specific error
*/

Status MapError(DWORD error)
{
    switch (error) {
    case ERROR_SUCCESS:
        return Status::OK;
    case ERROR_OPERATION_ABORTED:
        return Status::ABORTED;
    case ERROR_INVALID_HANDLE:
        return Status::CLOSED;
    case ERROR_IO_INCOMPLETE:
    case ERROR_IO_PENDING:
        return Status::PENDING;
    default:
        return Status::OTHER_FAILURE;
    }
}

}