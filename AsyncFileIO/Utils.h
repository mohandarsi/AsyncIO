#pragma once

#include<string>

#include "Definitions.h"

namespace AsyncFileIO
{

    Status MapError(DWORD error);
    std::string getLastSystemError();
    std::string getErrorDescription(DWORD);

}