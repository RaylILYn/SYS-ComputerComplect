// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
extern "C" { // применяем приведение к С, чтобы избежать смены имен экспортируемых данных
    // объявление экспортируемых данных
    __declspec(dllexport) HICON icon;
    __declspec(dllexport) HICON iconSmall;
    __declspec(dllexport) HBITMAP anim1;
    __declspec(dllexport) HBITMAP anim2;
    __declspec(dllexport) HBITMAP anim3;
    __declspec(dllexport) HBITMAP anim4;
    __declspec(dllexport) HCURSOR cursor;

    BOOL APIENTRY DllMain(HMODULE hModule,
        DWORD  ul_reason_for_call,
        LPVOID lpReserved
    )
    {
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH: // прикрепление dll к процессу
            icon = LoadIcon(hModule, MAKEINTRESOURCE(IDI_ICON1));
            iconSmall = LoadIcon(hModule, MAKEINTRESOURCE(IDI_ICON2));
            anim1 = LoadBitmap(hModule, MAKEINTRESOURCE(IDB_BITMAP1));
            anim2 = LoadBitmap(hModule, MAKEINTRESOURCE(IDB_BITMAP2));
            anim3 = LoadBitmap(hModule, MAKEINTRESOURCE(IDB_BITMAP3));
            anim4 = LoadBitmap(hModule, MAKEINTRESOURCE(IDB_BITMAP4));
            cursor = LoadCursor(hModule, MAKEINTRESOURCE(IDC_CURSOR1));
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH: // открепление dll от процесса
            DestroyIcon(icon);
            DestroyIcon(iconSmall);
            DestroyCursor(cursor);
            DeleteObject(anim1);
            DeleteObject(anim2);
            DeleteObject(anim3);
            DeleteObject(anim4);
            break;
        }
        return TRUE;
    }
}