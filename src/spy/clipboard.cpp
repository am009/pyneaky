#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#define UNICODE
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <neaky/utils.h>

// 全局变量:

WCHAR* GetClipboardText()
{
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		puts("Not text in clipboard !");
		return NULL;
	}
	// Try opening the clipboard
	if (!OpenClipboard(nullptr))
	{
		puts("GetClipboardText: OpenClipboard failed !");
		ShowError();
	}

	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr) 
	{
		puts("GetClipboardText: GetClipboardData failed !");
		ShowError();
	}

	// Lock the handle to get the actual text pointer
	WCHAR* pszText = static_cast<wchar_t*>(GlobalLock(hData));
	if (pszText == nullptr)
	{
		puts("GetClipboardText: GlobalLock failed !");
		ShowError();
		return NULL;
	}

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	return pszText;
}

const WCHAR* GetClipboardTextHelper()
{
	WCHAR* tmp = GetClipboardText();
	if (tmp == NULL)
	{
		return L"";
	}
	return tmp;
}
