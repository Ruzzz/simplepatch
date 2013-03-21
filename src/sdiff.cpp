/*
 *  Version: 1.0
 *  Date:    2013/02/08
 *  Author:  Ruzzz ruzzzua[]gmail.com
 */

#include <iostream>
#include "defs.h"
#include "Patch.h"

const char HELP[] =
{
    "Simple Diff v"VERSION" by Ruzzz\n"
    "Create simple patch file.\n"
    "Usage: sdiff oldfile newfile patchfile\n"
    "\n"
    "Note: oldfile and newfile must have the same size.\n"
};

int _tmain(int argc, const tchar *argv[])
{
    if (argc != 4)
    {
        std::cout << HELP;
        return 1;
    }
    else
    {
        Patch patch;
        if (!(patch.compare(argv[1], argv[2]) && patch.save(argv[3])))
        {
            std::cerr << patch.getLastError().toString() << std::endl;
            return 1;
        }
    }
    return 0;
}
