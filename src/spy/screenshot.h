#pragma once

BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName);
BOOL SaveBitmap(HBITMAP hBitmap, HANDLE fh);
HBITMAP   GetCaptureBmp();
BOOL ScreenShotSave(const WCHAR* path);
