#include "keyboardlock.h"

/*********************************************************************************************************/
void AppendWindowText(HWND hWnd, LPWSTR lpString)
{
    int iLength = GetWindowTextLength(hWnd);
    SendMessage(hWnd, EM_SETSEL, iLength, iLength);
    SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)lpString);
    SendMessage(hWnd, WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL);
}
/*********************************************************************************************************/
INT GetValueFromRegistryKey(HKEY hKey, LPWSTR registryKeyName, LPWSTR registryValueName, VOID* pValue, DWORD sizeValue)
{
    HKEY RegistryKey;
    PWCHAR name = NULL;
    DWORD maxValueLen = 0;

    RegOpenKeyW(hKey, registryKeyName, &RegistryKey);

    int numberValues = 0;
    if (RegQueryInfoKey(RegistryKey, NULL, NULL, NULL, NULL, NULL, NULL, &numberValues, &maxValueLen, NULL, NULL, NULL) != ERROR_SUCCESS)
        return -1;

    name = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)maxValueLen * sizeof(WCHAR) + 1);
    if (name == NULL)
    {
        RegCloseKey(RegistryKey);
        return -1;
    }
        
    for (int i = 0; i < numberValues; i++)
    {
        DWORD sizeName = maxValueLen + 1;
        DWORD data = 0;
        DWORD sizeData = sizeValue;

        RegEnumValue(RegistryKey, i, name, &sizeName, NULL, NULL, (PBYTE)pValue, &sizeData);

        if (!wcscmp(name, registryValueName))
        {
            HeapFree(GetProcessHeap(), 0, name);
            RegCloseKey(RegistryKey);
            return 0;
        }
    }

    HeapFree(GetProcessHeap(), 0, name);
    RegCloseKey(RegistryKey);

    return -1;
}
/*********************************************************************************************************/
VOID SetPosition(HWND window)
{
    RECT rect;
    HKEY RegistryKey;
    RegOpenKey(HKEY_CURRENT_USER, DZSoftawre, &RegistryKey);

    if (RegistryKey == NULL)
    {
        RegCreateKeyEx(HKEY_CURRENT_USER, DZSoftawre, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &RegistryKey, NULL);
    }

    GetWindowRect(window, &rect);

    if (window == hwndCapsLock)
    {
        RegSetValueEx(RegistryKey, TEXT("CAPSX"), 0, REG_DWORD, (const BYTE*)&rect.left, sizeof(LONG));
        RegSetValueEx(RegistryKey, TEXT("CAPSY"), 0, REG_DWORD, (const BYTE*)&rect.top, sizeof(LONG));

        if (debug == 1)
        {
            WCHAR buffer[2048] = { 0 };
            wsprintf(buffer, L"SETPOSITION : CAPS LOCK\r\nX = %d Y = %ld\r\n\r\n", rect.left, rect.top);
            AppendWindowText(editDebug, buffer);
        }
    }
    else if (window == hwndNumLock)
    {
        RegSetValueEx(RegistryKey, TEXT("NUMX"), 0, REG_DWORD, (const BYTE*)&rect.left, sizeof(LONG));
        RegSetValueEx(RegistryKey, TEXT("NUMY"), 0, REG_DWORD, (const BYTE*)&rect.top, sizeof(LONG));

        if (debug == 1)
        {
            WCHAR buffer[2048] = { 0 };
            wsprintf(buffer, L"SETPOSITION : NUM  LOCK\r\nX = %d Y = %ld\r\n\r\n", rect.left, rect.top);
            AppendWindowText(editDebug, buffer);
        }
    }
}
/*********************************************************************************************************/
LRESULT CALLBACK DebugProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_CREATE:
            editDebug = CreateWindow(TEXT("edit"), NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL, 0, 0, 550, 450, hwnd, NULL, NULL, NULL);
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
/*********************************************************************************************************/
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    static RECT rect;
    static BOOL mouseDown = FALSE;
    static POINT lastLocation;

    static HWND staticCaps;
    static HWND staticNum;
    HFONT hFont;

    switch (msg)
    {
        case WM_CLOSE:
            return 0;
        case WM_CREATE:
        {
            hFont = CreateFont(18, 0, 0, 0, FW_BLACK, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Arial Black"));

            if (hwndCapsLock == NULL)
            {
                staticCaps = CreateWindow(TEXT("static"), NULL, WS_VISIBLE | WS_CHILD, 0, 0, 120, 20, hwnd, NULL, NULL, NULL);
                SendMessage(staticCaps, WM_SETFONT, (WPARAM)hFont, TRUE);
            }
            else
            {
                staticNum = CreateWindow(TEXT("static"), NULL, WS_VISIBLE | WS_CHILD, 0, 0, 120, 20, hwnd, NULL, NULL, NULL);
                SendMessage(staticNum, WM_SETFONT, (WPARAM)hFont, TRUE);
            }

            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            if (hwnd == hwndCapsLock)
            {
                if (capsLockStatus == TRUE)
                {
                    SetWindowText(staticCaps, TEXT("CAPS LOCK ON"));
                    FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB_GREEN));
                }
                else if (capsLockStatus == FALSE)
                {
                    SetWindowText(staticCaps, TEXT("CAPS LOCK OFF"));
                    FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB_RED));
                }
            }
            else if (hwnd == hwndNumLock)
            {
                if (numLockStatus == TRUE)
                {
                    SetWindowText(staticNum, TEXT("NUM  LOCK ON"));
                    FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB_GREEN));
                }
                else if (numLockStatus == FALSE)
                {
                    SetWindowText(staticNum, TEXT("NUM  LOCK OFF"));
                    FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB_RED));
                }
            }

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wparam;

            SetTextColor(hdcStatic, RGB_WHITE);
            SetTextColor(hdcStatic, RGB_BLACK);

            if (hwnd == hwndCapsLock)
            {
                if (capsLockStatus == TRUE)
                {
                    SetTextColor(hdcStatic, RGB_BLACK);
                    SetBkColor(hdcStatic, RGB_GREEN);
                    return (INT_PTR)CreateSolidBrush(RGB_GREEN);
                }
                else if (capsLockStatus == FALSE)
                {
                    SetBkColor(hdcStatic, RGB_RED);
                    return (INT_PTR)CreateSolidBrush(RGB_RED);
                }
            }
            else if (hwnd == hwndNumLock)
            {
                if (numLockStatus == TRUE)
                {
                    SetTextColor(hdcStatic, RGB_BLACK);
                    SetBkColor(hdcStatic, RGB_GREEN);
                    return (INT_PTR)CreateSolidBrush(RGB_GREEN);
                }
                else if (numLockStatus == FALSE)
                {
                    SetBkColor(hdcStatic, RGB_RED);
                    return (INT_PTR)CreateSolidBrush(RGB_RED);
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            SetCapture(hwnd);
            mouseDown = TRUE;
            GetCursorPos(&lastLocation);

            GetWindowRect(hwnd, &rect);
            lastLocation.x = lastLocation.x - rect.left;
            lastLocation.y = lastLocation.y - rect.top;
            break;
        }
        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            mouseDown = FALSE;
            SetPosition(hwnd);
            break;
        }
        case WM_MOUSEMOVE:
        {
            if (mouseDown)
            {
                POINT currentPosition;
                GetCursorPos(&currentPosition);
                MoveWindow(hwnd, currentPosition.x - lastLocation.x, currentPosition.y - lastLocation.y, rect.right - rect.left, rect.bottom - rect.top, FALSE);
            }
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
/*********************************************************************************************************/
VOID PrintCapsLock(SHORT state)
{
    EnterCriticalSection(&capsMutex);
    if (state == 0)
    {
        if (debug == 1)
            AppendWindowText(editDebug, TEXT("CAPSLOCK ON\n"));

        capsLockStatus = TRUE;
        RECT rect;
        GetWindowRect(hwndCapsLock, &rect);
        SetWindowPos(hwndCapsLock, HWND_TOPMOST, 0, 0, 112, 20, SWP_NOMOVE);
        RedrawWindow(hwndCapsLock, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
    }

    else if (state == 1)
    {
        if (debug == 1)
            AppendWindowText(editDebug, TEXT("CAPSLOCK OFF\n"));

        capsLockStatus = FALSE;
        RECT rect;
        GetWindowRect(hwndCapsLock, &rect);
        SetWindowPos(hwndCapsLock, HWND_TOPMOST, 0, 0, 120, 20, SWP_NOMOVE);
        RedrawWindow(hwndCapsLock, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
    }
    LeaveCriticalSection(&capsMutex);
}
/*********************************************************************************************************/
VOID PrintNumLock(SHORT state)
{
    EnterCriticalSection(&numMutex);
    if (state == 0)
    {
        if (debug == 1)
            AppendWindowText(editDebug, TEXT("NUMLOCK ON\n"));

        numLockStatus = TRUE;
        RECT rect;
        GetWindowRect(hwndNumLock, &rect);
        SetWindowPos(hwndNumLock, HWND_TOPMOST, 0, 0, 112, 20, SWP_NOMOVE);
        RedrawWindow(hwndNumLock, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
    }

    else if (state == 1)
    {
        if (debug == 1)
            AppendWindowText(editDebug, TEXT("NUMLOCK OFF\n"));

        numLockStatus = FALSE;
        RECT rect;
        GetWindowRect(hwndNumLock, &rect);
        SetWindowPos(hwndNumLock, HWND_TOPMOST, 0, 0, 120, 20, SWP_NOMOVE);
        RedrawWindow(hwndNumLock, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
    }
    LeaveCriticalSection(&numMutex);
}
/*********************************************************************************************************/
LRESULT WINAPI HookProc(int code, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
        case WM_KEYDOWN:
        {
            if (((KBDLLHOOKSTRUCT*)lParam)->vkCode == VK_CAPITAL)
                PrintCapsLock(GetKeyState(VK_CAPITAL));
            else if (((KBDLLHOOKSTRUCT*)lParam)->vkCode == VK_NUMLOCK)
                PrintNumLock(GetKeyState(VK_NUMLOCK));
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}
/*********************************************************************************************************/
DWORD WINAPI ThreadUpdateLocks(_In_ LPVOID lpParameter)
{
    SHORT state = 0;
    while (1)
    {
        Sleep(1000);

        state = GetKeyState(VK_CAPITAL);
        if (state != capsLockStatus && (state == 0 || state == 1))
            PrintCapsLock(!GetKeyState(VK_CAPITAL));

        state = GetKeyState(VK_NUMLOCK);
        if (state != numLockStatus && (state == 0 || state == 1))
            PrintNumLock(!GetKeyState(VK_NUMLOCK));
    }
    ExitThread(0);
}
/*********************************************************************************************************/
INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
    WNDCLASS classDebug = { 0 };
    MSG      msg = { 0 };
    POINT    capsPosition = { 0 };
    POINT    numPosition = { 0 };
    WNDCLASS classCapsLock = { 0 };
    WNDCLASS classNumLock = { 0 };
    WNDCLASS classControl = { 0 };
    DWORD    opacity;
    hinst = hInstance;

    InitCommonControls();
    InitializeCriticalSection(&capsMutex);
    InitializeCriticalSection(&numMutex);

    classControl.lpszClassName = TEXT("classControl");
    classControl.lpfnWndProc = ControlProc;
    classControl.hInstance = hInstance;
    classControl.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    RegisterClass(&classControl);

    hwndControl = CreateWindow(classControl.lpszClassName, TITLE,WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 400, NULL, NULL, NULL, NULL);
    if (hwndControl == NULL)
        return -1;

    classDebug.lpszClassName = TEXT("classDebug");
    classDebug.lpfnWndProc = DebugProc;
    classDebug.hInstance = hInstance;
    classDebug.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    
    RegisterClass(&classDebug);

    hwndDebug = CreateWindow(classDebug.lpszClassName, TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, NULL, NULL, NULL, NULL);
    if (hwndDebug == NULL)
        return -1;

    if (GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"DEBUG", (void*)&debug, sizeof(debug)) == -1)
        debug = 0;

    if (debug == 1)
        ShowWindow(hwndDebug, TRUE);

    if (GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"CAPSX", (void*)&capsPosition.x, sizeof(capsPosition.x)) == -1)
        capsPosition.x = 5;
    if(GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"CAPSY", (void*)&capsPosition.y, sizeof(capsPosition.y)) == -1)
        capsPosition.y = 5;
    if(GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"NUMX", (void*)&numPosition.x, sizeof(numPosition.x)) == -1)
        numPosition.x = 5;
    if(GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"NUMY", (void*)&numPosition.y, sizeof(numPosition.y)) == -1)
        numPosition.y = 30;


    classCapsLock.lpszClassName = TEXT("classCapsLock");
    classCapsLock.lpfnWndProc = WindowProc;
    classCapsLock.hInstance = hInstance;
    classCapsLock.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    RegisterClass(&classCapsLock);

    hwndCapsLock = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST, classCapsLock.lpszClassName, NULL, WS_VISIBLE | WS_POPUPWINDOW, capsPosition.x, capsPosition.y, 120, 20, NULL, NULL, NULL, NULL);
    if (hwndCapsLock == NULL)
        return -1;

    if (GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"CAPSOPACITY", (void*)&opacity, sizeof(opacity)) == -1)
        opacity = 80;

    SetLayeredWindowAttributes(hwndCapsLock, 0, 255 * (BYTE)opacity / 100, LWA_ALPHA);

    classNumLock.lpszClassName = TEXT("classNumLock");
    classNumLock.lpfnWndProc = WindowProc;
    classNumLock.hInstance = hInstance;
    classNumLock.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    RegisterClass(&classNumLock);

    hwndNumLock = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST, classNumLock.lpszClassName, NULL, WS_VISIBLE | WS_POPUPWINDOW, numPosition.x, numPosition.y, 120, 20, NULL, NULL, NULL, NULL);
    if (hwndNumLock == NULL)
        return -1;

    if (GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"NUMOPACITY", (void*)&opacity, sizeof(opacity)) == -1)
        opacity = 80;
    SetLayeredWindowAttributes(hwndNumLock, 0, 255 * (BYTE)opacity / 100, LWA_ALPHA);


    SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)HookProc, NULL, 0);

    PrintCapsLock(!GetKeyState(VK_CAPITAL));
    PrintNumLock(!GetKeyState(VK_NUMLOCK));

    TrayIcon.cbSize = sizeof(NOTIFYICONDATA);
    TrayIcon.hWnd = hwndControl;
    TrayIcon.uID = 0;
    TrayIcon.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON1));
    TrayIcon.uCallbackMessage = MY_WM_NOTIFYICON;
    TrayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    memcpy(TrayIcon.szTip, L"KeyboardLock Control Pannel", sizeof(L"KeyboardLock Control Pannel"));
    Shell_NotifyIcon(NIM_ADD, &TrayIcon);


    DWORD th;
    hThreadUpdateLocks = CreateThread(NULL, 0, ThreadUpdateLocks, NULL, 0, &th);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}