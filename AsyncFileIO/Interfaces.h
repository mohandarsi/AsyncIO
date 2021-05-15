#pragma once
#include<future>

#ifdef ASYNCFILEIO_EXPORTS
#define ASYNCFILEIO_API __declspec(dllexport)
#else
#define ASYNCFILEIO_API __declspec(dllimport)
#endif

#include "spdlog/spdlog.h"

namespace AsyncFileIO
{
    class IFileProcessor;
    ASYNCFILEIO_API std::unique_ptr<IFileProcessor>  CreateFileProcessor(spdlog::logger& log);
}

