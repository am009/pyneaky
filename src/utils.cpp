#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#define UNICODE
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <tlhelp32.h>
#include <versionhelpers.h>
#include <Securitybaseapi.h >

int GetIntegrity()
{
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hToken;
	DWORD dwLengthNeeded;
	DWORD dwError = ERROR_SUCCESS;
	PTOKEN_MANDATORY_LABEL pTIL = NULL;
	if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hToken))
	{
		return -1;
	}
	if (GetTokenInformation(hToken, TokenIntegrityLevel,
		NULL, 0, &dwLengthNeeded))
	{
		return -1;
	}
	dwError = GetLastError();
	if (dwError != ERROR_INSUFFICIENT_BUFFER)
	{
		return -1;
	}
	pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0,
		dwLengthNeeded);
	if (pTIL == NULL)
	{
		return -1;
	}

	if (GetTokenInformation(hToken, TokenIntegrityLevel,
		pTIL, dwLengthNeeded, &dwLengthNeeded))
	{
		DWORD dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid,
			(DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1));

		if (dwIntegrityLevel >= SECURITY_MANDATORY_SYSTEM_RID)
		{
			return 4;
		}
		else if (dwIntegrityLevel >= SECURITY_MANDATORY_HIGH_RID)
		{
			return 3;
		}
		else if (dwIntegrityLevel >= SECURITY_MANDATORY_MEDIUM_RID)
		{
			return 2;
		}
		else if (dwIntegrityLevel >= SECURITY_MANDATORY_LOW_RID)
		{
			return 1;
		}
		return 0;
	}
	return -1;
}

// 不区分大小写，获取进程PID
DWORD GetPIDByName(LPCWSTR name)
{
	DWORD pid = NULL;
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe;
	THREADENTRY32 te;

	pe.dwSize = sizeof(pe);
	te.dwSize = sizeof(te);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	Process32First(hSnapshot, &pe);
	do {
		if (_wcsicmp(name, pe.szExeFile) == 0) {
			pid = pe.th32ProcessID;
			//if (Thread32First(hSnapshot, &te)) {
			//	do {
			//		if (te.th32OwnerProcessID == pid) {
			//			tids.push_back(te.th32ThreadID);
			//		}
			//	} while (Thread32Next(hSnapshot, &te));
			//}
			break;
		}

	} while (Process32Next(hSnapshot, &pe));
	CloseHandle(hSnapshot);

	return pid;

}

LPCSTR GetSysVersion()
{
	if (IsWindowsServer())
	{
		return "Windows Server";
	}
	else if (IsWindows10OrGreater())
	{
		return "Windows 10";
	}
	else if (IsWindows8OrGreater())
	{
		return "Windows 8";
	}
	else if (IsWindows7OrGreater())
	{
		return "Windows 7";
	}
	else if (IsWindowsVistaOrGreater())
	{
		return "Windows vista";
	}
	else if (IsWindowsXPOrGreater())
	{
		return "Windows XP";
	}
	return "Unknown";
}

LPWSTR lstrcat_heap(LPCWSTR str1, LPCWSTR str2)
{
	size_t len1 = (size_t)lstrlen(str1) * 2;
	size_t len2 = (size_t)lstrlen(str2) * 2;
	LPWSTR dest = (LPWSTR)malloc(len1 + len2 + 2);
	if (dest == NULL)
	{
		return NULL;
	}
	lstrcpy(dest, str1);
	lstrcat(dest, str2);
	return dest;
}

void ConsoleInit()
{
	if (AllocConsole())
	{
		FILE* pCout;
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		SetConsoleTitle(L"Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	}
}

void ShowError(DWORD err)
{
	// 翻译 ErrorCode
	LPSTR Error = NULL;
	if (::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		0,
		(LPSTR)&Error,
		0,
		NULL) == 0)
	{
		printf("FormatMessageA failed. Error code: %lu", err);
		return;
	}

	// Display message.
	//MessageBoxA(NULL, Error, L"Error", MB_OK | MB_ICONWARNING);
	printf("LastError is %s", Error);

	// Free the buffer.
	if (Error)
	{
		::LocalFree(Error);
		Error = 0;
	}
}

void ShowError()
{
	// Get last error.
	DWORD err = GetLastError();
	ShowError(err);
}

void ErrorExit(const wchar_t* message)
{
	MessageBox(NULL, message, L"Error", MB_OK | MB_ICONWARNING);
	exit(0);
}

void ShowMessage(wchar_t* message)
{
	MessageBox(NULL, message, L"Error", MB_OK | MB_ICONWARNING);
}