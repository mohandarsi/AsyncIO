#include "FileMode.h"

namespace FileAPI
{

FileMode::FileMode(const std::string& mode)
{
    read = false;
    write = false;
    extended = false;
    append = false;

    /* The first character should specify read or write access. */
    if (mode[0] == 'r') {
        read = true;
    }
    else if (mode[0] == 'w') {
        write = true;
    }
    else if (mode[0] == 'a') {
        append = true;
    }
    else {
        throw std::exception("Invalid access type, should be either 'r' or 'w'");
    }
    if (mode[1] == 0) {
        return;
    }
    if (mode[1] == '+') {
        extended = true;
    }
}

}