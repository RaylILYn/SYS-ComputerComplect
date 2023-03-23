// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "string"


int p = 0, c = 0;
HINSTANCE inst = NULL;
HWND Main = NULL;
std::string* filter2; // объявляем указатель на строку 
TCHAR tt[255];

__declspec(dllexport) int WINAPI DialogCd(HWND, std::string*&); // экспортируем функцию окна сд привода

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: // Процесс загружает DLL
        inst = hModule;
        break;
    case DLL_THREAD_ATTACH: // Создание нового потока
    case DLL_THREAD_DETACH:// Отключение DLL
    case DLL_PROCESS_DETACH:// Подключение DLL
        break;
    }
    return TRUE;
}

BOOL CALLBACK Cd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        *filter2 = "";
        for (int i = 0; i < 255; ++i) tt[i] = '\0';
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_RADIO11:
            strcpy_s(tt, "DDR5");
            break;
        case IDC_RADIO22:
            strcpy_s(tt, "DDR4");
            break;
        case IDC_RADIO33:
            strcpy_s(tt, "DDR3");
            break;
        case IDC_RADIO44:
            strcpy_s(tt, "DDR2");
            break;
        case IDOK1:
            if (IsDlgButtonChecked(hWnd, IDC_RADIO55)) GetDlgItemText(hWnd, IDC_EDIT2, tt, sizeof(tt)); // если отмечено "Other", переносим ввод в фильтр
            *filter2 = std::string(tt);
            PostMessage(Main, 155, NULL, NULL);
            EndDialog(hWnd, IDOK1);
            break;
        case IDCANCEL2:
            if (IsDlgButtonChecked(hWnd, IDC_RADIO55)) GetDlgItemText(hWnd, IDC_EDIT2, tt, sizeof(tt));
            *filter2 = std::string(tt);
            PostMessage(Main, 155, NULL, NULL);
            EndDialog(hWnd, IDCANCEL2);
            break;
        }
        return TRUE;
    case WM_DESTROY:
        PostMessage(Main, 155, NULL, NULL);
        DestroyWindow(hWnd);
        return TRUE;
    case WM_CLOSE:
        PostMessage(Main, 155, NULL, NULL);
        DestroyWindow(hWnd);
        return TRUE;
    default:
        return FALSE;
    }
    return FALSE;
}
int WINAPI DialogCd(HWND MainWnd, std::string*& Filter2)
{
    // предписание компилятору, чтобы он сохранил имя экспортируемой функции таким, какое оно в коде
    #pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)
    Main = MainWnd; // запоминаем окно основного процесса
    filter2 = Filter2; // указываем на фильтр привода основного процесса
    DialogBox(inst, MAKEINTRESOURCE(ID_CH_CD), NULL, Cd); // создаем окно
    return 0;
}