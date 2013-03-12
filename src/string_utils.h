/*
 *  Date:    2013/03/03
 *  Author:  Ruzzz ruzzzua[]gmail.com
 */

#pragma once

namespace StringUtils
{

inline bool isSpace(const char c)
{
#ifdef _WIN32
    return isspace(static_cast<unsigned char>(c)) != 0;
#endif
    return isspace(c) != 0;
}

inline bool isSpace(const wchar_t c)
{
    return iswspace(c) != 0;
}

template <typename TString>
void trimInPlace(TString &s)
{
    if (s.empty())
        return;
    auto last = s.cend(), first = s.cbegin();
    for (; first != last && isSpace(*first); ++first);
    if (last == first)
    {
        s.clear();
        return;
    }
    --last;
    for (; first != last && isSpace(*last); --last);
    s.assign(first, last + 1);
}

template <typename TString>
void upperCaseInPlace(TString &s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

};  // StringUtils
