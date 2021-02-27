# Neaky

Make python a Windows spyware.

features:
- clipboard - get clipboard content.
- screenshot - save screenshot to path
- remote_dll_injection, get_pid_by_name, get_pids_by_name 
- set_startup_reg, delete_startup_reg - add startup registry key.
- keylog_stdout, keylog_to_file, keylog_stop - keylogging by rawinput
- elevate_self, elevated_execute - elevate from admin to system
- bypass_uac_exec - elevate from uac restricted token to full token

## example
see test/

## keylogger
keylogger requires a message loop on main thread, so when finishing initiliaze, it's required to call `neaky.message_loop()` to start keylogging, which normally will not return and cannot be terminated by Exceptions like Ctrl-C. you have to do other stuffs by creating another thread. See `test/keylog_raw_file` as an example.
when stopping raw input keylogger, message loop will return. Which will result in main thread exiting if there is not code after message_loop in main thread. This is because raw input keylogger creates a invisible window, when the keylogger stops, it destroys the window.

## install
```
pip install neaky
```

## build

(on Windows)

```cmd
python ./setup.py build
```

add module to pythonpath
```python
import sys
sys.path.append(r"C:\Users\warren\d\pyNeaky\pyneaky\build\lib.win-amd64-3.8")
```

## 目标

原本该模块是作为一个c语言dll存在，通过判断被植入的exe，单个dll实现各种不同功能，包括注入到任务管理器实现进程隐藏，启动时自动提权到能提权的最高程度等等，通过rundll32.exe启动从而勉强算是有微软签名的程序。这些都是作为dll的优点，然而如果作为python的拓展，则必须依附于python.exe。因此这里只提供部分功能，上述功能的实现考虑通过编写另外的dll，利用本拓展提供的dll注入功能注入。

- 一个独立的hook进程信息的dll - taskmgr-hook

- 一个提权小dll，用python CFFI 调用, 用于先提权后执行命令, 方便双重提权，控制台控制是bypass uac，system还是组合。


## stage2 todo

- 异常处理
- 注释
- message loop 新线程，释放GIL的控制启动和停止。
- get pid by name 返回list
- keylog 事件驱动调用python的函数
- 整理输出语句
- 输出全部换成wchar，wprintf

## notes - 笔记

[Coding Patterns for Python Extensions](https://pythonextensionpatterns.readthedocs.io/en/latest/index.html) 这本书不错

- 如何为函数增加参数相关的注释？

    https://stackoverflow.com/questions/38818400/specifying-python-function-signature-in-c-api
    https://stackoverflow.com/questions/1104823/python-c-extension-method-signatures-for-documentation/41245451#41245451
    [例子](https://github.com/MSeifert04/iteration_utilities/blob/master/src/iteration_utilities/_iteration_utilities/docsfunctions.h)
    而模块的注释：

- 操作list

    https://stackoverflow.com/questions/50668981/how-to-return-a-list-of-ints-in-python-c-api-extension-with-pylist

- 异常处理

    ```python
    PyErr_SetString(PyExc_RuntimeError, "Can not create default value for " #name);
    ```
    或者通过`PyErr_NewException`创建Exception子类作为第一个参数。[例子](https://docs.python.org/3/extending/extending.html#intermezzo-errors-and-exceptions) 

- PyArg_ParseTuple相关处理

    1. str转wchar

       由于u参数是*Deprecated*，首先利用`PyArg_ParseTuple`的U参数获取字符串，`PyUnicode_AsWideCharString`再转成wchar，最后 [`PyMem_Free()`](https://docs.python.org/3/c-api/memory.html#c.PyMem_Free) 。

       ```c
           PyObject *
           neaky_screenshot(PyObject *self, PyObject *args)
           {
               PyObject *path_obj;
               if (!PyArg_ParseTuple(args, "U", &path_obj))
               {
                   return NULL;
               }
               const Py_UNICODE *path = PyUnicode_AsWideCharString(path_obj, NULL);
               ScreenShotSave(path); // do something
               PyMem_Free(path);
               Py_RETURN_NONE;
           }
       ```

       

    2. path相关

       放弃了，还是和1一样，当普通的字符转wchar吧。文档虽然推荐使用PyUnicode_FSConverter函数，但是转换出来的bytes不是wchar这种utf-16类型的，估计还需要一次MultiByteToWideChar转换。这个函数本意是更robust的转义？

       python文档推荐使用O&的转换器函数方式

       ```
       PyObject *path = NULL;
       PyArg_ParseTuple(args, "O&", PyUnicode_FSConverter, &path);
       const char * path_ptr = PyBytes_AsString(path); // internal buffer
       ```

       普通的方式：

       ```cpp
       PyObject *filename_obj = Py_None, *filename_bytes;
       if (!PyArg_ParseTuple(args, "i|O:append_history_file", &nelements, &filename_obj))
               return NULL;
       if (!PyUnicode_FSConverter(filename_obj, &filename_bytes))
                   return NULL;
       ```

       

- 字符串相关

  在setup.py中定义使用wide的windows api。`define_macros=[("UNICODE", None)]`

  内部，get_system部分的错误处理目前还是使用char*，直接调用PyErr_SetString。

  但是似乎和python解释器协作起来，如果还是使用wide的print，在控制台就字符变宽两倍，vscode终端内正常。



- 流程
    1. 增加函数实现
    2. 增加文档，注册到module
