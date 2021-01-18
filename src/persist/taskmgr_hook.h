#pragma once

extern LPCWSTR HookName;
void SetHook();
extern HANDLE ScanningThread;
BOOL StartScanningThread();
BOOL StopScanningThread();
void InjectDebounce();
void ScanAndInject();


