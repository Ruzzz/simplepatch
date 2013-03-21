#pragma once

#define VERSION "0.3"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#ifndef _T
#ifdef _UNICODE
#define _T(text) L##text
#else
#define _T(text) text
#endif
#endif

#ifndef tchar
#ifdef _UNICODE
#define tchar wchar_t
#else
#define tchar char
#endif
#endif

#ifndef _tmain
#ifdef _UNICODE
#define _tmain wmain
#else
#define _tmain main
#endif
#endif

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif
