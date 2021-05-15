#pragma once

#include "IFileStream.h"

namespace spdlog {
    class logger;
}

namespace FileAPI
{

/** Stream class which represents opened file. */
class FileHandle;
class FileMode;

class FileStream : public IFileStream
{
public:
    
    /** Default prototype for read operation completion handler. */
     typedef   void   (*ReadHandler) (void*,Status);

    /** Default prototype for close operation completion handler. */
    //typedef   void    (*CloseHandler) (void );

    /** Stream states. */
    enum  class FileState {
        /** Stream is closed. Also initial state. */
        CLOSED,
        /** Stream is being opened and is not yet ready for reading/writing.
         * This process may fail. */
        OPENING,
        /** Stream is being passively opened and is not yet ready for reading/writing.
         * This process may fail. */
        OPENING_PASSIVE,
        /** Stream is opened and is ready for read/write operations. */
        OPENED
    };

public:
    FileStream(spdlog::logger& log, const std::string &path, const FileMode& mode);
    virtual ~FileStream();

    FileState getState() const
    {
        return m_state;
    }

    bool isClosed() const
    {
        return (m_state == FileState::CLOSED);
    }

    void setName(const std::string&);

    std::string  getName() const;

    std::future<IOStatus>
    write(const void *data, size_t length, Offset offset=OFFSET_NONE) override;
    
    std::future<IOStatus>
    read(void *data, size_t length, Offset offset = OFFSET_NONE) override;
    
    Offset seek(Offset position, RelativePosition relativePosition = RelativePosition::BEGIN) override;
    
    void writeSync(const void *data, size_t length, Offset offset=OFFSET_NONE);

    void setSize(size_t length) override;

private:

    spdlog::logger& m_logger;

    friend class FileProcessor;

    /** Current state of the stream. */
    FileState m_state ;

    /** Human readable stream name. */
    std::string m_szName;

   
    /** Current position in file when maintained by stream. */
    Offset m_currentPos;

    /** Native handle instance. */
    std::unique_ptr<FileHandle> m_ptrFileHandle;

};

}
