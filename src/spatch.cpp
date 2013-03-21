/*
 *  Version: 1.0
 *  Date:    2013/02/08
 *  Author:  Ruzzz ruzzzua[]gmail.com
 */

#include <iostream>
#include "defs.h"
#include "Patch.h"
#ifdef _WIN32
#include <windows.h>
#endif

#define EXIT_ERROR(msg) { std::cerr << msg << std::endl; return 1; }

const char HELP[] =
{
    "Simple Patch v"VERSION" by Ruzzz\n"
    "Apply simple patch file to target file.\n"
    "Usage: spatch targetfile patchfile[.sdiff|.simplediff] [-nobackup]\n"
};

#ifndef _tcscmp
#ifdef _UNICODE
#define _tcscmp wcscmp
#else
#define _tcscmp strcmp
#endif
#endif

bool copyFile(const tchar *from, const tchar *to)
{
#ifdef _WIN32
    return ::CopyFileW(from, to, FALSE) != FALSE;
#else
    return false;
#endif
}

int _tmain(int argc, const tchar *argv[])
{
    bool doBackup = true;
    if (argc != 3 && (argc != 4 || (doBackup = _tcscmp(argv[3], _T("-nobackup")) != 0)))
    {
        std::cout << HELP;
        return 1;
    }
    else
    {
        // create backup
        if (doBackup)
        {
            tstring backupFileName(argv[1]);
            backupFileName += _T(".original");
            if (!copyFile(argv[1], backupFileName.c_str()))
                EXIT_ERROR("Can not create backup file")
        }

        // apply patch
        Patch patch;
        if (!(patch.load(argv[2]) && patch.apply(argv[1])))
            EXIT_ERROR(patch.getLastError().toString())
    }
    return 0;
}
