#define DEBUG

#include <stdio.h>
#include <windows.h>
#include <CommCtrl.h>
#include <wchar.h>
#include "../res/resource.h"

#pragma comment(lib, "Comctl32")


#pragma warning(disable : 4996)

#define RGB_BLACK RGB(0, 0, 0)
#define RGB_WHITE RGB(255, 255, 255)
#define RGB_GREEN RGB(0, 255, 0)
#define RGB_RED   RGB(255, 0, 0)

#define TITLE L"KeyboardLock 1.5.2"
#define DZSoftawre L"SOFTWARE\\DZSoftware\\KeyboardLock"
#define CAPS 1
#define NUM  2
#define MY_WM_NOTIFYICON WM_USER+1
#define ID_QUIT 1
#define ID_ABOUT 2
#define ID_RESET 3
/*********************************************************************************************************/
HWND hwndCapsLock;
HWND hwndNumLock;
HWND hwndDebug;
HWND editDebug;
HWND hwndControl;
HWND hwndTrayIcon;
HWND hwndTrackCaps;
HWND hwndTrackNum;

int capsLockStatus;
int numLockStatus;
DWORD debug;

NOTIFYICONDATA TrayIcon;
HINSTANCE hinst;
HANDLE hThreadUpdateLocks;

CRITICAL_SECTION numMutex;
CRITICAL_SECTION capsMutex;

void AppendWindowText(HWND hWnd, LPWSTR lpString);
INT GetValueFromRegistryKey(HKEY hKey, LPWSTR registryKeyName, LPWSTR registryValueName, VOID* pValue, DWORD sizeValue);
VOID SetPosition(HWND window);
LRESULT CALLBACK DebugProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
VOID PrintCapsLock(SHORT state);
VOID PrintNumLock(SHORT state);
LRESULT WINAPI HookProc(int code, WPARAM wParam, LPARAM lParam);
TIMERPROC WINAPI UpdateLocks(VOID);
INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow);
LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
VOID SetOpacity(HWND window);