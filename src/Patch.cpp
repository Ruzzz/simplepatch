/*
 *  Date:    2013/03/03
 *  Author:  Ruzzz ruzzzua[]gmail.com
 */

#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>

#include "defs.h"
#include "Patch.h"
#include "Crc32.h"
#include "string_utils.h"

#ifdef _WIN32
typedef unsigned __int64 uint64_t;
#endif

//
//  Parser helper
//

class Parser
{
public:
    Parser(std::istream *patchFile) :
        file(patchFile), s(), ss()
    {
        ss.setf(std::ios::skipws);
        ss.exceptions(std::ios::badbit);
    }

    bool reset()
        /// if s is not empty then reinit ss
    {
        if (!s.empty())
        {
            ss.clear();
            ss.seekg(0, std::ios::beg);
            ss.str(s);
            return true;
        }
        else
            return false;
    }

    bool updateLine()
        /// If file operation is fail or string is empty then return false
    {
        std::getline(*file, s);
        if (!file->fail())
        {
            StringUtils::trimInPlace(s);
            return reset();
        }
        else
            return false;
    }

    template <typename T>
    bool getKey(const char *key, T &value)
        /// Parse string 'KEY VALUE'
    {
        if (updateLine())
        { 
            std::string s;
            ss >> s;
            StringUtils::upperCaseInPlace(s);
            if (s == key)
            {
                ss >> value;
                if (ss.fail())
                    value = 0;
                return true;
            }
        }
        return false;
    }

    std::istream *file;
    std::string s;
    std::istringstream ss;

private:
    Parser(Parser &);
    void operator=(Parser &);
};

//
//  Patch Error
//

const char *Patch::Error::toString() const
{
    switch (code_)
    {
        case OK:                   return "Ok";

        case EMPTY_PATCH:          return "Patch file is empty";
        case INVALID_SIGNATURE:    return "Invalid signature in patch file";
        case INVALID_SIZE:         return "Invalid size";
        case INVALID_CRC32:        return "Invalid crc32";
        case INVALID_OFFSET_VALUE: return "Invalid offset in patch file";
        case INVALID_BYTE_VALUE:   return "Invalid byte value in patch file";

        case CANNOT_CREATE_PATCH:  return "Cannot create patch file";
        case CANNOT_OPEN_PATCH:    return "Cannot open patch file";
        case CANNOT_WRITE_PATCH:   return "Error writing to patch file";
        case CANNOT_READ_PATCH:    return "Error reading patch file";

        case DIFFERENT_SIZE:       return "Files have different size";
        case CANNOT_OPEN_OLDFILE:  return "Cannot open old file";
        case CANNOT_READ_OLDFILE:  return "Error reading old file";
        case CANNOT_OPEN_NEWFILE:  return "Cannot open new file";
        case CANNOT_READ_NEWFILE:  return "Error reading new file";

        case EMPTY_TARGET:         return "Target file is empty";
        case CANNOT_OPEN_TARGET:   return "Cannot open target file";
        case CANNOT_READ_TARGET:   return "Error reading target file";
        case CANNOT_WRITE_TARGET:   return "Error writing to target file";

        default:                   return "Unknown error";
    }
}

//
//  Patch
//

const char *Patch::SIGNATURE = "SIMPLEDIFF";
const tchar *const Patch::PATCH_FILE_EXTS[] = 
{
    _T(".sdiff"),
    _T(".simplediff"),
    _T("")
};

bool Patch::calcCrc32(std::istream &f, unsigned int &result)
{
    f.clear();
    f.seekg(0);

    Crc32 crc;
    const unsigned int BUFFER_LEN = 1024 * 64;
    std::vector<char> buffer(BUFFER_LEN);
    while (!f.eof())
    {
        f.read((char *)buffer.data(), buffer.size());
        unsigned int readed = (unsigned int)f.gcount();
        if (readed)
        {
            if (f.bad())
                return false;
            else
                crc.compute((void *)buffer.data(), readed);
        }
    }
    result = crc.value();
    return true;
}

bool Patch::apply(const tchar *targetFileName)
{
    std::fstream targetFile(targetFileName, std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);
    if (!targetFile)
    {
        lastError_ = Error::CANNOT_OPEN_TARGET;
        return false;
    }
    else
        return apply_(targetFile);
}

bool Patch::apply_(std::fstream &targetFile)
{
    // size
    const auto size = targetFile.tellg();
    if (!size)
    {
        lastError_ = Error::EMPTY_TARGET;
        return false;
    }
    if (fileSize_ && fileSize_ != size)
    {
        lastError_ = Error::INVALID_SIZE;
        return false;
    }

    // crc
    if (fileCrc32_)
    {
        unsigned int currentCrc;
        if (!calcCrc32(targetFile, currentCrc))
        {
            lastError_ = Error::CANNOT_READ_TARGET;
            return false;
        }
        if (fileCrc32_ != currentCrc)
        {
            lastError_ = Error::INVALID_CRC32;
            return false;
        }
    }

    // apply
    targetFile.clear();
    for (auto it : diffData_)
    {
        targetFile.seekg(it.first);
        if (targetFile)
        {
            Bytes bytes = it.second;
            targetFile.write((const char *)bytes.data(), bytes.size());
            if (targetFile)
                continue;
        }

        lastError_ = Error::CANNOT_WRITE_TARGET;
        return false;
    }
    return true;
}

bool Patch::load(const tchar *patchFileName)
{
    std::ifstream patchFile;
    tstring s;

    for (int i = 0; i < ARRAY_SIZE(PATCH_FILE_EXTS); ++i)
    {
        s = patchFileName;
        s += PATCH_FILE_EXTS[i];
        patchFile.open(s, std::ios::in);
        if (patchFile)
            return parse_(patchFile);
    }

    lastError_ = Error(Error::CANNOT_OPEN_PATCH);
    return false;
}

bool Patch::parse_(std::istream &patchFile)
{
    Parser p(&patchFile);
    p.file->exceptions(std::ios::badbit);
    try
    {
        // signature
        if (!(p.updateLine() && p.s == SIGNATURE))
        {
            lastError_ = Error::INVALID_SIGNATURE;
            return false;
        }
        else
        {
            std::string s;

            // size
            p.ss.setf(std::ios::dec, std::ios::basefield);
            if (!p.getKey("SIZE", fileSize_))
            {
                lastError_ = Error::INVALID_SIZE;
                return false;
            }

            // crc
            p.ss.setf(std::ios::hex, std::ios::basefield);
            if (!p.getKey("CRC", fileCrc32_))
            {
                lastError_ = Error::INVALID_CRC32;
                return false;
            }

            // work
            diffData_.clear();
            while (!p.file->eof())
            {
                if (!p.updateLine())
                    continue;  // skip empty strings

                // offset
                size_t offset;
                p.ss >> offset;
                if (p.ss.fail() || (fileSize_ && offset >= fileSize_))
                {
                    lastError_ = Error::INVALID_OFFSET_VALUE;
                    return false;
                }
                else
                {
                    // bytes
                    Bytes bytes;
                    while (!p.ss.eof())
                    {
                        unsigned int b = 0;
                        p.ss >> b;
                        if (p.ss.fail())
                        {
                            if (!p.ss.eof())
                            {
                                lastError_ = Error::INVALID_BYTE_VALUE;
                                return false;
                            }
                            else
                                break;  // skip last spaces
                        }
                        if (b > 0xFF)
                        {
                            lastError_ = Error::INVALID_BYTE_VALUE;
                            return false;
                        }
                        bytes.push_back(static_cast<unsigned char>(b));
                    }
                    if (bytes.size() > 0)
                        diffData_[offset] = bytes;
                }
            }  // while

            if (diffData_.size() == 0)
            {
                lastError_ = Error::EMPTY_PATCH;
                return false;
            }
        }
    }
    catch (std::ios::failure /*&e*/)
    {
        lastError_ = Error::CANNOT_READ_PATCH;
        return false;
    }

    lastError_ = Error::OK;
    return true;
}

bool Patch::compare(const tchar *oldFileName, const tchar *newFileName)
{
    std::ifstream oldFile(oldFileName, std::ios::in | std::ios::binary);
    if (oldFile)
    {
        std::ifstream newFile(newFileName, std::ios::in | std::ios::binary);
        if (newFile)
            return compare_(oldFile, newFile);
        else
            lastError_ = Error::CANNOT_OPEN_NEWFILE;
    }
    else
        lastError_ = Error::CANNOT_OPEN_OLDFILE;
    return false;
}

bool Patch::compare_(std::istream &oldFile, std::istream &newFile)
{
    // check size
    oldFile.seekg(0, std::ios_base::end);
    const auto oldFileSize = oldFile.tellg();
    newFile.seekg(0, std::ios_base::end);
    const auto newFileSize = newFile.tellg();
    if (oldFileSize != newFileSize)
    {
        lastError_ = Error(Error::DIFFERENT_SIZE);
        return false;
    }

    fileSize_ = static_cast<size_t>(oldFileSize);

    // init
    newFile.seekg(0);
    oldFile.seekg(0);

    bool isDiffState = false;
    Crc32 crc;
    Bytes bytes;
    size_t offset;
    diffData_.clear();

    // work
    while (oldFile && newFile)
    {
        unsigned int b1 = oldFile.get();  // TODO Optimize
        if (oldFile.eof())
            break;
        if (!oldFile)
        {
            lastError_ = Error::CANNOT_READ_OLDFILE;
            return false;
        }

        unsigned int b2 = newFile.get();
        if (!newFile)
        {
            lastError_ = Error::CANNOT_READ_NEWFILE;
            return false;
        }

        crc.compute(b1);
        if (b1 == b2)
        {
            if (isDiffState)
            {
                isDiffState = false;
                if (bytes.size() > 0)
                    diffData_[offset] = bytes;
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
        diffData_[offset] = bytes;
    fileCrc32_ = crc.value();

    lastError_ = Error::OK;
    return true;
}

bool Patch::save(const tchar *patchFileName)
{
    std::ofstream patchFile(patchFileName, std::ios::out | std::ios::trunc);
    if (patchFile)
        return save_(patchFile);
    else
    {
        lastError_ = Error::CANNOT_CREATE_PATCH;
        return false;
    }
}

bool Patch::save_(std::ostream &patchFile)
{
    lastError_ = Error::CANNOT_WRITE_PATCH;

    // signature
    patchFile << SIGNATURE << '\n';
    if (!patchFile)
        return false;

    // size
    patchFile << "SIZE ";
    patchFile.setf(std::ios::dec, std::ios::basefield);
    patchFile << fileSize_ << '\n';
    if (!patchFile)
        return false;

    // crc32
    patchFile << "CRC ";
    patchFile.setf(std::ios::hex, std::ios::basefield);
    patchFile.setf(std::ios::uppercase);
    patchFile.fill('0');
    patchFile.width(8);
    patchFile << fileCrc32_ << '\n';
    if (!patchFile)
        return false;

    unsigned int offsetLen = (unsigned int)ceil(log((long double)fileSize_)/log(16));
    offsetLen += offsetLen % 4;

    for (auto it : diffData_)
    {
        patchFile.width(offsetLen);
        patchFile << it.first;
        for (unsigned int b : it.second)
        {
            patchFile << ' ';
            patchFile.width(2);
            patchFile << b;
        }
        patchFile << '\n';
        if (!patchFile)
            return false;
    }

    lastError_ = Error::OK;
    return true;
}
