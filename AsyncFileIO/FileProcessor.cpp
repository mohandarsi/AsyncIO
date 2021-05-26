
#include "logging.h"

#include "FileProcessor.h"
#include "FileMode.h"
#include "FileStream.h"
#include "OverlappedIOController.h"


namespace AsyncFileIO
{

FileProcessor::FileProcessor(spdlog::logger& log)
    : m_logger(log)
    , m_iocontroller(std::make_unique<OverlappedIOController>(log))
{
    m_logger.trace("FileProcessor::FileProcessor");
}

FileProcessor::~FileProcessor()
{
    m_logger.trace("FileProcessor::~FileProcessor");
    m_iocontroller->disable();
}
void FileProcessor::enable()
{
    m_logger.trace("FileProcessor::enable");
    m_iocontroller->enable();
}

std::unique_ptr<IFileStream>
FileProcessor::open(const std::string& name, const std::string& mode)
{
    m_logger.debug("FileProcessor::open fileName: {} with mode {} ", name, mode);
    auto fileStream = std::make_unique<FileStream>(m_logger,name, FileMode(mode));
    registerStream(*fileStream);
    return fileStream;
}

void
FileProcessor::registerStream(FileStream& stream) const
{
    m_iocontroller->registerHandle(*stream.m_ptrFileHandle.get());
}

}