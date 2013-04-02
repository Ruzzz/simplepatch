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
#include "SPatchOptions.h"


const char USAGE[] =
{
    "Simple Patch v"VERSION" by Ruzzz\n"
    "Apply simple patch file to target file.\n"
    "Usage: spatch [options] target-file patch-file[.sdiff|.simplediff]\n"
    "\n"
    "Options:\n"
    "  -nobackup|-rewrite       - Don't create backup file <filename.original>\n"
    "                             or rewrite backup file if exists.\n"
};


int _tmain(const int argc, const tchar *argv[])
{
    if (argc < 3)
    {
        std::cout << USAGE;
        return EXIT_FAILURE;
    }
    else
    {
        SPatch::Options options(argc, argv);
        if (!options.isValid())
            MAIN_ABORT(options.getLastError().toString())
        else
        {
            // create backup
            if (!options.noBackup())
            {
                tstring backupFileName(options.targetFileName());
                backupFileName += _T(".original");
                if (!FileUtil::copyFile(options.targetFileName(), backupFileName.c_str(),
                    options.rewrite()))
                {
                    if (!options.rewrite() && FileUtil::existsFile(backupFileName.c_str()))
                        PRINT_ERROR("Backup file exists, use -rewrite")
                    else
                        PRINT_ERROR("Can not create backup file")
                    return EXIT_FAILURE;
                }
            }

            // apply patch
            Patch::Patcher patcher;
            if (!(patcher.load(options.patchFileName()) && patcher.apply(options.targetFileName())))
                MAIN_ABORT(patcher.getLastError().toString())

            // TODO Print OK message
        }
    }

#if defined(_WIN32) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif
    return EXIT_SUCCESS;
}
