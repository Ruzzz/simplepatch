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


#define VERSION "0.2"
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

int _tmain(int argc, const tchar *argv[])
{
    bool doBackup = true;
    if (argc != 3 && (argc != 4 || (doBackup = _tcscmp(argv[3], _T("-nobackup")) != 0)))
    {
        std::cout << HELP;
        return 1;
    }

    // create backup
    if (doBackup)
    {
        tstring backupFileName(argv[1]);
        backupFileName += _T(".original");
#ifdef _WIN32
        if (!::CopyFileW(argv[1], backupFileName.c_str(), FALSE))
            EXIT_ERROR("Can not create backup file")
#else
        // TODO
#endif
    }

    // open file
      // TODO Move to class Patch, but see 'size' below
    std::fstream targetFile(argv[1], std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);
    if (!targetFile)
        EXIT_ERROR("Can not open target file")
    const auto size = targetFile.tellg();
    if (!size)
        EXIT_ERROR("Target file is empty")
    else
    {
        // apply patch
        Patch patch;
        if (!(patch.parse(argv[2], static_cast<size_t>(size)) && patch.apply(targetFile)))
            EXIT_ERROR(patch.getLastError().toString());
    }

    return 0;
}
