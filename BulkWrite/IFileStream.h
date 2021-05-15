#pragma once

#include <future>
#include "Definitions.h"

namespace FileAPI
{

enum class RelativePosition
{
    BEGIN,
    CURRENT
};

class IFileStream
{
public:

    //OFFSET_NONE indicates to read or write operations to consider offset as current file position
    virtual std::future<IOStatus>
    write(const void *data, size_t len, Offset offset) = 0;

    virtual std::future<IOStatus>
    read(void *data, size_t length, Offset offset) = 0;

    virtual Offset seek(Offset position, RelativePosition relativePosition = RelativePosition::BEGIN) = 0;
    virtual void setSize(size_t len) =0;

    virtual ~IFileStream() = default;
};

}

