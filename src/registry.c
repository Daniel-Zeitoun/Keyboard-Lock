#include "keyboardlock.h"

/************************************************************************************************************/
BOOL GetRegistryKeyValue(HKEY hKey, LPCTSTR keyName, LPCTSTR valueName, PVOID value, DWORD valueSize)
{
	HKEY registryKey = NULL;
	if (RegOpenKeyEx(hKey, keyName, 0, KEY_READ, &registryKey) != ERROR_SUCCESS)
		return FALSE;

	if (RegGetValue(registryKey, NULL, valueName, RRF_RT_ANY, NULL, value, &valueSize) != ERROR_SUCCESS)
		return FALSE;

	if (RegCloseKey(registryKey) != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}
/************************************************************************************************************/
BOOL ReadConfig()
{
	if (!GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("CAPSX"), &caps.position.x, sizeof(DWORD)))
		return FALSE;

	if (!GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("CAPSY"), &caps.position.y, sizeof(DWORD)))
		return FALSE;

	if (!GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("CAPSOPACITY"), &caps.opacity, sizeof(DWORD)))
		return FALSE;

	if (!GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("NUMX"), &num.position.x, sizeof(DWORD)))
		return FALSE;

	if (!GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("NUMY"), &num.position.y, sizeof(DWORD)))
		return FALSE;

	if (!GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("NUMY"), &num.position.y, sizeof(DWORD)))
		return FALSE;

	if (!GetRegistryKeyValue(HKEY_CURRENT_USER, DZSoftawre, TEXT("NUMOPACITY"), &num.opacity, sizeof(DWORD)))
		return FALSE;

	return TRUE;
}
/************************************************************************************************************/
BOOL WritePosition(HWND hWnd)
{
	RECT rect;
	HKEY registryKey;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, DZSoftawre, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &registryKey, NULL) != ERROR_SUCCESS)
		return FALSE;

	if (!GetWindowRect(hWnd, &rect))
		return FALSE;

	if (hWnd == caps.hWnd)
	{
		if (RegSetValueEx(registryKey, TEXT("CAPSX"), 0, REG_DWORD, (const BYTE*)&rect.left, sizeof(DWORD)) != ERROR_SUCCESS)
			return FALSE;
		if (RegSetValueEx(registryKey, TEXT("CAPSY"), 0, REG_DWORD, (const BYTE*)&rect.top, sizeof(DWORD)) != ERROR_SUCCESS)
			return FALSE;
	}
	else if (hWnd == num.hWnd)
	{
		if (RegSetValueEx(registryKey, TEXT("NUMX"), 0, REG_DWORD, (const BYTE*)&rect.left, sizeof(DWORD)) != ERROR_SUCCESS)
			return FALSE;
		if (RegSetValueEx(registryKey, TEXT("NUMY"), 0, REG_DWORD, (const BYTE*)&rect.top, sizeof(DWORD)) != ERROR_SUCCESS)
			return FALSE;
	}

	if (RegCloseKey(registryKey) != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}
/************************************************************************************************************/
BOOL WriteOpacity(HWND hWnd, DWORD opacity)
{
	RECT rect;
	HKEY registryKey;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, DZSoftawre, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &registryKey, NULL) != ERROR_SUCCESS)
		return FALSE;

	if (!GetWindowRect(hWnd, &rect))
		return FALSE;

	if (hWnd == caps.hWnd)
	{
		if (RegSetValueEx(registryKey, TEXT("CAPSOPACITY"), 0, REG_DWORD, (const BYTE*)&opacity, sizeof(DWORD)) != ERROR_SUCCESS)
			return FALSE;
	}
	else if (hWnd == num.hWnd)
	{
		if (RegSetValueEx(registryKey, TEXT("NUMOPACITY"), 0, REG_DWORD, (const BYTE*)&opacity, sizeof(DWORD)) != ERROR_SUCCESS)
			return FALSE;
	}

	if (RegCloseKey(registryKey) != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}
/************************************************************************************************************/