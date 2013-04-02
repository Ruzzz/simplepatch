//
// Project: Substitute
// Date:    2013-03-28
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#pragma once


#include <string>

#include "defs.h"
#include "StringUtil.h"


namespace SPatch {


class Error
{
public:
    friend class Options;

    enum Code
    {
        OK,

        WRONG_ARGUMENTS_NUMBER,
        INVALID_ARGUMENT
    };

    Error(Code code = OK) : code_(code) {}
    Error(const Error &other) : code_(other.code_) {}
    const Error& operator=(const Error &other) { code_ = other.code_; return *this; }
    const Error& operator=(const Code &code) { code_ = code; return *this; }

    const Code code() const { return code_; }
    const char *toString() const;

private:

    void setInvalidArgument(const tchar *invalidArgument)
    {
        code_ = INVALID_ARGUMENT;

        invalidArgumentError_.assign("Invalid option");
        std::string argument;
        if (StringUtil::prepareAnsi(invalidArgument, argument))
        {
            invalidArgumentError_ += ": ";
            invalidArgumentError_ += argument;
        }
    }

    Code code_;
    std::string invalidArgumentError_;
};


bool operator==(const Error &lhs, const Error &rhs);
bool operator==(const Error &lhs, const Error::Code &rhs);
bool operator==(const Error::Code &lhs, const Error &rhs);


class Options
{
public:
    Options(const int argc, const tchar *argv[]) :
        targetFileName_(nullptr),
        patchFileName_(nullptr),
        noBackup_(false),
        rewrite_(false)
    {
        parse_(argc, argv);
    }

    bool isValid() const { return lastError_ == Error::OK; }
    const Error &getLastError() const { return lastError_; }

    tchar *targetFileName()  const { return targetFileName_; };
    tchar *patchFileName() const { return patchFileName_; };
    bool noBackup()    const { return noBackup_; };
    bool rewrite()     const { return rewrite_; };

private:
    Options();

    void parse_(const int argc, const tchar *argv[])
        // If all options is valid and valid count of arguments then return true
    {
        int i = 1;
        while (i < argc && '-' == *argv[i])
        {
            if (_tcscmp(argv[i], _T("-rewrite")) == 0)
                rewrite_ = true;
            else if (_tcscmp(argv[i], _T("-nobackup")) == 0)
                noBackup_ = true;
            else
            {
                lastError_.setInvalidArgument(argv[i]);
                return;
            }
            ++i;
        }

        if (argc != i + 2)  // target-file patch-file
        {
            lastError_ = Error::WRONG_ARGUMENTS_NUMBER;
            return;
        }

        targetFileName_ = const_cast<tchar *>(argv[i]);
        patchFileName_ = const_cast<tchar *>(argv[i + 1]);
    }

    Error lastError_;

    tchar *targetFileName_;
    tchar *patchFileName_;
    bool noBackup_;
    bool rewrite_;
};


}  // namespace SPatch
