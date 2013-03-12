/*
 *  Date:    2013/03/03
 *  Author:  Ruzzz ruzzzua[]gmail.com
 */

#pragma once

#include <fstream>
#include <vector>
#include <map>

#include "defs.h"

class Patch
{
public:

    typedef std::vector<unsigned char> Bytes;
    typedef std::map<size_t, Bytes> DiffData;

    class Error
    {
    public:
        enum Code
        {
            OK,

            EMPTY_PATCH,
            INVALID_SIGNATURE,
            INVALID_SIZE,
            INVALID_CRC32,
            INVALID_OFFSET_VALUE,
            INVALID_BYTE_VALUE,

            CANNOT_CREATE_PATCH,
            CANNOT_OPEN_PATCH,
            CANNOT_WRITE_PATCH,            
            CANNOT_READ_PATCH,

            DIFFERENT_SIZE,
            CANNOT_OPEN_OLDFILE,
            CANNOT_READ_OLDFILE,
            CANNOT_OPEN_NEWFILE,
            CANNOT_READ_NEWFILE,

            EMPTY_TARGET,
            CANNOT_OPEN_TARGET,
            CANNOT_READ_TARGET,
            CANNOT_WRITE_TARGET
        };

        Error(Code code = OK) : code_(code) {}
        Error(const Error &other) : code_(other.code_) {}
        const Error& operator=(const Error &other) { code_ = other.code_; return *this; }
        const Error& operator=(const Code &code) { code_ = code; return *this; }
        const Code code() const { return code_; }
        const char *toString() const;

    private:
        Code code_;
    };

    Patch() : fileSize_(0), fileCrc32_(0) {}
    const Error &getLastError() const { return lastError_; }
    bool apply(const tchar *targetFileName);
    bool save(const tchar *patchFileName);
    bool load(const tchar *patchFileName);
    bool compare(const tchar *oldFileName, const tchar *newFileName);
    
private:
    static bool calcCrc32(std::istream &f, unsigned int &result);
    
    bool apply_(std::fstream &targetFile);
    bool save_(std::ostream &patchFile);
    bool parse_(std::istream &patchFile);
    bool compare_(std::istream &oldFile, std::istream &newFile);

    static const char *SIGNATURE;
    static const tchar *const PATCH_FILE_EXTS[];

    DiffData diffData_;
    size_t fileSize_;
    unsigned int fileCrc32_;
    Error lastError_;
};
