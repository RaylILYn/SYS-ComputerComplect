#pragma warning (disable : 4996)
#pragma comment(lib, "msimg32.lib")
#include "framework.h"
#include "Lab.h"
#include "stdio.h"
#include "string"
#include "vector"
#include <commdlg.h>
#include <vector>
#include <iostream>

#define MAX_LOADSTRING 100
#define MaxLength 250
   
// Глобальные переменные:
HINSTANCE hInst;                                // Текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // Имя класса главного окна

    // DllRes
HWND hWndText, Main;
HMODULE hDllRes;
HICON icon;
HICON small;
HCURSOR hCursor; 
DWORD b;
WINGDIAPI int WINAPI DialogCd(HWND, std::string*&); // Импорт функции из dll 

    // Анимация
HWND mHwnd;
RECT clientRect;
HBITMAP pattern;
HBITMAP dog;
BLENDFUNCTION blf = { AC_SRC_OVER, 0, 255, 0 };
bool forward = true;
int alpha = 0;

HGDIOBJ Anim, anim1, anim2, anim3, anim4;
bool anim = false;
int anim01 = 0, anim02 = 0, animState = 1;
int xc = 15, yc = 315, xt = 0, yt = 0, vx = 5, vy = 5;


std::string res, t, strFilt1, strFilt2, fullText = "", filt = "";
char buff[1000];
FILE* fileStream;
std::vector<wchar_t*> fileInfo;
int i = 0;
int y = 0;
int c = 0;
POINT point;

int xpos, ypos, wmId, wmEvent;
HDC hdc, memDC, memDC1;
PAINTSTRUCT ps;
static OPENFILENAME file;
static TCHAR name[MAX_PATH];
static HANDLE hFile;
BOOL success;
int j = 0;
DWORD dw = 0;
static char text[MaxLength];
char filename[MAX_PATH];

std::string* filter2 = &filt; // Указатель на адрес переменной, чтобы ее можно было изменять из dll
char filter1[255];
BOOL flag = FALSE;
int p = 0;

int toInt(TCHAR* s)
{
    int numb = 0;
    for (int i = 0; i < 255 && s[i] - '0' >= 0 && s[i] - '9' <= 0; ++i)
    {
        numb = numb * 10 + (s[i] - '0');
    }
    return numb;
}

struct computer {
    TCHAR model[255];
    TCHAR harddisk[255];
    TCHAR ram[255];
    int price = -1;
} lp, computers[50], filteredcomp[50];

    // Сортировка компьютеров по цене
void sortComputer(computer* computer)
{
    for (int i = 0; i < 50; ++i)
    {
        for (int j = i + 1; j < 50; j++)
        {
            if (computer[i].price > computer[j].price && computer[j].price != -1)
            {
                lp = computer[i];
                computer[i] = computer[j];
                computer[j] = lp;
            }
        }
    }
}

    // Разделение текста *data* на объекты *computer* 
void getComputers(std::string data, computer* computers)
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
        strcpy_s(computers[i].model, values[0].c_str());
        computers[i].price = toInt((TCHAR*)values[1].c_str());
        strcpy_s(computers[i].harddisk, values[2].c_str());
        strcpy_s(computers[i].ram, values[3].c_str());
        c++;
    }
    sortComputer(computers);
}

    // Фильтрация компьютеров
void filterComp(computer* computers, char* filter1, std::string filter2)
{
    for (int i = 0; i < 50; i++) filteredcomp[i].price = -1;
    c = 0;
    strFilt1 = "";
    for (int i = 0; i < 255 && filter1[i] != '\0'; ++i) strFilt1 += filter1[i];
    strFilt2 = "";
    strFilt2 = filter2;
    for (int i = 0; i < 50; ++i)
    {
        if (filter1[0] != '\0' && filter2 != "") {
            if (std::strcmp(computers[i].harddisk, strFilt1.c_str()) == 0 && std::strcmp(computers[i].ram, strFilt2.c_str()) == 0)
            {
                filteredcomp[c] = computers[i];
                c++;
            }
        }
        else if (filter1[0] == '\0' && filter2 != "")
        {
            if (std::strcmp(computers[i].ram, strFilt2.c_str()) == 0)
            {
                filteredcomp[c] = computers[i];
                c++;
            }
        }
        else if (filter2 == "" && filter1[0] != '\0')
        {
            if (std::strcmp(computers[i].harddisk, strFilt1.c_str()) == 0)
            {
                filteredcomp[c] = computers[i];
                c++;
            }
        }
        else {
            filteredcomp[c] = computers[i];
            c++;
        }
    }
    sortComputer(filteredcomp);
}

    // Подготовка объектов *computers* к выводу строкой
std::string formatComp(computer* computers)
{
    res = "";
    for (int i = 0; i < 1000; ++i) buff[i] = '\0';
    for (int i = 0; i < 50; ++i)
    {
        if (computers[i].price == -1) break;
        snprintf(buff, sizeof(buff), "Model: %s\t  Price: %d\t  Hard:  %s\t  RAM:  %s\t\r\n", computers[i].model, computers[i].price, computers[i].harddisk, computers[i].ram);
        
        for (int j = 0; j < 1000; ++j)
        {
            if (buff[j] != '\0') res += buff[j];
            buff[j] = '\0';
        }
    }
    return res;
}

    // Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
    
    // Точка входа в программу
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{   
    // 1. hInstance - Дескриптор экземпляра. ОС использует это значение для идентификации исполняемого файла (EXE) при загрузке в память
    // 2. hPrevInstance - Не имеет смысла, он использовался в 16-разрядной Windows, но теперь всегда равен NULL
    // 3. lpCmdLine - Содержит агрументы командной строки в виде строки Юникода
    // 4. nCmdShow - Флаг, указывающий, будет ли главное окно приложения свернуто, развернуто или показано обычным образом
    
    // Параметры которые не используются
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadString(hInstance, IDS_APP_TITLE, (LPSTR)szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_LAB, (LPSTR)szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 1. [in, optional] HINSTANCE hInstance    - Дескриптор экземпляра модуля исполняемый файл которого содержит строковый ресурс
    // 2. [in] UINT uID                         - Идентификатор строки для загрузки
    // 3. [out] LPWSTR lpBuffer                 - Буфер для приема строки
    // 4. [in] int cchBufferMax                 - Размер буфера в символах
    
    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Загрузка таблицы акселераторов в память и получение ее
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB));
    // Информация о сообщениях из очереди сообщений потока
    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(Main, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

    // Функция MyRegisterClass() регистрирует класс окна
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = icon;
    wcex.hCursor        = hCursor;
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_LAB); 
    wcex.lpszClassName  = (LPSTR)szWindowClass;
    wcex.hIconSm        = small;
    return RegisterClassEx(&wcex);
}
    
    // Функция InitInstance(HINSTANCE, int) сохраняет маркер экземпляра и создает главное окно
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной
   
   /*
   pattern = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
   dog = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
   */

   HWND hWnd = CreateWindow((LPSTR)szWindowClass, (LPSTR)szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL | WS_BORDER, 75, 75, 760, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   Main = hWnd;
   hWndText = CreateWindowEx(0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE, 0, 0, 760, 600, hWnd, (HMENU)0, hInst, NULL);
   
   // Загрузка меню из ресурсов
   HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(ID_MENU));
   SetMenu(hWnd, hMenu);
    
   // Настройка структуры файла для открытия
   file.lStructSize = sizeof(OPENFILENAME);
   file.hInstance = hInst;
   file.lpstrFilter = _T("Text\0*.txt");
   file.lpstrFile = name;
   file.nMaxFile = 256;
   file.lpstrTitle = _T("Открыть файл для чтения");
   file.lpstrInitialDir = _T(".\\");
   file.lpstrDefExt = _T("txt");

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

TCHAR model[255];
char tt[255];

    // Функция по поиску жесткого диска
BOOL CALLBACK Hard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) // обработка окна-фильтра видеокарт
{
    switch (message)
    {
    case WM_INITDIALOG:
        for (int i = 0; i < 255; ++i)
        {
            filter1[i] = '\0';
            tt[i] = '\0';
        }
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_RADIO1:
            strcpy_s(tt, "Seagate");
            break;
        case IDC_RADIO2:
            strcpy_s(tt, "WDBlue");
            break;
        case IDC_RADIO3:
            strcpy_s(tt, "Samsung");
            break;
        case IDC_RADIO4:
            strcpy_s(tt, "Kingston");
            break;
        case IDC_RADIO5:
            strcpy_s(tt, "ADATA");
            break;
        case IDC_RADIO6:
            strcpy_s(tt, "NETAC");
            break;
        case IDOK:
            // Если отмечено "Other", переносим ввод в фильтр
            if (IsDlgButtonChecked(hWnd, IDC_RADIO7)) GetDlgItemText(hWnd, IDC_EDIT1, tt, sizeof(tt)); // если отмечено "Other", переносим ввод в фильтр
            strcpy_s(filter1, tt);
            DestroyWindow(hWndText);
            filterComp(computers, filter1, *filter2);
            t = formatComp(filteredcomp);
            hWndText = CreateWindowEx(0, "EDIT", t.c_str(), WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 50, 50, 500, 200, Main, (HMENU)0, hInst, NULL);
            EndDialog(hWnd, IDOK);
            break;
        case IDCANCEL1:
            if (IsDlgButtonChecked(hWnd, IDC_RADIO7)) GetDlgItemText(hWnd, IDC_EDIT1, tt, sizeof(tt));
            strcpy_s(filter1, tt);
            DestroyWindow(hWndText);
            filterComp(computers, filter1, *filter2);
            t = formatComp(filteredcomp);
            hWndText = CreateWindowEx(0, "EDIT", t.c_str(), WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 50, 50, 500, 200, Main, (HMENU)0, hInst, NULL);
            EndDialog(hWnd, IDCANCEL1);
            break;
        }
        return TRUE;
    case WM_DESTROY:
        DestroyWindow(hWnd);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return TRUE;
    default:
        return FALSE;
    }
    return TRUE;
}

computer item;
TCHAR strPrice[255];
std::string base = "";
int m = 0, c1 = 0;

    // Функция добавления модели
BOOL CALLBACK AddItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) // обработка окна добавления модели
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK3:
            GetDlgItemText(hWnd, IDC_EDIT1, item.model, sizeof(item.model));
            GetDlgItemText(hWnd, IDC_EDIT2, strPrice, sizeof(strPrice));
            item.price = toInt(strPrice);
            GetDlgItemText(hWnd, IDC_EDIT3, item.harddisk, sizeof(item.harddisk));
            GetDlgItemText(hWnd, IDC_EDIT4, item.ram, sizeof(item.ram));
            if (item.model[0] != '\0' && item.harddisk[0] != '\0' && item.ram[0] != '\0' && item.price) {
                base = "";
                base.append(item.model);
                base += '#';
                base.append(strPrice);
                base += '#';
                base.append(item.harddisk);
                base += '#';
                base.append(item.ram);
                base += "\r\n";

                hFile = CreateFile(name, FILE_APPEND_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                m = 0;
                while (m < (int)base.length())
                {
                    c1 = 0;
                    for (int j = m; j < m + 250; ++j)
                    {

                        if (j >= (int)base.length())
                        {
                            text[j % 250] = '\0';
                            continue;
                        }
                        text[j % 250] = base[j];
                        c1++;
                    }
                    m += 250;
                    if (!(WriteFile(hFile, &text, c1, &dw, NULL) && dw != 0))
                    {
                        break;
                    }

                }
                CloseHandle(hFile);

                DestroyWindow(hWndText);
                for (int i = 0; i < MaxLength; ++i) text[i] = '\0';
                fullText = "";

                hFile = CreateFile(name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                while (ReadFile(hFile, &text, sizeof(text), &dw, NULL) && dw != 0)
                {
                    fullText.append(text);
                    for (int i = 0; i < MaxLength; ++i) text[i] = '\0';
                }
                
                CloseHandle(hFile);
                EndDialog(hWnd, IDOK3);
                getComputers(fullText, computers);
                t = formatComp(computers);
                hWndText = CreateWindowEx(0, "EDIT", t.c_str(), WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 0, 0, 600, 400, Main, (HMENU)0, hInst, NULL);
                break;
            }
            else {
                MessageBox(hWnd, _T("Похоже, вы ввели не все данные либо указали недопустимые значения"), _T("Ошибка"), MB_OK);
                break;
            }
        case IDCANCEL3:
            EndDialog(hWnd, IDCANCEL3);
            break;
        }
        return TRUE;
    case WM_DESTROY:
        for (int i = 0; i < 255; i++) item.model[i] = '\0';
        for (int i = 0; i < 255; i++) item.harddisk[i] = '\0';
        for (int i = 0; i < 255; i++) item.ram[i] = '\0';
        item.price = 0;
        DestroyWindow(hWnd);
        break;
    case WM_CLOSE:
        for (int i = 0; i < 255; i++) item.model[i] = '\0';
        for (int i = 0; i < 255; i++) item.harddisk[i] = '\0';
        for (int i = 0; i < 255; i++) item.ram[i] = '\0';
        item.price = 0;
        DestroyWindow(hWnd);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
   
    // Обработка общего окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Обработка нажатий ПКМ по главному окну
    case WM_RBUTTONDOWN:
        HMENU hSubMenu, hMenu;
        POINT pt;
        int xpos, ypos;
        xpos = LOWORD(lParam);          // Позиция х мыши
        ypos = HIWORD(lParam);          // Позиция у мыши
        pt = { xpos, ypos };            // Точка с координатами х и у
        ClientToScreen(hWnd, &pt);      // Перевод из экранных координат в оконные

        // Показ контекстного меню главного окна
        hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_CONTEXT_MENU));
        hSubMenu = GetSubMenu(hMenu, 0);
        TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
        break;

        // Dll
    case 155:
        DestroyWindow(hWndText);
        filterComp(computers, filter1, *filter2);
        t = formatComp(filteredcomp);
        hWndText = CreateWindowEx(0, "EDIT", t.c_str(), WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 50, 50, 500, 200, Main, (HMENU)0, hInst, NULL);
        break;

    case WM_SETCURSOR:
        SetCursor(hCursor);
        break;

    case WM_CREATE:
        // При создании окна импортируем библиотеку с ресурсами и получаем необходимые программе ресурсы
        hDllRes = LoadLibrary("DllRes");
        icon = *((HICON*)GetProcAddress(hDllRes, "icon"));
        small = *((HICON*)GetProcAddress(hDllRes, "iconSmall"));
        hCursor = *((HCURSOR*)GetProcAddress(hDllRes, "cursor"));
        anim1 = *((HBITMAP*)GetProcAddress(hDllRes, "anim1"));
        anim2 = *((HBITMAP*)GetProcAddress(hDllRes, "anim2"));
        anim3 = *((HBITMAP*)GetProcAddress(hDllRes, "anim3"));
        anim4 = *((HBITMAP*)GetProcAddress(hDllRes, "anim4"));
        SetClassLong(hWnd, GCL_HICON, (LONG)icon);
        SetClassLong(hWnd, GCL_HICONSM, (LONG)small);
        SetClassLong(hWnd, GCL_HCURSOR, (LONG)hCursor);
        hdc = GetDC(hWnd);
        memDC = CreateCompatibleDC(hdc);
        SetTimer(hWnd, 0, 50, NULL);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
                // Обработка кнопки "About"
            case ID_HELP_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, About);
                break;

                // Обработка кнопки "Exit"
            case ID_FILE_EXIT:
                DestroyWindow(hWnd);
                break;

                // Обработка кнопки "Open File"
            case ID_FILE_OPENBASE:
                flag = FALSE;
                DestroyWindow(hWndText);
                for (int i = 0; i < MaxLength; ++i) text[i] = '\0';
                fullText = "";
                for (int i = 0; i < 50; ++i) computers[i].price = -1;
                file.lpstrTitle = _T("Открыть файл для чтения");
                if (!GetOpenFileName(&file)) return 1;
                hFile = CreateFile(name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                while (ReadFile(hFile, &text, sizeof(text), &dw, NULL) && dw != 0)
                {
                    fullText.append(text);
                    for (int i = 0; i < MaxLength; ++i) text[i] = '\0';
                }
                getComputers(fullText, computers);
                t = formatComp(computers);
                hWndText = CreateWindowEx(0, "EDIT", t.c_str(), WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | WS_HSCROLL | ES_READONLY, 50, 50, 500, 200, hWnd, (HMENU)0, hInst, NULL);
                flag = TRUE;
                CloseHandle(hFile);
                break;
                
                // Обработка кнопки "Harddisk"
            case ID_INFO_MODEL:
                DialogBox(hInst, MAKEINTRESOURCE(ID_CH_MODEL), NULL, Hard);
                break;

                // Обработка кнопки "RAM"
            case ID_INFO_CD:
                *filter2 = "";
                DialogCd(hWnd, filter2); // Вызов функции, находящейся в dll
                break;

            case ID_FILE_ADDITEM:
                if (!flag)
                {
                    MessageBox(hWnd, _T("Сначала откройте базу данных"), _T("Ошибка"), MB_OK);
                    break;
                }
                DialogBox(hInst, MAKEINTRESOURCE(DIAL_ADDITEM), NULL, AddItem);
                break;

                // Обработка кнопки "Save File"
            case ID_FILE_SAVE:
                file.lpstrTitle = _T("Сохранить файл как");
                if (!GetSaveFileName(&file)) return 1;
                hFile = CreateFile(name, FILE_APPEND_DATA, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                m = 0;
                while (m < (int)fullText.length())
                {
                    c1 = 0;
                    for (int j = m; j < m + 250; ++j)
                    {

                        if (j >= (int)fullText.length())
                        {
                            text[j % 250] = '\0';
                            continue;
                        }
                        text[j % 250] = fullText[j];
                        c1++;
                    }
                    m += 250;
                    if (!(WriteFile(hFile, &text, c1, &dw, NULL) && dw != 0))
                    {
                        break;
                    }
                }
                CloseHandle(hFile);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
        GetClientRect(mHwnd, &clientRect);
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HPEN newPen = CreatePen(PS_SOLID, 10, RGB(190, 190, 190));
        HBRUSH newBrush = CreatePatternBrush(pattern);

        HGDIOBJ oldBrush = SelectObject(hdc, newBrush);
        HGDIOBJ oldPen = SelectObject(hdc, newPen);

        Rectangle(hdc, clientRect.left + 590, clientRect.top + 300, clientRect.right + 10, clientRect.bottom + 10);
        SelectObject(hdc, oldBrush);
        DeleteObject(newBrush);


        Rectangle(hdc, clientRect.left + 10, clientRect.top + 421, clientRect.right + 590, clientRect.bottom + 300);

        BITMAP dogObject;
        GetObject(dog, sizeof(BITMAP), &dogObject);

        HDC memDC = CreateCompatibleDC(hdc);
        SelectObject(memDC, dog);

        if (alpha >= 255) forward = false;
        if (alpha <= 0) forward = true;

        if (forward) alpha += 20;
        else alpha -= 20;

        blf.SourceConstantAlpha = alpha;
        AlphaBlend(hdc, 330, 310, dogObject.bmWidth, dogObject.bmHeight, memDC, 0, 0, dogObject.bmWidth, dogObject.bmHeight, blf);

        DeleteDC(memDC);

        SelectObject(hdc, oldPen);
        DeleteObject(newPen);

        EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        FreeLibrary(hDllRes);
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_TIMER:
        hdc = GetDC(hWnd);
        switch (animState) // Перебираем кадры анимации
        {
        case 1:
            animState = 2;
            Anim = anim2;
            break;
        case 2:
            animState = 3;
            Anim = anim3;
            break;
        case 3:
            animState = 4;
            Anim = anim4;
            break;
        case 4:
            animState = 1;
            Anim = anim1;
            break;
        }
        //Функция SelectObject выбирает объект в указанном контексте устройства (DC). Новый объект заменяет предыдущий объект того же типа
        SelectObject(memDC, Anim);

        // Функция BitBlt выполняет передачу битовых блоков данных о цвете, соответствующих прямоугольнику пикселей из заданного исходного контекста устройства в целевой контекст устройства
        BitBlt(hdc, xc, yc, 100, 100, NULL, 0, 0, PATCOPY);
        
        // 1. HDC hdcDest - Дескриптор целевого DC
        // 2. int nXDest - X-координата левого верхнего угла целевого прямоугольника
        // 3. int nYDest - Y-координата левого верхнего угла целевого прямоугольника
        // 4. int nWidth - Ширина целевого прямоугольника
        // 5. int nHeight - Высота целевого прямоугольника
        // 6. HDC hdcSrc - Дескриптор исходного DC
        // 7. int nXSrc - X-координата левого верхнего угла исходного прямоугольника
        // 8. int nYSrc - Y-коорд. левого верхнего угла исходного прямоугольника
        // 9. DWORD dwRop - Код растровой операции

        if (!anim) // Считаем новые координаты и сторону движения анимации
        {
            xc += 10;
            anim01 += 1;
        }
        else {
            xc -= 10;
            anim02 += 1;
        }
        if (anim01 == 46 || anim02 == 46) // После выполнения 46 передвижений - смена направления
        {
            anim = !anim;
            anim01 = 0;
            anim02 = 0;
        }
        BitBlt(hdc, xc, yc, 100, 100, memDC, 0, 0, SRCCOPY);
       
        // Функция ReleaseDC освобождает контекст устройства  (DC)  для использования другими приложениями
        ReleaseDC(hWnd, hdc);

    default:
        // Функция DefWindowProc вызывается оконной процедурой по умолчанию, чтобы обеспечить обработку по умолчанию любого сообщения окна
        return DefWindowProc(hWnd, message, wParam, lParam);
        // 1.   hWnd - Дескриптор оконной процедуры, которая получает сообщение.
        // 2.   message - Определяет дополнительную информацию о сообщении. 
        // 3-4. wParam, lParam - Содержание этого параметра зависит от значения параметра 
    }
    return 0;
}

    // Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON1 || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

