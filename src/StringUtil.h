//
// Project: Simple Patch
// Date:    2013-03-03
// Author:  Ruzzz <ruzzzua[]gmail.com>
//

#pragma once

class StringUtil
{
public:
    static bool isSpace(const char c)
    {
#ifdef _WIN32
        return isspace(static_cast<unsigned char>(c)) != 0;
#endif
        return isspace(c) != 0;
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

private:
    StringUtil();
    StringUtil(StringUtil&);
    void operator=(StringUtil&);
};
