/*
 *  Version: 1.0
 *  Date:    2013/02/08
 *  Author:  Ruzzz ruzzzua[]gmail.com
 */

#ifdef _WIN32
#define _UNICODE
#define UNICODE
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>

const char *SIGNATURE = "SIMPLEDIFF";
const char HELP[] =
{
    "Create simple patch.\n"
    "Usage: sdiff oldfile newfile patchfile\n"
    "Note: oldfile and newfile must have the same size.\n"
};

void error(const char *msg)
{
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}

#ifdef _WIN32
int wmain(int argc, const wchar_t *argv[])
#else
int main(int argc, const char *argv[])
#endif
{
    if (argc < 4)
    {
        std::cout << HELP;
        return EXIT_FAILURE;
    }

    // oldfile
    std::ifstream oldFile(argv[1], std::ios::in | std::ios::ate | std::ios::binary);
    if (oldFile.fail())
        error("Can not open oldfile");
    auto oldFileSize = oldFile.tellg();
    oldFile.seekg(0);

    // newfile
    std::ifstream newFile(argv[2], std::ios::in | std::ios::ate | std::ios::binary);
    if (newFile.fail())
        error("Can not open newfile");
    auto newFileSize = newFile.tellg();

    // check size
    if (oldFileSize != newFileSize)
        error("Files have different sizes");
    newFile.seekg(0);

    // patchfile
    std::ofstream patchFile(argv[3], std::ios::out | std::ios::trunc);
    if (patchFile.fail())
        error("Can not create patchfile");
    patchFile << std::setbase(16) << std::setfill('0');

    // init
	patchFile << SIGNATURE << std::endl;
#ifdef _WIN32
    unsigned __int64 fileSize = static_cast<unsigned __int64>(oldFileSize);
#else
    uint64_t fileSize = static_cast<uint64_t>(oldFileSize);
#endif
    unsigned int offsetLen = fileSize > 0xffffffff ? 16 : 8;
    bool isDiffMode = false;
    unsigned int b1, b2;

    // work
    while (oldFile.good() && newFile.good())
    {
        b1 = oldFile.get();
        if (oldFile.fail())
            error("Error reading from oldfile");
        b2 = newFile.get();
        if (newFile.fail())
            error("Error reading from newfile");
        if (b1 == b2)
        {
            if (isDiffMode)
            {
                isDiffMode = false;
                patchFile << '\n';
                if (patchFile.fail())
                    error("Error writing to patchfile");
            }
            continue;
        }
        // b1 != b2
        if (isDiffMode)
        {
            patchFile << " " << std::setw(2) << b2;
        }
        else
        {
            isDiffMode = true;
            size_t offset = static_cast<size_t>(newFile.tellg()) - 1;
            patchFile << std::setw(offsetLen) << offset << ' ';
            patchFile << std::setw(2) << b2;
        }
        if (patchFile.fail())
            error("Error writing to patchfile");
    }

    return EXIT_SUCCESS;
}
