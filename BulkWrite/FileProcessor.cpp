
#include "spdlog/spdlog.h"

#include "FileProcessor.h"
#include "FileMode.h"
#include "FileStream.h"
#include "FileHandle.h"
#include "OverlappedIOController.h"


namespace FileAPI
{

FileProcessor::FileProcessor(spdlog::logger& log)
    : m_logger(log)
    , m_iocontroller(std::make_unique<OverlappedIOController>(log))
{
    m_logger.trace("FileProcessor");
}

FileProcessor::~FileProcessor()
{
    m_logger.trace("~FileProcessor");
    m_iocontroller->disable();
}
void FileProcessor::enable()
{
    m_logger.trace("Enable");
    m_iocontroller->enable();
}

std::unique_ptr<IFileStream>
FileProcessor::open(const std::string &name, const std::string &mode)
{
    auto* fileStream = new FileStream(m_logger,name, FileMode(mode));
    registerStream(*fileStream);
    return std::unique_ptr<IFileStream>(fileStream);
}

void
FileProcessor::registerStream(FileStream& stream) const
{
    m_iocontroller->registerHandle(*stream.m_ptrFileHandle.get());
}

}