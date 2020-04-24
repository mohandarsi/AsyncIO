
#include<iostream>
#include "FileStream.h"

#include "FileHandle.h"
#include "FileMode.h"

namespace FileAPI
{

FileStream::FileStream(const std::string &path, FileMode mode):m_ptrFileHandle(std::make_unique<FileHandle>(path,mode))
{
   SetName(path);
   m_state = OPENED;
   m_currentPos = 0;
}

FileStream::~FileStream(void)
{
	std::cout << " \n DEBUG:Destructor FileStream \n";
}

std::string
FileStream::GetName() const
{
    return m_szName;
}

void FileStream::SetFileSize(size_t len)
{
	m_ptrFileHandle->SetFileSize(len);
}

void
FileStream::SetName(const std::string& new_name)
{
    m_szName = new_name;
}

Offset
FileStream::Seek(Offset pos, bool is_relative)
{
    Offset new_pos = m_currentPos;
    if (is_relative) {
        new_pos += pos;
    } else {
        new_pos = pos;
    }
    if (new_pos < 0) {
        throw std::exception("Invalid position specified (negative)");
    }
    m_currentPos = new_pos;
    return new_pos;
}

std::future<IOStatus>
FileStream::Write(const void *data, size_t len,
                                    Offset offset)
{

    std::cout<<"DEBUG:FileStream::Write \n";
    return m_ptrFileHandle->Write(offset,data, len);

}

void FileStream::WriteSync(const void *data, size_t len, Offset offset)
{
    auto ioptr = Write(data,len,offset);
    ioptr.wait();
}

}