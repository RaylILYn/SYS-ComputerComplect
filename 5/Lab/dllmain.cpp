// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"



    
    struct laptop {
        TCHAR model[255];
        TCHAR videocard[255];
        TCHAR cd[255];
        int price = -1;
    } laptops[50], filteredLaptops[50];
    int p = 0, c = 0;
    HINSTANCE inst = NULL;
    HWND Main = NULL, hWndText = NULL;
    char filter1[255], filter2[255], tt[255], buff[255];
    std::string strFilt1 = "", strFilt2 = "", res = "", t = "";

    int toInt(TCHAR* s)
    {
        int numb = 0;
        for (int i = 0; i < 255 && s[i] - '0' >= 0 && s[i] - '9' <= 0; ++i)
        {
            numb = numb * 10 + (s[i] - '0');
        }
        return numb;
    }
    void sortLaptops(laptop* laptops)
    {
        for (int i = 0; i < 50; ++i)
        {
            for (int j = i + 1; j < 50; j++)
            {
                if (laptops[i].price > laptops[j].price && laptops[j].price != -1)
                {
                    p = laptops[i].price;
                    laptops[i].price = laptops[j].price;
                    laptops[j].price = p;
                }
            }
        }
    }

    // разделение текста *data* на объекты *laptop* 
    void getLaptops(std::string data, laptop* laptops)
    {
        int c = 0;
        int count = 0;
        std::vector<std::string> lines, values;
        std::string line = "";
        for (int i = 0; i < data.length(); ++i)
        {
            if (data[i] == '\n') // разделение на строки
            {
                lines.push_back(line);
                line = "";
            }
            else {
                line += data[i];
            }
        }

        for (int i = 0; i < (int)lines.size() && i < 50; ++i)
        {
            c = 0;
            values.clear();
            line = "";
            for (int j = 0; j < lines[i].length(); ++j)
            {
                if (lines[i][j] == '#' || lines[i][j] == '\r') // разделение на параметры модели
                {
                    values.push_back(line);
                    line = "";
                }
                else {
                    line += lines[i][j];
                }
            }
            strcpy_s(laptops[i].model, values[0].c_str());
            laptops[i].price = toInt((TCHAR*)values[1].c_str());
            strcpy_s(laptops[i].videocard, values[2].c_str());
            strcpy_s(laptops[i].cd, values[3].c_str());
            c++;
        }
        sortLaptops(laptops);
    }

    // фильтрация ноутбуков
    void filterLaptop(laptop* laptops, char* filter1, char* filter2)
    {
        for (int i = 0; i < 50; i++) filteredLaptops[i].price = -1;
        c = 0;
        strFilt1 = "";
        for (int i = 0; i < 255 && filter1[i] != '\0'; ++i) strFilt1 += filter1[i];
        strFilt2 = "";
        for (int i = 0; i < 255 && filter2[i] != '\0'; ++i) strFilt2 += filter2[i];
        for (int i = 0; i < 50; ++i)
        {
            if (filter1[0] != '\0' && filter2[0] != '\0') {
                if (std::strcmp(laptops[i].videocard, strFilt1.c_str()) == 0 && std::strcmp(laptops[i].cd, strFilt2.c_str()) == 0)
                {
                    filteredLaptops[c] = laptops[i];
                    c++;
                }
            }
            else if (filter1[0] == '\0' && filter2[0] != '\0')
            {
                if (std::strcmp(laptops[i].cd, strFilt2.c_str()) == 0)
                {
                    filteredLaptops[c] = laptops[i];
                    c++;
                }
            }
            else if (filter2[0] == '\0' && filter1[0] != '\0')
            {
                if (std::strcmp(laptops[i].videocard, strFilt1.c_str()) == 0)
                {
                    filteredLaptops[c] = laptops[i];
                    c++;
                }
            }
            else {
                filteredLaptops[c] = laptops[i];
                c++;
            }
        }
        sortLaptops(filteredLaptops);
    }

    // подготовка объектов *laptop* к выводу строкой
    std::string formatLaptops(laptop* laptops)
    {
        res = "";
        for (int i = 0; i < 1000; ++i) buff[i] = '\0';
        for (int i = 0; i < 50; ++i)
        {
            if (laptops[i].price == -1) break;
            snprintf(buff, sizeof(buff), "Модель: %s, цена: %d, видеокарта: %s, cd-rom: %s\r\n", laptops[i].model, laptops[i].price, laptops[i].videocard, laptops[i].cd);

            for (int j = 0; j < 1000; ++j)
            {
                if (buff[j] != '\0') res += buff[j];
                buff[j] = '\0';
            }
        }
        return res;
    }
    BOOL APIENTRY DllMain(HMODULE hModule,
        DWORD  ul_reason_for_call,
        LPVOID lpReserved
    )
    {
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
            inst = hModule;
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
        }
        return TRUE;
    }


    BOOL CALLBACK Cd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_INITDIALOG:
            for (int i = 0; i < 255; ++i)
            {
                filter2[i] = '\0';
                tt[i] = '\0';
            }
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDC_RADIO11:
                strcpy_s(tt, "48x");
                break;
            case IDC_RADIO22:
                strcpy_s(tt, "52x");
                break;
            case IDC_RADIO33:
                strcpy_s(tt, "CD-RW");
                break;
            case IDC_RADIO44:
                strcpy_s(tt, "DVD");
                break;
            case IDOK1:
                if (IsDlgButtonChecked(hWnd, IDC_RADIO55)) GetDlgItemText(hWnd, IDC_EDIT2, tt, sizeof(tt)); // если отмечено "Other", переносим ввод в фильтр
                strcpy_s(filter2, tt);
                DestroyWindow(hWndText);
                filterLaptop(laptops, filter1, filter2);
                t = formatLaptops(filteredLaptops);
                hWndText = CreateWindowEx(0, "EDIT", t.c_str(),
                    WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                    ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                    10, 10, 880, 580,
                    Main, (HMENU)0, inst, NULL);
                EndDialog(hWnd, IDOK1);
                break;
            case IDCANCEL2:
                if (IsDlgButtonChecked(hWnd, IDC_RADIO55)) GetDlgItemText(hWnd, IDC_EDIT2, tt, sizeof(tt));
                strcpy_s(filter2, tt);
                DestroyWindow(hWndText);
                filterLaptop(laptops, filter1, filter2);
                t = formatLaptops(filteredLaptops);
                hWndText = CreateWindowEx(0, "EDIT", t.c_str(),
                    WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                    ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                    10, 10, 880, 580,
                    Main, (HMENU)0, inst, NULL);
                EndDialog(hWnd, IDCANCEL2);
                break;
            }
            return TRUE;
        case WM_DESTROY:
            for (int i = 0; i < 255; i++) filter2[i] = '\0';
            DestroyWindow(hWnd);
            return TRUE;
        case WM_CLOSE:
            for (int i = 0; i < 255; i++) filter2[i] = '\0';
            DestroyWindow(hWnd);
            return TRUE;
        default:
            return FALSE;
        }
        return FALSE;
    }
extern "C" {
    __declspec(dllexport) BOOL WINAPI Init(HWND MainWnd, HWND Text, char* Filter1, char* Filter2)
    {
        Main = MainWnd;
        strcpy_s(filter1, Filter1);
        strcpy_s(filter2, Filter2);
        hWndText = Text;
        DialogBox(inst, MAKEINTRESOURCE(ID_CH_CD), NULL, (DLGPROC)Cd);
        return TRUE;
    }
}
