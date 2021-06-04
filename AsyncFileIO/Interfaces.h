#pragma once

#include<future>
#include  "dllexportimport.h"

#include "spdlog/spdlog.h"

namespace AsyncFileIO
{
    class IFileProcessor;
    ASYNCFILEIO_API std::unique_ptr<IFileProcessor>  CreateFileProcessor(spdlog::logger& log);
}

