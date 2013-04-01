//
// Project: Simple Patch
// Date:    2013-02-08
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#if defined(_WIN32) && defined(_DEBUG)
#define _CRTDBG_MAPALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "defs.h"
#include "Patch.h"
#include "FileUtil.h"


const char USAGE[] =
{
    "Simple Patch v"VERSION" by Ruzzz\n"
    "Apply simple patch file to target file.\n"
    "Usage: spatch target-file patch-file[.sdiff|.simplediff] [-nobackup|-rewrite]\n"
    "\n"
    "-nobackup                  - Don't create backup file <filename.original>.\n"
    "-rewrite                   - Rewrite backup file if exists.\n"
};


int _tmain(const int argc, const tchar *argv[])
{
    bool noBackup = false;
    bool rewrite = false;
    
    if (argc == 3 ||
           (argc == 4 &&
               ((rewrite = _tcscmp(argv[3], _T("-rewrite")) == 0) ||
               (noBackup = _tcscmp(argv[3], _T("-nobackup")) == 0))))
    {
        // create backup
        if (!noBackup)
        {
            tstring backupFileName(argv[1]);
            backupFileName += _T(".original");
            if (!FileUtil::copyFile(argv[1], backupFileName.c_str(), rewrite))
            {
                if (!rewrite && FileUtil::existsFile(backupFileName.c_str()))
                    PRINT_ERROR("Backup file exists, use -rewrite")
                else
                    PRINT_ERROR("Can not create backup file")
                return EXIT_FAILURE;
            }
        }

        // apply patch
        Patch::Patcher patcher;
        if (!(patcher.load(argv[2]) && patcher.apply(argv[1])))
            MAIN_ABORT(patcher.getLastError().toString())

        // TODO Print OK message
    }
    else
    {
        std::cout << USAGE;
        return EXIT_FAILURE;
    }

#if defined(_WIN32) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif
    return EXIT_SUCCESS;
}
