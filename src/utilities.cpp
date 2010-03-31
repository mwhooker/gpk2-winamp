
const unsigned long MAX_STRING = 1024;
//const wchar_t *DebugHeader = L"[libgerpok]";
const bool gSpewToFile = false;

const bool gErrorSpewToFile = false;

#include <stdio.h>
#include <stdafx.h>
#include <utilities.h>
#define UNICODE

// ***************************************************************************
// Function:    DebugSpew
// Description: Outputs text to debugger, usage is same as printf ;)
// ***************************************************************************
VOID DebugSpew(const wchar_t *szFormat, ...)
{
#ifdef DBG_SPEW
    wchar_t szOutput[MAX_STRING] = {0};
    va_list vaList;
    va_start( vaList, szFormat );
    wvsprintfW(szOutput,szFormat, vaList);
    DebugSpewAlways(szOutput);
#endif
}



VOID DebugSpewAlways(const wchar_t *szFormat, ...)
{
    wchar_t szOutput[MAX_STRING] = {0};
    va_list vaList;

   // OutputDebugString(DebugHeader);
    va_start( vaList, szFormat );
    wvsprintfW(szOutput,szFormat, vaList);
    OutputDebugStringW(szOutput);
    OutputDebugStringW(L"\n");
}
#undef UNICODE


