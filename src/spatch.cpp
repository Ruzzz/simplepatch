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
#include <sstream>
#include <string>
#include <vector>
#include <map>

typedef std::vector<unsigned char> TBytes;

const char *SIGNATURE = "SIMPLEDIFF";
char HELP[] =
{
    "Copy oldfile to newfile and apply patchfile.\n"
    "Usage: spatch oldfile newfile patchfile\n"
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
    size_t oldFileSize = static_cast<size_t>(oldFile.tellg());
    if (oldFileSize == 0)
        error("oldfile is empty");
    oldFile.seekg(0);

    // newfile
    std::fstream newFile(argv[2], std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    if (newFile.fail())
        error("Can not create newfile");

    // clone file
    newFile << oldFile.rdbuf();
    if (oldFile.fail() || newFile.fail())
        error("Can not copy oldfile to newfile");
    oldFile.close();

    // patchfile
    std::ifstream patchFile(argv[3], std::ios::in);
    if (patchFile.fail())
        error("Can not open patchfile");

    // parse patchfile

	if (!patchFile.eof())
	{
		std::string signature;
		std::getline(patchFile, signature);
		if (signature != SIGNATURE)
			error("Invalid signature in patchfile");
	}

    std::map<size_t, TBytes> diffData;
    while (!patchFile.eof())
    {
        // read full line
        std::string line;
        std::getline(patchFile, line);

        // skip empty lines
        if (line.empty())
            continue;
        std::istringstream lineStream(line);
        lineStream >> std::ws;
        if (lineStream.eof())
            continue;

        // offset
        size_t offset = 0;
        lineStream >> std::hex >> offset;
        if (offset >= oldFileSize)
            error("Invalid offset in patchfile");

        // bytes
        TBytes bytes;
        lineStream >> std::ws;
        while (!lineStream.eof())
        {
            unsigned int b = 0;
            lineStream >> std::hex >> b;
            bytes.push_back(static_cast<unsigned char>(b));
            lineStream >> std::ws;
        }
        if (bytes.size() > 0)  // TODO Print error?
            diffData[offset] = bytes;  // TODO Check exists
    }

    // apply
    const auto last = diffData.cend();
    auto it = diffData.cbegin();
    for (; it != last; ++it)
    {
        size_t offset = it->first;
        TBytes bytes = it->second;
        newFile.seekg(offset);
        if (newFile.fail())
            error("Error patching newfile");
        newFile.write((const char *)bytes.data(), bytes.size());
		if (newFile.bad())
			error("Error writing to newfile");
    }

    return EXIT_SUCCESS;
}
