/*
 *  Version: 1.0
 *  Date:    2013/02/08
 *  Author:  Ruzzz ruzzzua[]gmail.com
 */

#include <iostream>
#include "defs.h"
#include "Patch.h"

#define VERSION "0.2"
#define EXIT_ERROR(msg) { std::cerr << msg << std::endl; return 1; }

const char HELP[] =
{
    "Simple Diff v"VERSION" by Ruzzz\n"
    "Create simple patch file.\n"
    "Usage: sdiff oldfile newfile patchfile\n"
    "\n"
    "Note: oldfile and newfile must have the same size.\n"
};

#ifdef _WIN32
typedef unsigned __int64 uint64_t;
#endif

int _tmain(int argc, const tchar *argv[])
{
    if (argc != 4)
    {
        std::cout << HELP;
        return EXIT_FAILURE;
    }
    else
    {
        // create patch
        Patch patch;
        if (!(patch.create(argv[1], argv[2]) && patch.save(argv[3])))
            EXIT_ERROR(patch.getLastError().toString())
    }
    return 0;
}
