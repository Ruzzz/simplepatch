#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>

#include "defs.h"
#include "Patch.h"
#include "Crc32.h"

#ifdef _WIN32
typedef unsigned __int64 uint64_t;
#endif

#define ERROR(code_) { lastError_ = Error(code_); return false; }

const char * SIGNATURE = "SIMPLEDIFF";
const tchar * const PATCH_FILE_EXTS[] = 
{
    _T(""),
    _T(".sdiff"),
    _T(".simplediff")
};

const char *Patch::Error::toString() const
{
    switch (code_)
    {
        case OK:
            return nullptr;
        case CANNOT_OPEN_PATCH:
            return "Cannot open patch file";
        case INVALID_SIGNATURE:
            return "Invalid signature in patch file";
        case INVALID_OFFSET_VALUE:
            return "Invalid offset in patch file";
        case INVALID_BYTE_VALUE:
            return "Invalid byte value in patch file";
        case CANNOT_CREATE_PATCH:
            return "Cannot create patch file";
        case CANNOT_OPEN_OLDFILE:
            return "Cannot open oldfile";
        case CANNOT_OPEN_NEWFILE:
            return "Cannot open newfile";
        case DIFFERENT_SIZE:
            return "Files have different size";
        case CANNOT_READ_OLDFILE:
            return "Error reading from old file";
        case CANNOT_READ_NEWFILE:
            return "Error reading from new filee";
        case CANNOT_WRITE_PATCH:
            return "Error writing to patch file";
        case CANNOT_APPLY_PATCH:
            return "Cannot apply patch";
        case INVALID_CRC32:
            return "Invalid crc32";
            
        default:
            return "Unknown error";
    }
}

bool Patch::apply(std::fstream &targetFile)
{
    targetFile.clear();
    targetFile.seekg(0);

    Crc32 hash;
    const unsigned int BUFFER_LEN = 1024 * 64;
    std::vector<char> buffer(BUFFER_LEN);
    while (!targetFile.eof())
    {
        targetFile.read((char *)buffer.data(), buffer.size());
        unsigned int readed = (unsigned int)targetFile.gcount();
        if (readed && targetFile.bad())
            ERROR(Error::CANNOT_APPLY_PATCH)
        if (readed)
            hash.compute((void *)buffer.data(), readed);
    }
    unsigned int currentHash = hash.value();
    if (crc32_ != currentHash)
        ERROR(Error::INVALID_CRC32)

    targetFile.clear();
    for (auto it : data_)
    {
        targetFile.seekg(it.first);
        if (targetFile)
        {
            Bytes bytes = it.second;
            targetFile.write((const char *)bytes.data(), bytes.size());
            if (!targetFile)
                ERROR(Error::CANNOT_APPLY_PATCH)
        }
        else
            ERROR(Error::CANNOT_APPLY_PATCH)
    }
    return true;
}

bool Patch::save(const tchar *patchFileName)
{
    std::ofstream patchFile(patchFileName, std::ios::out | std::ios::trunc);
    if (patchFile)
        return save_(patchFile);
    ERROR(Error::CANNOT_CREATE_PATCH)
}

bool Patch::save_(std::ostream &patchFile)
{
    patchFile << SIGNATURE << '\n';

    patchFile.setf(std::ios_base::uppercase);
    patchFile.setf(std::ios_base::hex , std::ios_base::basefield);
    patchFile << std::setfill('0');

    patchFile << std::setw(8) << crc32_ << '\n';

    unsigned int offsetLen = (unsigned int)ceil(log((long double)originalFileSize_)/log(16));
    if (offsetLen < 4)
        offsetLen = 4;

    for (auto it : data_)
    {
        patchFile << std::setw(offsetLen) << it.first;
        for (unsigned int b : it.second)
            patchFile << ' ' << std::setw(2) << b;
        patchFile << '\n';
        if (!patchFile)
            ERROR(Error::CANNOT_WRITE_PATCH)
    }
    return true;
}

bool Patch::parse(const tchar *patchFileName, const size_t maxOffset)
{
    std::ifstream patchFile;
    tstring s;

    for (int i = 0; i < ARRAY_SIZE(PATCH_FILE_EXTS); ++i)
    {
        s = patchFileName;
        s += PATCH_FILE_EXTS[i];
        patchFile.open(s, std::ios::in);
        if (patchFile)
            return parse_(patchFile, maxOffset);
    }

    ERROR(Error::CANNOT_OPEN_PATCH)
}

bool Patch::parse_(std::istream &patchFile, const size_t maxOffset)
{
    if (!patchFile.eof())
    {
        std::string signature;
        std::getline(patchFile, signature);
        if (signature != SIGNATURE)
            ERROR(Error::INVALID_SIGNATURE)
    }

    if (!patchFile.eof())
    {
        std::string crc32;
        std::getline(patchFile, crc32);
        if (crc32.empty())
            ERROR(Error::INVALID_CRC32)
        std::istringstream crc32Stream(crc32);
        crc32Stream.setf(std::ios_base::hex, std::ios_base::basefield);
        crc32Stream.setf(std::ios_base::skipws);
        crc32Stream >> crc32_;
        if (!crc32Stream)
            ERROR(Error::INVALID_CRC32)
    }

    data_.clear();
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

        lineStream.setf(std::ios_base::hex, std::ios_base::basefield);

        // offset
        size_t offset = 0;
        lineStream >> offset;
        if (!lineStream || (maxOffset && offset >= maxOffset))
            ERROR(Error::INVALID_OFFSET_VALUE)

        // bytes
        Bytes bytes;
        lineStream >> std::ws;
        while (!lineStream.eof())
        {
            unsigned int b = 0;
            lineStream >> b;
            if (!lineStream || b > 0xFF)
                ERROR(Error::INVALID_BYTE_VALUE)
            bytes.push_back(static_cast<unsigned char>(b));
            lineStream >> std::ws;
        }
        if (bytes.size() > 0)
            data_[offset] = bytes;
    }
    return true;
}

bool Patch::create(const tchar *oldFileName, const tchar *newFileName)
{
    std::ifstream oldFile(oldFileName, std::ios::in | std::ios::binary);
    if (oldFile)
    {
        std::ifstream newFile(newFileName, std::ios::in | std::ios::binary);
        if (newFile)
            return create_(oldFile, newFile);
        ERROR(Error::CANNOT_OPEN_NEWFILE)
    }
    ERROR(Error::CANNOT_OPEN_OLDFILE)
}

bool Patch::create_(std::istream &oldFile, std::istream &newFile)
{
    // check size
    oldFile.seekg(0, std::ios_base::end);
    const auto oldFileSize = oldFile.tellg();
    originalFileSize_ = static_cast<size_t>(oldFileSize);
    newFile.seekg(0, std::ios_base::end);
    const auto newFileSize = newFile.tellg();

    if (oldFileSize != newFileSize)
        ERROR(Error::DIFFERENT_SIZE)

    // init
    newFile.seekg(0);
    oldFile.seekg(0);

    bool isDiffState = false;
    Crc32 hash;
    Bytes bytes;
    size_t offset;
    data_.clear();

    // work
    while (oldFile && newFile)
    {
        unsigned int b1 = oldFile.get();  // TODO Optimize
        if (oldFile.eof())
            break;
        if (!oldFile)
            ERROR(Error::CANNOT_READ_OLDFILE)
            unsigned int b2 = newFile.get();
        if (!newFile)
            ERROR(Error::CANNOT_READ_NEWFILE)
            hash.compute(b1);
        if (b1 == b2)
        {
            if (isDiffState)
            {
                isDiffState = false;
                if (bytes.size() > 0)
                    data_[offset] = bytes;
            }
        }
        else
        {
            if (!isDiffState)
            {
                isDiffState = true;
                offset = static_cast<size_t>(newFile.tellg()) - 1;
                bytes.clear();
            }
            bytes.push_back(b2);
        }
    }

    if (isDiffState && bytes.size() > 0)
        data_[offset] = bytes;
    crc32_ = hash.value();

    return true;
}
