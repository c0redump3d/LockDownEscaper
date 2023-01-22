#pragma once

#include "LDBHook.h"
#include <detours.h>
#include <WinUser.h>
#include <tchar.h>

class Win32Hook
{

public:

    static BOOL attachHooks();
    static BOOL detachHooks();
    
    /*
     *
     *  WIN32 API HOOKS
     *
     */
    //Running Processes Hook
    inline static BOOL(WINAPI *ogEnumProcesses)(DWORD *lpidProcess, DWORD cb, DWORD *cbNeeded) = EnumProcesses;
    inline static BOOL(WINAPI *ogEnumProcessModules)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded) = EnumProcessModules;
    //Terminate Process Hooks
    inline static VOID(WINAPI *ogExitProcess)(UINT uExitCode) = ExitProcess;
    inline static BOOL(WINAPI *ogTerminateProcess)(HANDLE hProcess, UINT   uExitCode) = TerminateProcess;
    //Monitor Hooks
    inline static BOOL (WINAPI *ogEnumDisplayMonitors)(HDC hdc, LPCRECT lprcClip, MONITORENUMPROC lpfnEnum, LPARAM dwData) = EnumDisplayMonitors;
    inline static BOOL(WINAPI* ogEnumDisplaySettingsA)(LPCSTR lpszDeviceName, DWORD iModeNum, DEVMODEA* lpDevMode) = EnumDisplaySettingsA;
    //Debugger Detection
    inline static BOOL(WINAPI *ogIsDebuggerPresent)() = IsDebuggerPresent;
    //Window Hooks
    inline static BOOL(WINAPI *ogSetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) = SetWindowPos;
    inline static HWND (WINAPI *ogGetForegroundWindow)() = GetForegroundWindow;
    inline static HWND (WINAPI *ogSetActiveWindow)(HWND hWnd) = SetActiveWindow;
    inline static BOOL(WINAPI *ogShowWindow)(HWND hWnd, int nCmdShow) = ShowWindow;
    inline static LONG (WINAPI *ogSetWindowLongW)(HWND hWnd, int nIndex, LONG dwNewLong) = SetWindowLongW;
    inline static LONG(WINAPI* ogSetWindowLong)(HWND hWnd, int nIndex, LONG dwNewLong) = SetWindowLong;
    inline static HWND (WINAPI *ogSetFocus)(HWND hWnd) = SetFocus;
    
    //Keyboard Hooks
    inline static SHORT (WINAPI *ogGetAsyncKeyState)(int vKey) = GetAsyncKeyState;
    //Detection hooks
    inline static HHOOK(WINAPI *ogSetWindowsHookExA)(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) = SetWindowsHookExA;
    //Registry Hooks
    inline static LONG (WINAPI *ogRegSetValueExA)(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData) = RegSetValueExA;
    inline static LONG (WINAPI *ogRegDeleteKeyExA)(HKEY hKey, LPCSTR lpSubKey, REGSAM samDesired, DWORD Reserved) = RegDeleteKeyExA;
    inline static LONG (WINAPI *ogRegDeleteValueA)(HKEY hKey, LPCSTR lpValueName) = RegDeleteValueA;

};
