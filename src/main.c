#include "keyboardlock.h"

/************************************************************************************************************/
LockButton caps = { 0 };
LockButton num = { 0 };
ControlPanel controlPanel = { 0 };
/************************************************************************************************************/
INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
	/**************************/
	// Initialization
	InitCommonControls();
	
	/**************************/
	// Definition of classControlPanel
	WNDCLASS classControlPanel = { 0 };
	classControlPanel.lpszClassName = TEXT("classControlPanel");
	classControlPanel.lpfnWndProc = ControlPanelWindowProc;
	classControlPanel.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	classControlPanel.hCursor = LoadCursor(NULL, IDC_ARROW);
	classControlPanel.hInstance = hInstance;
	if (!RegisterClass(&classControlPanel))
		return EXIT_FAILURE;

	/**************************/
	// Create control panel window
	HWND hControlPanel = CreateWindow(classControlPanel.lpszClassName, TITLE, WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 400, NULL, NULL, NULL, NULL);
	if (hControlPanel == NULL)
		return EXIT_FAILURE;

	/**************************/
	// Add notify icon
	controlPanel.notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	controlPanel.notifyIconData.hWnd = hControlPanel;
	controlPanel.notifyIconData.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	controlPanel.notifyIconData.uCallbackMessage = MY_WM_NOTIFYICON;
	controlPanel.notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	CopyMemory(controlPanel.notifyIconData.szTip, TEXT("KeyboardLock Control Panel"), sizeof(TEXT("KeyboardLock Control Panel")));
	Shell_NotifyIcon(NIM_ADD, &controlPanel.notifyIconData);
	
	/**************************/
	// Definition of classLock
	WNDCLASS classLock = { 0 };
	classLock.lpszClassName = TEXT("classLock");
	classLock.lpfnWndProc = LockWindowProc;
	classLock.hInstance = hInstance;
	classLock.hCursor = LoadCursor(NULL, IDC_HAND);
	if (!RegisterClass(&classLock))
		return EXIT_FAILURE;


	InitializeCriticalSection(&caps.lock);
	InitializeCriticalSection(&num.lock);

	/**************************/
	// Retrieve configuration from registry
	if (!ReadConfig())
	{
		// Or set default config
		caps.position.x = 5;
		caps.position.y = 5;
		caps.opacity = 80;

		num.position.x = 5;
		num.position.y = 30;
		num.opacity = 80;
	}

	/**************************/
	// Create CAPS window
	if (!(caps.hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST, classLock.lpszClassName, NULL, WS_VISIBLE | WS_POPUPWINDOW, caps.position.x, caps.position.y, 120, 20, NULL, NULL, NULL, &caps)))
		return EXIT_FAILURE;

	// Set CAPS opacity
	if (!SetLayeredWindowAttributes(caps.hWnd, 0, 255 * (BYTE)caps.opacity / 100, LWA_ALPHA))
		return EXIT_FAILURE;

	caps.virtualKey = VK_CAPITAL;

	/**************************/
	// Create NUM window
	if (!(num.hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST, classLock.lpszClassName, NULL, WS_VISIBLE | WS_POPUPWINDOW, num.position.x, num.position.y, 120, 20, NULL, NULL, NULL, &num)))
		return EXIT_FAILURE;

	// Set NUM opacity
	if (!SetLayeredWindowAttributes(num.hWnd, 0, 255 * (BYTE)num.opacity / 100, LWA_ALPHA))
		return EXIT_FAILURE;

	num.virtualKey = VK_NUMLOCK;

	/**************************/
	// Write config in registry
	WritePosition(caps.hWnd);
	WritePosition(num.hWnd);
	WriteOpacity(caps.hWnd, caps.opacity);
	WriteOpacity(num.hWnd, num.opacity);

	/**************************/
	// First check on locks
	UpdateLock(&caps, TRUE);
	UpdateLock(&num, TRUE);

	/**************************/
	// Install hook on keyboard
	if (!SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0))
		return EXIT_FAILURE;

	// Install hook on mouse
	if (!SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0))
		return EXIT_FAILURE;

	CreateThread(NULL, 0, UpdateLocksThreadProc, NULL, 0, NULL);

	/**************************/
	// Event loop
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteCriticalSection(&num.lock);
	DeleteCriticalSection(&caps.lock);

	return 0;
}
/************************************************************************************************************/