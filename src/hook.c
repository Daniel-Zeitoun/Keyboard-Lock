#include "keyboardlock.h"

/************************************************************************************************************/
BOOL MouseIsInsideWindow(HWND hWnd)
{
	POINT cursor = { 0 };
	if (!GetCursorPos(&cursor))
		return FALSE;

	RECT rect = { 0 };
	if (!GetWindowRect(hWnd, &rect))
		return FALSE;

	// If mouse is inside window
	if (cursor.y >= rect.top && cursor.y < rect.bottom && cursor.x >= rect.left && cursor.x < rect.right)
		return TRUE;

	return FALSE;
}
/************************************************************************************************************/
LRESULT CALLBACK MouseHookProc(INT code, WPARAM wParam, LPARAM lParam)
{
	SHORT lControl = GetKeyState(VK_LCONTROL);
	SHORT rControl = GetKeyState(VK_RCONTROL);

	// If CONTROL button is pressed
	if (lControl == 0 || lControl == 1 && rControl == 0 || rControl == 1)
	{
		if (MouseIsInsideWindow(caps.hWnd))
			ShowWindow(caps.hWnd, FALSE);
		else
			ShowWindow(caps.hWnd, TRUE);

		if (MouseIsInsideWindow(num.hWnd))
			ShowWindow(num.hWnd, FALSE);
		else
			ShowWindow(num.hWnd, TRUE);
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}
/************************************************************************************************************/
LRESULT CALLBACK KeyboardHookProc(INT code, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_KEYDOWN)
	{
		switch (((LPKBDLLHOOKSTRUCT)lParam)->vkCode)
		{
		case VK_CAPITAL:
			UpdateLock(&caps, FALSE);
			break;

		case VK_NUMLOCK:
			UpdateLock(&num, FALSE);
			break;

		case VK_LCONTROL:
		case VK_RCONTROL:
			ShowWindow(caps.hWnd, TRUE);
			ShowWindow(num.hWnd, TRUE);
			break;
		}
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}
/************************************************************************************************************/