#pragma once

void ConsoleInit();
void ShowError();
void ShowError(DWORD err);
void ShowMessage(wchar_t *);
void ErrorExit(const wchar_t* message);
LPWSTR lstrcat_heap(LPCWSTR str1, LPCWSTR str2);
DWORD GetPIDByName(LPCWSTR name);
LPCSTR GetSysVersion();
int GetIntegrity();


