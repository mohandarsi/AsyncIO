#pragma once
#include<string>

namespace FileAPI
{

class FileMode
{
public:
    /** Read access. */
    bool read : 1,

        /** Write access.*/
        write : 1,

        /** Append.*/
        append : 1,

        /** Extended access (corresponds to "+" in opening mode string.)*/
        extended : 1;

    /*Parses the mode */
    FileMode(const std::string &mode_str);
    ~FileMode(void) {}
};

}

