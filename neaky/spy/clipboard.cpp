#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "..\neakymodule.h"

#include <neaky/utils.h>

WCHAR* GetClipboardText()
{
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		PyErr_SetString(NeakyError, "Not text in clipboard !");
		return NULL;
	}
	// Try opening the clipboard
	if (!OpenClipboard(nullptr))
	{
		PyErr_SetString(NeakyError, "OpenClipboard failed !");
		return NULL;
	}

	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr)
	{
		PyErr_SetString(NeakyError, "GetClipboardData failed !");
		return NULL;
	}

	// Lock the handle to get the actual text pointer
	WCHAR* pszText = static_cast<wchar_t*>(GlobalLock(hData));
	if (pszText == nullptr)
	{
		PyErr_SetString(NeakyError, "GlobalLock failed !");
		return NULL;
	}

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	return pszText;
}
