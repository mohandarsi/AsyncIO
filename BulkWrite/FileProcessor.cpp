#include "FileProcessor.h"
#include "FileStream.h"
#include "IORequest.h"
#include "FileHandle.h"
#include "OverlappedIOController.h"

std::unique_ptr<IFileProcessor> CreateFileProcessor()
{
    return std::make_unique<FileProcessor>();
}
FileProcessor::FileProcessor(void):
         m_ptrIOcontroller(std::make_unique<OverlappedIOController>())
{
}

FileProcessor::~FileProcessor(void)
{
	m_ptrIOcontroller->Disable();
}
void FileProcessor::Enable()
{
     m_ptrIOcontroller->Enable();
}

std::unique_ptr<IFileStream>
FileProcessor::Open(const std::string &name, const std::string &mode)
{
	auto fileStream = new FileStream(name, mode);
    RegisterStream(*fileStream);
	return std::unique_ptr<IFileStream>(fileStream);
}

void
FileProcessor::RegisterStream(FileStream& stream)
{
    m_ptrIOcontroller->RegisterHandle(*stream.m_ptrFileHandle.get());
}