//
// Project: Simple Patch
// Date:    2013-02-08
// Author:  Ruslan Zaporojets <ruzzzua[]gmail.com>
//


#if defined(_WIN32) && defined(_DEBUG)
#define _CRTDBG_MAPALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include "defs.h"
#include "Patch.h"


const char USAGE[] =
{
    "Simple Diff v" VERSION  " by Ruslan Zaporojets\n"
    "Create simple patch file.\n"
    "Usage: zsdiff old-file new-file patch-file\n"
    "\n"
    "Note: old-file and new-file must have the same size.\n"
};


int _tmain(const int argc, const tchar *argv[])
{
    if (argc == 4)
    {
        Patch::Patcher patcher;
        if (!(patcher.compare(argv[1], argv[2]) && patcher.save(argv[3])))
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
