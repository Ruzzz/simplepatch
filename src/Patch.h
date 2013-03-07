#pragma once

#include <fstream>
#include <vector>
#include <map>

#include "defs.h"

typedef std::vector<unsigned char> Bytes;
typedef std::map<size_t, Bytes> PatchData;

class Patch
{
public:

    class Error
    {
    public:
        enum Code
        {
            OK,
            INVALID_SIGNATURE,
            INVALID_OFFSET_VALUE,
            INVALID_BYTE_VALUE,
            INVALID_CRC32,
            CANNOT_CREATE_PATCH,
            CANNOT_OPEN_PATCH,
            CANNOT_WRITE_PATCH,
            CANNOT_APPLY_PATCH,
            DIFFERENT_SIZE,
            CANNOT_OPEN_OLDFILE,
            CANNOT_READ_OLDFILE,
            CANNOT_OPEN_NEWFILE,
            CANNOT_READ_NEWFILE
        };

        Error(Code code = OK) : code_(code) {}
        const Error& operator=(const Error &other) { code_ = other.code_; return *this; }
        const Code code() const { return code_; }
        const char *toString() const;

    private:
        Code code_;
    };

    Patch() : originalFileSize_(0), crc32_(0) {}
    const Error &getLastError() const { return lastError_; }
    bool apply(std::fstream &targetFile);
    bool save(const tchar *patchFileName);
    bool parse(const tchar *patchFileName, const size_t maxOffset = 0);
    bool create(const tchar *oldFileName, const tchar *newFileName);
    
private:
    bool save_(std::ostream &patchFile);
    bool parse_(std::istream &patchFile, const size_t maxOffset = 0);
    bool create_(std::istream &oldFile, std::istream &newFile);

    PatchData data_;
    size_t originalFileSize_;
    unsigned int crc32_;
    Error lastError_;
};
