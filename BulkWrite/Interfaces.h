#pragma once
#include<future>

#ifdef BULKWRITE_EXPORTS
#define BULKWRITE_API __declspec(dllexport)
#else
#define BULKWRITE_API __declspec(dllimport)
#endif

#include "spdlog/spdlog.h"

namespace FileAPI
{
    class IFileProcessor;
    BULKWRITE_API std::unique_ptr<FileAPI::IFileProcessor>  CreateFileProcessor(spdlog::logger& log);
}

