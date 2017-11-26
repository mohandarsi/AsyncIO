#include "FileStream.h"
#include "IORequest.h"
#include "FileHandle.h"

FileMode::FileMode(const std::string &mode_str)
{
    read = false;
    write = false;
    extended = false;
	append = false;

    /* The first character should specify read or write access. */
    if (mode_str[0] == 'r') {
        read = true;
    } else if (mode_str[0] == 'w') {
        write = true;
    } 
	else if (mode_str[0] == 'a') {
		append = true;
	}
	else {
        throw std::exception("Invalid access type, should be either 'r' or 'w'");
    }
    if (mode_str[1] == 0) {
        return;
    }
    if (mode_str[1] == '+') {
        extended = true;
    }
}

const FileStream::Offset FileStream::OFFSET_NONE = -1;
const FileStream::Offset FileStream::OFFSET_END = LLONG_MAX;

FileStream::FileStream(const std::string &path, FileMode mode):
    m_mode(mode),m_ptrFileHandle(new FileHandle(path,mode))
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

FileStream::Offset
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

IRequest::Ptr
FileStream::Write(const void *data, size_t len,
                                    Offset offset,
                                    WriteHandler completion_handler)
{

    //std::cout<<"DEBUG:FileStream::Write \n";
    //make sure no write is currently handled
    if(m_ptrFileHandle->GetWriteActivity() ==true)
        throw std::exception("Currently Write is under Progress for this stream");
    
    if(offset == OFFSET_NONE)
       offset = m_currentPos;
    
	WriteRequest* rawWriterequest = new WriteRequest(offset, data, len);// dynamic_cast<WriteRequest*>(writeRequest.get());
    /*rawWriterequest->AddCompletionHandler(boost::bind(&FileStream::HandleWriteCompletion, this,_1,_2));
    if(completion_handler != NULL)
        rawWriterequest->AddCompletionHandler(completion_handler);*/
	auto callback = std::bind(&FileStream::HandleWriteCompletion, this, std::placeholders::_1, std::placeholders::_2);
	rawWriterequest->AddCompletionHandler(callback);
	if (completion_handler != NULL)
		rawWriterequest->AddCompletionHandler(completion_handler);
	IRequest::Ptr writeRequest(rawWriterequest);
    m_ptrFileHandle->Write(writeRequest);
    return writeRequest;

}

void FileStream::WriteSync(const void *data, size_t len, Offset offset)
{
    IRequest::Ptr ioptr = Write(data,len,offset);
    ioptr->Wait();
}

void
FileStream::HandleWriteCompletion(IOResult result,std::size_t length)
{

    //XXX use real number of bytes written
    if (result == OK ) {

        m_currentPos += length;
    }

}