#pragma once

/*********************************************************************************************************/
#include <windows.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"
/*********************************************************************************************************/
#pragma comment(lib, "Comctl32")
/*********************************************************************************************************/
#define RGB_BLACK RGB(0, 0, 0)
#define RGB_WHITE RGB(255, 255, 255)
#define RGB_GREEN RGB(0, 255, 0)
#define RGB_RED   RGB(255, 0, 0)

#define TITLE TEXT("KeyboardLock 2.1.0")
#define DZSoftawre TEXT("SOFTWARE\\DZSoftware\\KeyboardLock")
#define MY_WM_NOTIFYICON WM_USER +1
#define ID_QUIT 1
#define ID_ABOUT 2
#define ID_RESET 3
/*********************************************************************************************************/
typedef struct ControlPanel
{
	HWND hwnd;
	NOTIFYICONDATA notifyIconData;
	HWND hTrayIcon;
	HWND hTrackCaps;
	HWND hTrackNum;
} ControlPanel;
/************************************************************************************************************/
typedef enum State
{
	ON,
	OFF
} State;
/************************************************************************************************************/
typedef struct LockButton
{
	HWND hWnd;
	HWND hStatic;
	State state;
	POINT position;
	DWORD opacity;
	DWORD virtualKey;
} LockButton;

extern LockButton caps;
extern LockButton num;
extern ControlPanel controlPanel;

// controlpanel.c
LRESULT CALLBACK ControlPanelWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// hooks.c
BOOL MouseIsInsideWindow(HWND hWnd);
LRESULT CALLBACK MouseHookProc(INT code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardHookProc(INT code, WPARAM wParam, LPARAM lParam);

// lock.c
LRESULT CALLBACK LockWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID ShowLock(LockButton* lockButton);
VOID UpdateLock(LockButton* lockButton, BOOL reverse);
DWORD CALLBACK UpdateLocksThreadProc(LPVOID param);

// main.c
INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow);

// registry.c
BOOL GetRegistryKeyValue(HKEY hKey, LPCTSTR keyName, LPCTSTR valueName, PVOID value, DWORD valueSize);
BOOL ReadConfig();
BOOL WritePosition(HWND hWnd);
BOOL WriteOpacity(HWND hWnd, DWORD opacity);
