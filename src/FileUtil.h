//
// Project: Simple Patch
// Date:    2013-03-28
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#pragma once


#ifdef _WIN32
#include <windows.h>
#else
#include <fstream>
#endif


class FileUtil
{
public:

#ifdef _WIN32
    
    static bool copyFile(const char *from, const char *to, bool rewrite = false)
    {
        return ::CopyFileA(from, to, rewrite ? FALSE : TRUE) != FALSE;
    }

    static bool copyFile(const wchar_t *from, const wchar_t *to, bool rewrite = false)
    {
        return ::CopyFileW(from, to, rewrite ? FALSE : TRUE) != FALSE;
    }

    static bool existsFile(const char *fileName)
    {
        return ::GetFileAttributesA(fileName) != DWORD(-1);
    }

    static bool existsFile(const wchar_t *fileName)
    {
        return ::GetFileAttributesW(fileName) != DWORD(-1);
    }

#else  // OTHERS OS

    template<typename TChar>
    static bool existsFile(const TChar *fileName)
    {
        return std::ifstream(fileName) != nullptr;
    }

#endif  // _WIN32

private:
    FileUtil();
    FileUtil(FileUtil&);
    void operator=(FileUtil&);
};
