//
// Project: Simple Patch
// Date:    2013-03-03
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#pragma once


#define VERSION "0.3"

#define MAIN_ABORT(msg) { std::cerr << msg << std::endl; return 1; }
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#ifdef GTEST
#define TESTING
#endif

#ifdef _UNICODE
    #define tstring std::wstring
  
    #ifndef _T
    #define _T(text) L##text
    #endif

    #ifndef tchar
    #define tchar wchar_t
    #endif

    #ifndef _tmain
    #define _tmain wmain
    #endif

    #ifndef _tcscmp
    #define _tcscmp wcscmp
    #endif
#else
    #define tstring std::string

    #ifndef _T
    #define _T(text) text
    #endif

    #ifndef tchar
    #define tchar char
    #endif

    #ifndef _tmain
    #define _tmain main
    #endif

    #ifndef _tcscmp
    #define _tcscmp strcmp
    #endif
#endif
