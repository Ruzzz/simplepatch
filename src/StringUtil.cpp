//
// Project: Substitute
// Date:    2013-03-28
// Author:  Ruslan Zaporojets <ruzzzua[]gmail.com>
//


#ifdef _WIN32
#include <windows.h>
#endif

#include "StringUtil.h"


#ifdef _WIN32
bool StringUtil::wideToAnsiArray(std::vector<char> &result, const wchar_t *s, int size /*= -1*/)
{
    if (s && size)
    {
        int needed = ::WideCharToMultiByte(CP_ACP, 0, s, size, NULL, 0, NULL, NULL);
        if (!(needed && SUCCEEDED(::GetLastError())))
            return false;
        result.resize(needed);
        needed = ::WideCharToMultiByte(CP_ACP, 0, s, size, &result[0], needed, NULL, NULL);
        if (!(needed && SUCCEEDED(::GetLastError())))
            return false;
        result.resize(result.size() - 1);  // remove last character '/0'
    }
    else
        result.clear();
    return true;
}
#endif
