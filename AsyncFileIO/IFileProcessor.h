#pragma once
#include <memory>
#include <string>

namespace AsyncFileIO
{
class IFileStream;

class IFileProcessor
{

public:
    /*Open file.
    "r"	read: Open file for input operations. The file must exist.
    "r+"	read/update: Open a file for update (both for input and output). The file must exist.
    "w"	write: Create an empty file for output operations.
    If a file with the same name already exists,its contents are discarded and the file is treated as a new empty file.
    w+"	write/update: Create an empty file and open it for update (both for input and output).
    If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.

    */
    virtual std::unique_ptr<IFileStream> open(const std::string &name, const std::string &mode) = 0;
    virtual void enable() = 0;
    virtual ~IFileProcessor() = default;
};

}

