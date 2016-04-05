//
// Project: Simple Patch
// Date:    2013-03-08
// Author:  Ruslan Zaporojets <ruzzzua[]gmail.com>
//


#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>

#include "defs.h"
#include "Patch.h"
#include "Crc32.h"
#include "StringUtil.h"


namespace {  // Helpers


class DiffComputer
{
public:
    DiffComputer(Patch::DiffData *diffData) :
        offset_(0),
        foundOffset_(0),
        bytes_(),
        data_(diffData)
    {
        data_->clear();
    }

    ~DiffComputer()
    {
        checkBytes();
    }

    void compare(unsigned char b1, unsigned char b2)
    {
        if (b1 == b2)
            checkBytes();
        else
        {
            if (bytes_.empty())
                foundOffset_ = offset_;
            bytes_.push_back(b2);
        }
        ++offset_;
    }

    size_t offset_;

private:
    DiffComputer();
    DiffComputer(DiffComputer &);
    void operator=(DiffComputer &);

    void checkBytes()
    {
        if (!bytes_.empty())
        {
            Patch::Bytes b;
            b.swap(bytes_);
            (*data_)[foundOffset_] = b;
        }
    }

    size_t foundOffset_;
    Patch::Bytes bytes_;
    Patch::DiffData *data_;
};


class StreamParser
{
public:
    StreamParser(std::istream *patchFile) : file(patchFile), s(), ss()
    {
        ss.setf(std::ios::skipws);
        ss.exceptions(std::ios::badbit);
    }

    bool reset()
        /// if s is not empty then reinitialize ss
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
            StringUtil::trimInPlace(s);
            return reset();
        }
        else
            return false;
    }

    template <typename T>
    bool getKey(const char *key, T &value)
        /// Parse string 'KEY VALUE'. If VALUE is empty then value = 0.
        /// If key == KEY and VALUE is valid T or empty then return true.
    {
        if (updateLine())
        { 
            std::string k;
            ss >> k;
            StringUtil::upperCaseInPlace(k);
            if (k == key)
            {
                ss >> std::ws;
                if (ss.eof())
                    value = 0;
                else
                {
                    ss >> value;
                    if (ss.fail())
                        return false;
                }
                return true;
            }
        }
        return false;
    }

    std::istream *file;
    std::string s;
    std::istringstream ss;

private:
    StreamParser(StreamParser&);
    void operator=(StreamParser&);
};


}  // namespace for helpers


namespace Patch {


//
//  Error
//


const char *Patch::Error::toString() const
{
    switch (code_)
    {
        case OK:                   return "OK";

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
        case CANNOT_WRITE_TARGET:  return "Error writing to target file";

        default:                   return "Unknown error";
    }
}


bool operator==(const Error &lhs, const Error &rhs)
{
    return lhs.code() == rhs.code();
}


bool operator==(const Error &lhs, const Error::Code &rhs)
{
    return lhs.code() == rhs;
}


bool operator==(const Error::Code &lhs, const Error &rhs)
{
    return lhs == rhs.code();
}


//
//  Patcher
//


const char *Patcher::SIGNATURE = "SIMPLEDIFF";
const tchar *const Patcher::PATCH_FILE_EXTS[] = 
{
    _T(""),
    _T(".zsdiff"),
    _T(".simplediff")
};


bool Patcher::apply(const tchar *targetFileName)
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


bool Patcher::apply_(std::fstream &targetFile)
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
        if (!Crc32::calc(targetFile, currentCrc))
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
            targetFile.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
            if (targetFile)
                continue;
        }

        lastError_ = Error::CANNOT_WRITE_TARGET;
        return false;
    }
    return true;
}


bool Patcher::load(const tchar *patchFileName)
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

    lastError_ = Error::CANNOT_OPEN_PATCH;
    return false;
}


bool Patcher::parse_(std::istream &patchFile)
{
    StreamParser p(&patchFile);
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
            while (!p.file->eof() && !p.file->fail())

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
                    if (!bytes.empty())
                        diffData_[offset] = bytes;
                }
            }  // while

            if (diffData_.empty())
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


bool Patcher::compare(const tchar *oldFileName, const tchar *newFileName)
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


bool Patcher::compare_(std::istream &oldFile, std::istream &newFile)
{
    // check size
    oldFile.seekg(0, std::ios_base::end);
    const auto oldFileSize = oldFile.tellg();
    newFile.seekg(0, std::ios_base::end);
    const auto newFileSize = newFile.tellg();
    if (oldFileSize != newFileSize)
    {
        lastError_ = Error::DIFFERENT_SIZE;
        return false;
    }

    fileSize_ = static_cast<size_t>(oldFileSize);

    // init
    newFile.seekg(0);
    oldFile.seekg(0);
    std::ios::iostate prevStateOld = oldFile.exceptions();
    std::ios::iostate prevStateNew = newFile.exceptions();
    oldFile.exceptions(std::ios::badbit | std::ios::failbit);
    newFile.exceptions(std::ios::badbit | std::ios::failbit);
    lastError_ = Error::OK;

    // work
    try
    {
        Crc32 crc;
        DiffComputer c(&diffData_);

        std::istreambuf_iterator<char> oldIt(oldFile.rdbuf()),
                                       newIt(newFile.rdbuf()),
                                       end;

        for (;oldIt != end && newIt != end; ++oldIt, ++newIt)
        {
            crc.compute(*oldIt);
            c.compare(*oldIt, *newIt);
        }

        fileCrc32_ = crc.value();
    }
    catch (std::ios::failure /*&e*/)
    {
        if (!oldFile)
            lastError_ = Error::CANNOT_READ_OLDFILE;
        else
            lastError_ = Error::CANNOT_READ_NEWFILE;
    }

    oldFile.exceptions(prevStateOld);
    newFile.exceptions(prevStateNew);
    return lastError_ == Error::OK;
}


bool Patcher::save(const tchar *patchFileName)
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


bool Patcher::save_(std::ostream &patchFile)
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

    size_t max = fileSize_;
    if (!max)
        for (auto it : diffData_)
            if (it.first > max)
                max = it.first;
    unsigned int offsetLen = static_cast<unsigned int>(ceil(log(static_cast<long double>(max))/log(16)));
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


}  // namespace Patch
