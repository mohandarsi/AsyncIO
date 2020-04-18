#pragma once
#include "interfaces.h"

class OverlappedIOController;
class FileStream;
class FileProcessor : public IFileProcessor
{
public:
    FileProcessor(void);
    virtual ~FileProcessor(void);

    /*Open file.*/
	std::unique_ptr<IFileStream>
    Open(const std::string &name,const std::string &mode) override;

    void Enable();

private:
    /** I/O controller instance. */
    std::unique_ptr<OverlappedIOController> m_ptrIOcontroller;

    void RegisterStream(FileStream&);
};
