#include "keyboardlock.h"

/************************************************************************************************************/
LRESULT CALLBACK LockWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static RECT rect = { 0 };
	static BOOL mouseClicked = FALSE;
	static POINT lastLocation = { 0 };

	switch (uMsg)
	{
	case WM_CLOSE:
		return 0;
	case WM_CREATE:
	{
		HFONT hFont = CreateFont(18, 0, 0, 0, FW_BLACK, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Arial Black"));

		if (!caps.hWnd)
		{
			caps.hStatic = CreateWindow(TEXT("static"), NULL, WS_VISIBLE | WS_CHILD, 0, 0, 120, 20, hWnd, NULL, NULL, NULL);
			SendMessage(caps.hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
		}
		else if (!num.hWnd)
		{
			num.hStatic = CreateWindow(TEXT("static"), NULL, WS_VISIBLE | WS_CHILD, 0, 0, 120, 20, hWnd, NULL, NULL, NULL);
			SendMessage(num.hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
		}
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hWnd, &ps);

		if (hWnd == caps.hWnd)
		{
			if (caps.state == ON)
			{
				SetWindowText(caps.hStatic, TEXT("CAPS LOCK ON"));
				FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB_GREEN));
			}
			else
			{
				SetWindowText(caps.hStatic, TEXT("CAPS LOCK OFF"));
				FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB_RED));
			}
		}
		else if (hWnd == num.hWnd)
		{
			if (num.state == ON)
			{
				SetWindowText(num.hStatic, TEXT("NUM  LOCK ON"));
				FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB_GREEN));
			}
			else
			{
				SetWindowText(num.hStatic, TEXT("NUM  LOCK OFF"));
				FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB_RED));
			}
		}

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB_WHITE);
		SetTextColor(hdcStatic, RGB_BLACK);

		if (hWnd == caps.hWnd)
		{
			if (caps.state == ON)
			{
				SetTextColor(hdcStatic, RGB_BLACK);
				SetBkColor(hdcStatic, RGB_GREEN);
				return (LRESULT)CreateSolidBrush(RGB_GREEN);
			}
			else
			{
				SetBkColor(hdcStatic, RGB_RED);
				return (LRESULT)CreateSolidBrush(RGB_RED);
			}
		}
		else if (hWnd == num.hWnd)
		{
			if (num.state == ON)
			{
				SetTextColor(hdcStatic, RGB_BLACK);
				SetBkColor(hdcStatic, RGB_GREEN);
				return (LRESULT)CreateSolidBrush(RGB_GREEN);
			}
			else
			{
				SetBkColor(hdcStatic, RGB_RED);
				return (LRESULT)CreateSolidBrush(RGB_RED);
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SetCapture(hWnd);
		mouseClicked = TRUE;
		GetCursorPos(&lastLocation);

		GetWindowRect(hWnd, &rect);
		lastLocation.x = lastLocation.x - rect.left;
		lastLocation.y = lastLocation.y - rect.top;
		break;
	}
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		mouseClicked = FALSE;
		WritePosition(hWnd);
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (mouseClicked)
		{
			if (hWnd == caps.hWnd)
			{
				POINT currentPosition;
				GetCursorPos(&currentPosition);

				caps.position.x = currentPosition.x - lastLocation.x;
				caps.position.y = currentPosition.y - lastLocation.y;
				MoveWindow(hWnd, caps.position.x, caps.position.y, rect.right - rect.left, rect.bottom - rect.top, FALSE);
			}
			else if (hWnd == num.hWnd)
			{
				POINT currentPosition;
				GetCursorPos(&currentPosition);

				num.position.x = currentPosition.x - lastLocation.x;
				num.position.y = currentPosition.y - lastLocation.y;
				MoveWindow(hWnd, num.position.x, num.position.y, rect.right - rect.left, rect.bottom - rect.top, FALSE);
			}
		}
		break;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
/************************************************************************************************************/
VOID ShowLock(LockButton* lockButton)
{
	RECT rect = { 0 };
	GetWindowRect(lockButton->hWnd, &rect);

	if (lockButton->state == ON)
		SetWindowPos(lockButton->hWnd, HWND_TOPMOST, 0, 0, 112, 20, SWP_NOMOVE);

	else if (lockButton->state == OFF)
		SetWindowPos(lockButton->hWnd, HWND_TOPMOST, 0, 0, 120, 20, SWP_NOMOVE);

	RedrawWindow(lockButton->hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
}
/************************************************************************************************************/
VOID UpdateLock(LockButton* lockButton, BOOL reverse)
{
	if (TryEnterCriticalSection(&lockButton->lock) == 0)
		return;

	SHORT state = 0;

	if (reverse)
	{
		// thread
		state = GetKeyState(lockButton->virtualKey);
		if (state >= 0)
			state = !state;
	}
	else
	{
		// button
		state = GetKeyState(lockButton->virtualKey);
	}

	if (state == 1 && lockButton->state != OFF)
	{
		lockButton->state = OFF;
		ShowLock(lockButton);
	}
	else if (state == 0 && lockButton->state != ON)
	{
		lockButton->state = ON;
		ShowLock(lockButton);
	}

	LeaveCriticalSection(&lockButton->lock);
}
/************************************************************************************************************/
DWORD CALLBACK UpdateLocksThreadProc(LPVOID param)
{
	SHORT state = 0;
	while (1)
	{
		UpdateLock(&caps, TRUE);
		UpdateLock(&num, TRUE);
		Sleep(50);
	}
	ExitThread(0);
}
/************************************************************************************************************/