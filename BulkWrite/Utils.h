#pragma once

#include<string>

#include "Definitions.h"

namespace FileAPI
{

    Status MapError(DWORD error);
    std::string getLastSystemError();
    std::string getErrorDescription(DWORD);

}