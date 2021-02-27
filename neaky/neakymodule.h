
#ifdef __cplusplus
extern "C" {
#endif

extern HINSTANCE hInst;
extern HINSTANCE hModule; // dll模块实例

extern WCHAR executable_path[MAX_PATH + 1];
extern WCHAR module_path[MAX_PATH + 1];

extern PyObject *NeakyError;


#ifdef __cplusplus
}
#endif