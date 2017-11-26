#pragma once

#include <windows.h>
#include <iostream>
#include <vector>



template<typename T> struct Deleter {
    void operator()(T *p)
    {
        delete p;
    }
};