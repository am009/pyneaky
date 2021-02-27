#pragma once

char *ElevatedExecute(DWORD pid, LPCWSTR executable_path, LPWSTR command_line);
char *ElevatedExecute(LPCWSTR executable_path, LPWSTR command_line);
char *ElevateSelf();
char *ElevateSelf(DWORD pid);
