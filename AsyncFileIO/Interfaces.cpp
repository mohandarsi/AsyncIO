#include "Interfaces.h"
#include "FileProcessor.h"

namespace AsyncFileIO
{
    std::unique_ptr<IFileProcessor> CreateFileProcessor(spdlog::logger& log)
    {
        return std::make_unique<FileProcessor>(log);
    }
}
