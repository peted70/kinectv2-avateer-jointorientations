#include "pch.h"
#include "Utility.h"
#include <strsafe.h>

void Utility::Out(LPCTSTR sFormat, ...)
{
#ifndef NO_OUTPUT
	va_list argptr;
	va_start(argptr, sFormat);
	TCHAR buffer[2000];
	HRESULT hr = StringCbVPrintf(buffer, sizeof(buffer), sFormat, argptr);
	if (STRSAFE_E_INSUFFICIENT_BUFFER == hr || S_OK == hr)
		OutputDebugString(buffer);
	else
		OutputDebugString(L"StringCbVPrintf error.");
#endif
}