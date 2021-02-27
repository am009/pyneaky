# Neaky

windows spy module. 

keylogging requires message loop.
some function requires using `pythonw.exe` (`.pyw` extension) - todo


- clipboard -------- ok
- system - simply execute command ---------- ok
- screenshot - save screenshot to path ------- ok
- remote_dll_injection, get_pid_by_name ---------- ok
- startup - add startup registry key ------- ok
- keylog_file - start keylog to file ------ ok
- keylog_stdout - ok
- get_system - elevate from admin to system - todo
- uac_bypass - elevate to admin - todo

## example
see test/

## keylogger
keylogger requires a message loop on main thread, so when finishing initiliaze, it's required to call `neaky.message_loop()` to start keylogging, which normally will not return. you have to do other stuffs by creating another thread. See `test/keylog_raw_file` as an example.
when stopping raw input keylogger, message loop will return. This is because raw input keylogger creates a invisible window, when the keylogger stops, it destroys the window.

## build

(on Windows)

```cmd
python ./setup.py build
```

## 目标

原本该模块是作为一个c语言dll存在，通过判断被植入的exe，单个dll实现各种不同功能，包括注入到任务管理器实现进程隐藏，启动时自动提权到能提权的最高程度等等，通过rundll32.exe启动从而勉强算是有微软签名的程序。这些都是作为dll的优点，然而如果作为python的拓展，则必须依附于python.exe。因此这里只提供部分功能，上述功能的实现考虑通过编写自己的dll，利用本拓展提供的dll注入功能注入。

- 一个独立的hook进程信息的dll - taskmgr-hook

- 一个提权小dll，用python CFFI 调用, 用于先提权后执行命令, 方便双重提权，控制台控制是bypass uac，system还是组合。


## stage2 todo

- 异常处理
- 注释
- message loop 新线程，释放GIL的控制启动和停止。
- get pid by name 返回list
- keylog 事件驱动调用python的函数
- 整理输出语句

## 笔记

- 如何为函数增加参数相关的注释？

    https://stackoverflow.com/questions/38818400/specifying-python-function-signature-in-c-api
    https://stackoverflow.com/questions/1104823/python-c-extension-method-signatures-for-documentation/41245451#41245451
    [例子](https://github.com/MSeifert04/iteration_utilities/blob/master/src/iteration_utilities/_iteration_utilities/docsfunctions.h)
    而模块的注释：

- 操作list

    https://stackoverflow.com/questions/50668981/how-to-return-a-list-of-ints-in-python-c-api-extension-with-pylist