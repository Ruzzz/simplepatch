//
// Project: Simple Patch
// Date:    2013-03-28
// Author:  Ruzzz <ruzzzua[]gmail.com>
//

#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

class FileUtil
{
public:
    static bool copyFile(const char *from, const char *to)
    {
#ifdef _WIN32
        return ::CopyFileA(from, to, 0) != 0;
#else
        return false;
#endif
    }

    static bool copyFile(const wchar_t *from, const wchar_t *to)
    {
#ifdef _WIN32
        return ::CopyFileW(from, to, 0) != 0;
#else
        return false;
#endif
    }

private:
    FileUtil();
    FileUtil(FileUtil&);
    void operator=(FileUtil&);
};
