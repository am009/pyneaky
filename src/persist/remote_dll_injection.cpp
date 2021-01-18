#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#define UNICODE
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <neaky/utils.h>

//找到 LoadLibrary 函数地址 (所有进程中都一样)
static void* LoadLibraryAddr = NULL;
static void* GetLoadLibraryAddr()
{
	if (LoadLibraryAddr == NULL)
	{
		HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");
		if (hKernel32 == NULL)
		{
			puts("### Unexpected Error in GetLoadLibraryAddr ###");
			ErrorExit(TEXT("GetModuleHandle"));
		}
		LPVOID llBaseAddress = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryW");
		if (llBaseAddress == NULL)
		{
			puts("### Unexpected Error in GetLoadLibraryAddr ###");
			ErrorExit(TEXT("GetProcAddress"));
		}
		printf("GetLoadLibraryAddr: LoadLibrary base address is: 0x%p\n", llBaseAddress);

		LoadLibraryAddr = llBaseAddress;
	}
	return LoadLibraryAddr;
}

BOOL InjectDll(DWORD pid, LPCWSTR dll_path)
{
	HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess == NULL)
	{
		printf("InjectDll: Couldn't open process.\n");
		return FALSE;
	}
	printf("InjectDll: Process handle: 0x%p\n", hProcess);
	
	//allocate memory in target process
	LPVOID lpBaseAddress = (LPVOID)VirtualAllocEx(hProcess, NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (lpBaseAddress == NULL)
	{
		printf("InjectDll: VirtualAllocEx failed.\n");
		return FALSE;
	}
	//printf("InjectDll: Allocated memory address in target process is: 0x%p\n", lpBaseAddress);

	//write DLL name to target process
	SIZE_T* lpNumberOfBytesWritten = 0;
	BOOL resWPM = WriteProcessMemory(hProcess, lpBaseAddress, dll_path, wcslen(dll_path) * 2 + 1, lpNumberOfBytesWritten);
	if (!resWPM)
	{
		printf("InjectDll: WriteProcessMemory failed.\n");
		return FALSE;
	}
	//printf("[+] DLL name is written to memory of target process\n");


	//start remote thread in target process
	HANDLE hThread = NULL;
	DWORD ThreadId = 0;
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetLoadLibraryAddr(), lpBaseAddress, 0, (LPDWORD)(&ThreadId));
	if (hThread == NULL)
	{
		printf("InjectDll: CreateRemoteThread failed.\n");
		return FALSE;
	}
	//printf("[+] Successfully started DLL in target process\n");
	printf("InjectDll: Injected thread id: %lu for pid: %lu\n", ThreadId, pid);
	return TRUE;
}