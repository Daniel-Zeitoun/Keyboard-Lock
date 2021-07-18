#include "keyboardlock.h"

/************************************************************************************************************/
LRESULT CALLBACK ControlPanelWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		DWORD opacity;
		HMENU hMenu = CreateMenu();
		AppendMenu(hMenu, MF_STRING, ID_ABOUT, TEXT("About"));
		SetMenu(hWnd, hMenu);

		CreateWindow(TEXT("static"), TEXT("KeyboardLock"), WS_CHILD | WS_VISIBLE | BS_CENTER | SS_CENTER, 100, 10, 100, 20, hWnd, NULL, NULL, NULL);

		if (GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("CAPSOPACITY"), (void*)&opacity, sizeof(opacity)) == -1)
			opacity = 0;

		CreateWindow(TEXT("static"), TEXT("CAPS LOCK OPACITY"), WS_CHILD | WS_VISIBLE | BS_CENTER | SS_CENTER, 75, 50, 150, 20, hWnd, NULL, NULL, NULL);
		controlPanel.hTrackCaps = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE, 30, 80, 220, 30, hWnd, NULL, NULL, NULL);
		SendMessage(controlPanel.hTrackCaps, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SendMessage(controlPanel.hTrackCaps, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)opacity);

		if (GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("NUMOPACITY"), (void*)&opacity, sizeof(opacity)) == -1)
			opacity = 0;

		CreateWindow(TEXT("static"), TEXT("NUM LOCK OPACITY"), WS_CHILD | WS_VISIBLE | BS_CENTER | SS_CENTER, 75, 150, 150, 20, hWnd, NULL, NULL, NULL);
		controlPanel.hTrackNum = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE, 30, 180, 220, 30, hWnd, NULL, NULL, NULL);
		SendMessage(controlPanel.hTrackNum, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SendMessage(controlPanel.hTrackNum, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)opacity);

		CreateWindow(TEXT("button"), TEXT("RESET"), WS_CHILD | WS_VISIBLE | BS_CENTER | SS_CENTER, 45, 250, 200, 30, hWnd, (HMENU)ID_RESET, NULL, NULL);
		break;

	}
	case WM_HSCROLL:
	{
		DWORD opacity;
		if ((HWND)lParam == controlPanel.hTrackCaps)
		{
			opacity = (DWORD)SendMessage(controlPanel.hTrackCaps, TBM_GETPOS, 0, 0);
			SetLayeredWindowAttributes(caps.hWnd, 0, 255 * (BYTE)opacity / 100, LWA_ALPHA);
			WriteOpacity(caps.hWnd, opacity);
		}
		else if ((HWND)lParam == controlPanel.hTrackNum)
		{
			opacity = (DWORD)SendMessage(controlPanel.hTrackNum, TBM_GETPOS, 0, 0);
			SetLayeredWindowAttributes(num.hWnd, 0, 255 * (BYTE)opacity / 100, LWA_ALPHA);
			WriteOpacity(num.hWnd, opacity);
		}
		break;
	}
	case WM_CLOSE:
	{
		ShowWindow(hWnd, SW_HIDE);
		return 0;
	}
	case MY_WM_NOTIFYICON:
	{
		if (lParam == WM_LBUTTONUP)
		{
			ShowWindow(hWnd, SW_SHOW);
			SetForegroundWindow(hWnd);
		}
		else if (lParam == WM_RBUTTONUP)
		{
			HMENU hpopup;
			POINT pos;

			hpopup = CreatePopupMenu();

			AppendMenu(hpopup, MF_STRING, ID_RESET, TEXT("Reset"));
			AppendMenu(hpopup, MF_STRING, ID_QUIT, TEXT("Quit"));

			GetCursorPos(&pos);
			SetForegroundWindow(hWnd);
			TrackPopupMenuEx(hpopup, 0, pos.x, pos.y, hWnd, NULL);
			DestroyMenu(hpopup);
		}
		break;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case ID_QUIT:
		{
			if (MessageBox(hWnd, TEXT("Do you really want to quit KeyboardLock ?"), TEXT("KeyboardLock"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			{
				//TerminateThread(hThreadUpdateLocks, 0);
				Shell_NotifyIcon(NIM_DELETE, &controlPanel.notifyIconData);
				PostQuitMessage(0);
			}
			break;
		}
		case ID_ABOUT:
		{
			MSGBOXPARAMS mb;
			mb.cbSize = sizeof(mb);
			mb.hwndOwner = hWnd;
			mb.hInstance = GetModuleHandle(NULL);
			mb.lpszText = TEXT("Developped by DZSoftware\t\r\n\r\nwww.danielzeitoun.fr\t");
			mb.lpszCaption = TITLE;
			mb.dwStyle = MB_USERICON;
			mb.lpszIcon = MAKEINTRESOURCE(IDI_ICON1);
			MessageBoxIndirect(&mb);
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
			RegSetValueEx(RegistryKey, TEXT("CAPSOPACITY"), 0, REG_DWORD, (const BYTE*)&var, sizeof(DWORD));
			RegSetValueEx(RegistryKey, TEXT("NUMOPACITY"), 0, REG_DWORD, (const BYTE*)&var, sizeof(DWORD));

			var = 5;

			RegSetValueEx(RegistryKey, TEXT("CAPSX"), 0, REG_DWORD, (const BYTE*)&var, sizeof(DWORD));
			RegSetValueEx(RegistryKey, TEXT("CAPSY"), 0, REG_DWORD, (const BYTE*)&var, sizeof(DWORD));

			RegSetValueEx(RegistryKey, TEXT("NUMX"), 0, REG_DWORD, (const BYTE*)&var, sizeof(DWORD));

			var = 30;
			RegSetValueEx(RegistryKey, TEXT("NUMY"), 0, REG_DWORD, (const BYTE*)&var, sizeof(DWORD));

			var = 0;
			RegSetValueEx(RegistryKey, TEXT("DEBUG"), 0, REG_DWORD, (const BYTE*)&var, sizeof(DWORD));

			SetWindowPos(caps.hWnd, HWND_TOPMOST, 5, 5, 120, 20, SWP_SHOWWINDOW);
			SetWindowPos(num.hWnd, HWND_TOPMOST, 5, 30, 120, 20, SWP_SHOWWINDOW);

			var = 80;
			SetLayeredWindowAttributes(caps.hWnd, 0, 255 * (BYTE)var / 100, LWA_ALPHA);
			SetLayeredWindowAttributes(num.hWnd, 0, 255 * (BYTE)var / 100, LWA_ALPHA);

			SendMessage(controlPanel.hTrackCaps, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)var);
			SendMessage(controlPanel.hTrackNum, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)var);

			RegCloseKey(RegistryKey);
			break;
		}
		}
		break;
	}
	case WM_CTLCOLORSTATIC:
		return (LRESULT)CreateSolidBrush(RGB(255, 255, 255));

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
/************************************************************************************************************/
