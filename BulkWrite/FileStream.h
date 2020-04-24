#pragma once

#include "Interfaces.h"

namespace FileAPI
{

/** Stream class which represents opened file. */
class FileHandle;
class FileMode;


class FileStream : public IFileStream
{
public:
    
    /* Offset special value which indicates that the offset value is
     * not specified.
     */
    static const Offset OFFSET_NONE;

    /* Offset special value which indicates that the offset value
     * corresponds to the stream end .
     */
    static const Offset OFFSET_END;

    
    /** Default prototype for read operation completion handler. */
     typedef   void   (*ReadHandler) (void*,Status);

    /** Default prototype for close operation completion handler. */
    //typedef   void    (*CloseHandler) (void );

    /** Stream states. */
    enum  FileState {
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
    FileStream(const std::string &path, FileMode mode);
    ~FileStream(void);

        /** Get current state of the stream. */
    FileState
    GetState() const
    {
        return m_state;
    }

    /** Checks if stream is closed or not. */
    bool
    IsClosed() const
    {
        return (m_state == CLOSED);
    }

    /** Set the stream name. */
    void
    SetName(const std::string&);

     /** Get human readable stream name. */
    std::string
    GetName() const;

     /** Set new current position for the stream.*/
    Offset
     Seek(Offset pos, bool is_relative = false);

    /** Initiate write operation.*/
    virtual std::future<IOStatus>
    Write(const void *data, size_t len, Offset offset=OFFSET_NONE) override;
    
    void WriteSync(const void *data, size_t len, Offset offset=OFFSET_NONE);

    /** Initiate stream close operation.*/
    //void
    //Close(CloseHandler completion_handler);

    //support read operations in future

	void SetFileSize(size_t len) override;

private:
 
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