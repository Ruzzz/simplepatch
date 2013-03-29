//
// Project: Simple Patch
// Date:    2013-02-08
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#include <iostream>
#include "defs.h"
#include "Patch.h"


const char USAGE[] =
{
    "Simple Diff v"VERSION" by Ruzzz\n"
    "Create simple patch file.\n"
    "Usage: sdiff old-file new-file patch-file\n"
    "\n"
    "Note: old-file and new-file must have the same size.\n"
};


int _tmain(int argc, const tchar *argv[])
{
    if (argc != 4)
    {
        std::cout << USAGE;
        return 1;
    }
    else
    {
        Patch::Patcher patcher;
        if (!(patcher.compare(argv[1], argv[2]) && patcher.save(argv[3])))
        {
            std::cerr << patcher.getLastError().toString() << std::endl;
            return 1;
        }
    }
    return 0;
}
