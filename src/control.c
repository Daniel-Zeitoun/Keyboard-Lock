#include "keyboardlock.h"


LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    DWORD opacity;
    static HMENU menu;
    switch (msg)
    {
        case WM_CREATE:
        {
            menu = CreateMenu();
            AppendMenu(menu, MF_STRING, ID_ABOUT, L"About");
            SetMenu(hwnd, menu);
            
            CreateWindow(L"static", L"KeyboardLock", WS_CHILD | WS_VISIBLE | BS_CENTER | SS_CENTER, 100, 10, 100, 20, hwnd, NULL, NULL, NULL);

            if (GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"CAPSOPACITY", (void*)&opacity, sizeof(opacity)) == -1)
                opacity = 0;

            CreateWindow(L"static", L"CAPS LOCK OPACITY", WS_CHILD | WS_VISIBLE | BS_CENTER | SS_CENTER, 75, 50, 150, 20, hwnd, NULL, NULL, NULL);
            hwndTrackCaps = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE, 30, 80, 220, 30, hwnd, NULL, hinst, NULL);
            SendMessage(hwndTrackCaps, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
            SendMessage(hwndTrackCaps, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)opacity);

            if (GetValueFromRegistryKey(HKEY_CURRENT_USER, DZSoftawre, L"NUMOPACITY", (void*)&opacity, sizeof(opacity)) == -1)
                opacity = 0;

            CreateWindow(L"static", L"NUM LOCK OPACITY", WS_CHILD | WS_VISIBLE | BS_CENTER | SS_CENTER, 75, 150, 150, 20, hwnd, NULL, NULL, NULL);
            hwndTrackNum = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE, 30, 180, 220, 30, hwnd, NULL, hinst, NULL);
            SendMessage(hwndTrackNum, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
            SendMessage(hwndTrackNum, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)opacity);


            CreateWindow(L"button", L"RESET", WS_CHILD | WS_VISIBLE | BS_CENTER | SS_CENTER, 45, 250, 200, 30, hwnd, ID_RESET, NULL, NULL);


            /*HWND hUpDownCaps = CreateWindowW(UPDOWN_CLASSW, NULL, WS_CHILD | WS_VISIBLE
                | UDS_SETBUDDYINT | UDS_ALIGNRIGHT,
                0, 0, 0, 0, hwnd, (HMENU)0, NULL, NULL);

            HWND hEditCaps = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDITW, NULL, WS_CHILD | BS_CENTER | SS_CENTER
                | WS_VISIBLE | ES_RIGHT, 230, 80, 50, 30, hwnd,
                (HMENU)0, NULL, NULL);


            SendMessageW(hUpDown, UDM_SETBUDDY, (WPARAM)hEdit, 0);
            SendMessageW(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(100, 0));
            SendMessageW(hUpDown, UDM_SETPOS32, 0, 0);*/

            return 0;
        }
        case WM_HSCROLL:
        {
            if (lparam == hwndTrackCaps)
            {
                opacity = SendMessage(hwndTrackCaps, TBM_GETPOS, 0, 0);
                SetLayeredWindowAttributes(hwndCapsLock, 0, 255 * opacity / 100, LWA_ALPHA);
                SetOpacity(hwndCapsLock, opacity);
            }
            else if (lparam == hwndTrackNum)
            {
                opacity = SendMessage(hwndTrackNum, TBM_GETPOS, 0, 0);
                SetLayeredWindowAttributes(hwndNumLock, 0, 255 * opacity / 100, LWA_ALPHA);
                SetOpacity(hwndNumLock, opacity);
            }
            break;
        }
        case WM_CLOSE:
        {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        case MY_WM_NOTIFYICON:
        {
            if (lparam == WM_LBUTTONUP)
            {
                ShowWindow(hwnd, SW_SHOW);
            }
            else if (lparam == WM_RBUTTONUP)
            {
                HMENU hpopup;
                POINT pos;

                hpopup = CreatePopupMenu();
                
                AppendMenu(hpopup, MF_STRING, ID_RESET, L"Réinitialiser");
                AppendMenu(hpopup, MF_STRING, ID_QUIT, L"Quitter");

                GetCursorPos(&pos);
                SetForegroundWindow(hwnd);
                TrackPopupMenuEx(hpopup, 0, pos.x, pos.y, hwnd, NULL);
                DestroyMenu(hpopup);
            }
            break;
        }
        case WM_COMMAND:
        {
            switch (wparam)
            {
                case ID_QUIT:
                {
                    if (MessageBox(hwnd, L"Voulez vous vraiment quitter KeyboardLock ?", L"KeyboardLock", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
                    {
                        TerminateThread(hThreadUpdateLocks, 0);
                        PostQuitMessage(0);
                    }
                }
                case ID_ABOUT:
                {
                    MSGBOXPARAMSW mb;
                    mb.cbSize = sizeof(mb);
                    mb.hwndOwner = hwnd;
                    mb.hInstance = hinst;
                    mb.lpszText = L"Réalisé par Daniel ZEITOUN\r\nDZSoftware";
                    mb.lpszCaption = TITLE;
                    mb.dwStyle = MB_USERICON;
                    mb.lpszIcon = MAKEINTRESOURCE(IDI_ICON1);
                    MessageBoxIndirectW(&mb);
                    break;
                }
                case ID_RESET:
                {
                    HKEY RegistryKey;
                    RegOpenKey(HKEY_CURRENT_USER, DZSoftawre, &RegistryKey);

                    if (RegistryKey == NULL)
                    {
                        RegCreateKeyEx(HKEY_CURRENT_USER, DZSoftawre, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &RegistryKey, NULL);
                    }

                    DWORD var = 80;
                    RegSetValueEx(RegistryKey, TEXT("CAPSOPACITY"), 0, REG_DWORD, (const BYTE*)&var, sizeof(LONG));
                    RegSetValueEx(RegistryKey, TEXT("NUMOPACITY"), 0, REG_DWORD, (const BYTE*)&var, sizeof(LONG));

                    var = 5;

                    RegSetValueEx(RegistryKey, TEXT("CAPSX"), 0, REG_DWORD, (const BYTE*)&var, sizeof(LONG));
                    RegSetValueEx(RegistryKey, TEXT("CAPSY"), 0, REG_DWORD, (const BYTE*)&var, sizeof(LONG));

                    RegSetValueEx(RegistryKey, TEXT("NUMX"), 0, REG_DWORD, (const BYTE*)&var, sizeof(LONG));

                    var = 30;
                    RegSetValueEx(RegistryKey, TEXT("NUMY"), 0, REG_DWORD, (const BYTE*)&var, sizeof(LONG));

                    var = 0;
                    RegSetValueEx(RegistryKey, TEXT("DEBUG"), 0, REG_DWORD, (const BYTE*)&var, sizeof(LONG));

                    SetWindowPos(hwndCapsLock, HWND_TOPMOST, 5, 5, 120, 20, SWP_SHOWWINDOW);
                    SetWindowPos(hwndNumLock, HWND_TOPMOST, 5, 30, 120, 20, SWP_SHOWWINDOW);

                    var = 80;
                    SetLayeredWindowAttributes(hwndCapsLock, 0, 255 * (BYTE)var / 100, LWA_ALPHA);
                    SetLayeredWindowAttributes(hwndNumLock, 0, 255 * (BYTE)var / 100, LWA_ALPHA);

                    SendMessage(hwndTrackCaps, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)var);
                    SendMessage(hwndTrackNum, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)var);

                    RegCloseKey(RegistryKey);
                    break;
                }
            }
            break;
        }
        case WM_CTLCOLORSTATIC:
            return CreateSolidBrush(RGB(255, 255, 255));
            break;
        
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
VOID SetOpacity(HWND window, DWORD opacity)
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
        RegSetValueEx(RegistryKey, TEXT("CAPSOPACITY"), 0, REG_DWORD, (const BYTE*)&opacity, sizeof(LONG));

        if (debug == 1)
        {
            WCHAR buffer[2048] = { 0 };
            wsprintf(buffer, L"SETOPACITY : CAPS LOCK\r\nOPACITY = %d\r\n\r\n", opacity);
            AppendWindowText(editDebug, buffer);
        }
    }
    else if (window == hwndNumLock)
    {
        RegSetValueEx(RegistryKey, TEXT("NUMOPACITY"), 0, REG_DWORD, (const BYTE*)&opacity, sizeof(LONG));

        if (debug == 1)
        {
            WCHAR buffer[2048] = { 0 };
            wsprintf(buffer, L"SETOPACITY : CAPS LOCK\r\nOPACITY = %d\r\n\r\n", opacity);
            AppendWindowText(editDebug, buffer);
        }
    }

    RegCloseKey(RegistryKey);
}