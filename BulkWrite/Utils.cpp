#pragma once
#include "interfaces.h"

std::string
GetSystemError()
{
    DWORD code = GetLastError();
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

/*
  Windows error to Libarary specific error
*/

IOResult MapError(DWORD error)
{
    switch (error) {
    case ERROR_SUCCESS:
        return OK;
    case ERROR_ACCESS_DENIED:
        return PERMISSION_DENIED;
    case ERROR_HANDLE_EOF:
        return END_OF_FILE;
    case ERROR_OPERATION_ABORTED:
        return CANCELED;
    case ERROR_INVALID_HANDLE:
        return CLOSED;
    default:
        return OTHER_FAILURE;
    }
}