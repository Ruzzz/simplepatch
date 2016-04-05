//
// Project: Simple Patch
// Date:    2013-03-03
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#pragma once


#include <vector>

#include "defs.h"


class StringUtil
{
public:
    static bool isSpace(const char c)
    {
#ifdef _WIN32
        return isspace(static_cast<unsigned char>(c)) != 0;
#else
        return isspace(c) != 0;
#endif
    }

    static bool isSpace(const wchar_t c)
    {
        return iswspace(c) != 0;
    }

    template <typename TString>
    static void trimInPlace(TString &s)
    {
        if (!s.empty())
        {
            auto first = s.cbegin(), last = s.cend();
            for (; first != last && isSpace(*first); ++first)
                ;
            if (first == last)
            {
                s.clear();
                return;
            }
            --last;
            for (; first != last && isSpace(*last); --last)
                ;
            s.assign(first, last + 1);
        }
    }

    template <typename TString>
    static void upperCaseInPlace(TString &s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    }

#ifdef _WIN32
    static bool wideToAnsiArray(std::vector<char> &result, const wchar_t *s, int size = -1);
    /// Convert from wide string to vector of ANSI character, without '/0'
#endif

    static bool prepareAnsi(const tchar *s, std::string &result)
    {
#ifdef _UNICODE
        std::vector<char> buffer;
        if (StringUtil::wideToAnsiArray(buffer, s))
        {
            result.assign(buffer.begin(), buffer.end());
            return true;
        }
        else
            return false;
#else
        result.assign(s);
        return true;
#endif
    }

private:
    StringUtil();
    StringUtil(StringUtil&);
    void operator=(StringUtil&);
};
