#pragma once

BOOL GetSystem(LPCWSTR self_cmd);
BOOL ElevatedExecute(DWORD pid, LPCWSTR executable_path, LPWSTR command_line);
BOOL ElevatedExecute(LPCWSTR executable_path, LPWSTR command_line);
BOOL ElevateSelf();
BOOL ElevateSelf(DWORD pid);
std::string get_username();
