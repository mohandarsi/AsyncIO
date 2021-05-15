#pragma once
#include "IFileProcessor.h"

namespace spdlog {
    class logger;
}

namespace FileAPI
{

class OverlappedIOController;
class IFileStream;
class FileStream;

class FileProcessor : public IFileProcessor
{
public:
    explicit FileProcessor(spdlog::logger& log);
    virtual ~FileProcessor();

    std::unique_ptr<IFileStream>
    open(const std::string &name,const std::string &mode) override;

    void enable() override;

private:
    void registerStream(FileStream&) const;

private:
    spdlog::logger& m_logger;
    std::unique_ptr<OverlappedIOController> m_iocontroller;
};

}
