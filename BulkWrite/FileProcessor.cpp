#include "FileProcessor.h"
#include "FileStream.h"
#include "IORequest.h"
#include "FileHandle.h"
#include "OverlappedIOController.h"

IFileProcessor* CreateFileProcessor()
{
    return new FileProcessor();
}
FileProcessor::FileProcessor(void):
         m_ptrIOcontroller(new OverlappedIOController())
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

IFileStream::Ptr
FileProcessor::Open(const std::string &name, const std::string &mode)
{
    IFileStream::Ptr stream(new FileStream(name, mode));
    RegisterStream(stream);
    return stream;
}

void
FileProcessor::RegisterStream(IFileStream::Ptr stream)
{
    FileStream* fileStream = dynamic_cast<FileStream*>(stream.get());
    fileStream->m_ptrFileHandle->SetStream(stream);
    m_ptrIOcontroller->RegisterHandle(*fileStream->m_ptrFileHandle);
}