#pragma once
#include "interfaces.h"

class OverlappedIOController;
class FileProcessor : public IFileProcessor
{
public:
    FileProcessor(void);
    virtual ~FileProcessor(void);

    /*Open file.*/
    IFileStream::Ptr
    Open(const std::string &name,const std::string &mode);

    void Enable();

private:
    /** I/O controller instance. */
    std::unique_ptr<OverlappedIOController> m_ptrIOcontroller;

    void RegisterStream(IFileStream::Ptr);
};
