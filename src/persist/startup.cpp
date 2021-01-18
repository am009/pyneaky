#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#define UNICODE
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <neaky/utils.h>

// 向HKEY_CURRENT_USER\Software\\Microsoft\\Windows\\CurrentVersion\\Run里加入自运行项目
BOOL SetStratupReg(const WCHAR * key, const WCHAR * path)
{
	//根键、子键名称和到子键的句柄
	HKEY hRoot = HKEY_CURRENT_USER;
	HKEY hKey;//打开指定子键
	DWORD dwDisposition = REG_OPENED_EXISTING_KEY;
	//如果不存在就创建
	LONG lRet = RegCreateKeyEx(
		hRoot,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisposition
	);
	if (lRet != ERROR_SUCCESS)
	{
		puts("SetStratupReg: open registry failed");
		ShowError(lRet);
		return FALSE;
	}
	
	//创建一个新的键值，设置键值数据为文件
	lRet = RegSetValueEx(
		hKey,
		key,
		0,
		REG_SZ,
		(BYTE*)path,
		wcslen(path) * 2
	);
	if (lRet != ERROR_SUCCESS)
	{
		puts("SetStratupReg: reg set value failed");
		ShowError(lRet);
		return FALSE;
	}
	puts("SetStratupReg: reg set value success");
	//关闭子键句柄
	RegCloseKey(hKey);
	return TRUE;
}

// 删除自启动注册表项
BOOL DeleteStratupReg(const WCHAR * key)
{
	//根键、子键名称和到子键的句柄
	HKEY hRoot = HKEY_CURRENT_USER;
	HKEY hKey;//打开指定子键
	long lReturn = RegOpenKeyEx(hRoot,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		NULL,
		KEY_ALL_ACCESS,
		&hKey);
	if (lReturn != ERROR_SUCCESS)
	{
		puts("DeleteStratupReg: open registry failed");
		ShowError(lReturn);
		return FALSE;
	}

	lReturn = RegDeleteValue(
		hKey,
		key);
	if (lReturn != ERROR_SUCCESS)
	{
		puts("DeleteStratupReg: reg set value failed");
		ShowError(lReturn);
		return FALSE;
	}
	puts("DeleteStratupReg: reg set value success");
	//关闭子键句柄
	RegCloseKey(hKey);
	return TRUE;
}