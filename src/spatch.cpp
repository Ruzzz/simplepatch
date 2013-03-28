//
// Project: Simple Patch
// Date:    2013-02-08
// Author:  Ruzzz <ruzzzua[]gmail.com>
//

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
    "Usage: spatch target-file patch-file[.sdiff|.simplediff] [-nobackup]\n"
};

int _tmain(int argc, const tchar *argv[])
{
    bool doBackup = true;
    if (argc != 3 && (argc != 4 || (doBackup = _tcscmp(argv[3], _T("-nobackup")) != 0)))
    {
        std::cout << USAGE;
        return 1;
    }
    else
    {
        // create backup
        if (doBackup)
        {
            tstring backupFileName(argv[1]);
            backupFileName += _T(".original");
            if (!FileUtil::copyFile(argv[1], backupFileName.c_str()))
                MAIN_ABORT("Can not create backup file")
        }

        // apply patch
        Patch::Patcher patcher;
        if (!(patcher.load(argv[2]) && patcher.apply(argv[1])))
            MAIN_ABORT(patcher.getLastError().toString())
    }
    return 0;
}
