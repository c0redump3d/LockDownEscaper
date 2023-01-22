#include "Win32Hook.h"

#include <set>

/*
*
*   Win32 Running Processes Hook
*
*/
BOOL WINAPI ldeEnumProcesses(DWORD *lpidProcess, DWORD cb, DWORD *cbNeeded) {
    std::cout << "[f][win32][LDEscaper]: EnumProcesses called, will return a fake list of running processes." << std::endl;
    return NULL;
}

BOOL WINAPI ldeEnumProcessModules(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded) {
    std::cout << "[f][win32][LDEscaper]: EnumProcessModules called, will return a fake list of running process modules." << std::endl;
    return NULL;
}

/*
*
*   Win32 Terminate Process Hooks
*
*/

VOID WINAPI ldeExitProcess(UINT uExitCode) {
    std::cout << "[f][win32][LDEscaper]: ExitProcess called, but process will not close." << std::endl;
}

BOOL WINAPI ldeTerminateProcess(HANDLE hProcess, UINT uExitCode)
{
    //std::cout << "[f][win32][LDEscaper]: TerminateProcess called, will return TRUE as exit code." << std::endl;
    return TRUE;
}

/*
*
*   Win32 Monitor Hooks
*
*/

BOOL WINAPI ldeEnumDisplayMonitors(HDC hdc, LPCRECT lprcClip, MONITORENUMPROC lpfnEnum, LPARAM dwData)
{
    std::cout << "[f][win32][LDEscaper]: EnumDisplayMonitors called, will return fake monitor specifications." << std::endl;
    MONITORINFO mi = { sizeof(1) };
    mi.rcMonitor.left = 0;
    mi.rcMonitor.top = 0;
    mi.rcMonitor.right = 1280;
    mi.rcMonitor.bottom = 720;
    mi.rcWork = mi.rcMonitor;
    lpfnEnum(nullptr, hdc, &mi.rcMonitor, dwData);
    return TRUE;
}

BOOL WINAPI ldeEnumDisplaySettingsA(LPCSTR lpszDeviceName, DWORD iModeNum, DEVMODEA* lpDevMode) {
    //Change the resolution to 1280x720
    lpDevMode->dmPelsWidth = 1280;
    lpDevMode->dmPelsHeight = 720;
    return Win32Hook::ogEnumDisplaySettingsA(lpszDeviceName, iModeNum, lpDevMode);
}

/*
*
*   Win32 Debugger Hooks
*
*/
BOOL WINAPI ldeIsDebuggerPresent()
{
    return FALSE;
}

/*
*
*   Win32 Window Hooks
*
*/

LRESULT CALLBACK ldeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_SETFOCUS || message == WM_ACTIVATEAPP || message == 0x2B1)
    {
        std::cout << "[f][LDBdll][LDEscaper]: Blocked call to set focus." << std::endl;
        return 0;
    }

    return CallWindowProc(originalWndProc, hWnd, message, wParam, lParam);
}

BOOL WINAPI ldeSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if (uFlags & SWP_NOZORDER) {
         std::cout << "[f][win32][LDEscaper]: SetWindowPos called, TopMost flag disabled." << std::endl;
         // Remove the topmost flag
         hWndInsertAfter = HWND_NOTOPMOST;
    }
    //Disable the ability to hide the taskbar.
    if (hWnd == FindWindowA("Shell_TrayWnd", NULL)) {
        std::cout << "[f][win32][LDEscaper]: SetWindowPos called, ignoring call to hide taskbar." << std::endl;
        return TRUE;
    }
    return Win32Hook::ogSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// Function to check if a window belongs to a specific application
BOOL IsWindowFromApp(HWND hwnd, DWORD app_pid) {
    DWORD window_pid = 0;
    GetWindowThreadProcessId(hwnd, &window_pid);
    return window_pid == app_pid;
}

BOOL CALLBACK ldeEnumWindowsProc(HWND hwnd, LPARAM lParam) {
    return TRUE;
}

HWND WINAPI ldeGetForegroundWindow() {
    HWND hWnd = Win32Hook::ogGetForegroundWindow();
    DWORD dwProcessId;
    GetWindowThreadProcessId(hWnd, &dwProcessId);
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
    if (hProcess == NULL) {
        return hWnd;
    }
    HWND hParentWnd = NULL;
    EnumWindows(ldeEnumWindowsProc, (LPARAM)&hParentWnd);
    CloseHandle(hProcess);
    
    std::cout << "[f][win32][LDEscaper]: GetForegroundWindow called, will return parent process." << std::endl;
    return hParentWnd;
}

HWND WINAPI ldeSetActiveWindow(HWND hWnd)
{
    std::cout<<"[f][win32][LDEscaper]: SetActiveWindow called, will return NULL." << std::endl;
    return NULL;
}

BOOL WINAPI ldeShowWindow(HWND hWnd, int nCmdShow) {
    // If the window is maximized, change it to normal
    if (nCmdShow == SW_SHOWMAXIMIZED) {
        std::cout << "[f][win32][LDEscaper]: ShowWindow called, will return SW_SHOWNORMAL." << std::endl;
        // Change to SW_SHOWNORMAL instead of minimizing the window
        nCmdShow = SW_SHOWNORMAL;
    }
    //Disable the ability to hide the taskbar.
    if (hWnd == FindWindowA("Shell_TrayWnd", NULL)) {
        std::cout << "[f][win32][LDEscaper]: ShowWindow called, blocking request to hide taskbar." << std::endl;
        return TRUE;
    }
    
    return Win32Hook::ogShowWindow(hWnd, nCmdShow);
}

LONG WINAPI ldeSetWindowLongW(HWND hWnd, int nIndex, LONG dwNewLong) {
    if (nIndex == GWL_STYLE) {
        if (dwNewLong & WS_EX_TOPMOST) {
            dwNewLong &= ~WS_EX_TOPMOST; // remove the fullscreen flag
            std::cout << "[f][win32][LDEscaper]: SetWindowLongW called, will remove TopMost flag." << std::endl;
        }
    }
    return Win32Hook::ogSetWindowLongW(hWnd, nIndex, dwNewLong);
}

LONG WINAPI ldeSetWindowLong(HWND hWnd, int nIndex, LONG dwNewLong) {
    if (nIndex == GWL_STYLE) {
        //TODO: Remove?
    }
    
    return Win32Hook::ogSetWindowLong(hWnd, nIndex, dwNewLong);
}

HWND WINAPI ldeSetFocus(HWND hWnd) {
    return NULL;
}


/*
 *
 *  Win32 Keyboard Hooks
 *
 */
SHORT WINAPI ldeGetAsyncKeyState(int vKey)
{
    std::cout << "[f][win32][LDEscaper]: GetAsyncKeyState called, will return 0." << std::endl;
    return NULL;
}

/*
 *
 *  Win32 Windows Hooks
 *
 */

HHOOK WINAPI ldeSetWindowsHookExA(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) {
    std::cout << "[f][win32][LDEscaper]: SetWindowsHookExA called, will return NULL." << std::endl;
    // return NULL to prevent the application from setting any Windows hooks
    return NULL;
}

/*
 *
 *  Win32 Registry Hooks
 *
 */

LONG WINAPI ldeRegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData) {
    std::cout << "[f][win32][LDEscaper]: RegSetValueExA called, will return ERROR_SUCCESS." << std::endl;
    return ERROR_SUCCESS;
}

LONG WINAPI ldeRegDeleteKeyExA(HKEY hKey, LPCSTR lpSubKey, REGSAM samDesired, DWORD Reserved) {
    std::cout << "[f][win32][LDEscaper]: RegDeleteKeyExA called, will return ERROR_SUCCESS." << std::endl;
    return ERROR_SUCCESS;
}

LONG WINAPI ldeRegDeleteValueA(HKEY hKey, LPCSTR lpValueName) {
    std::cout << "[f][win32][LDEscaper]: RegDeleteValueA called, will return ERROR_SUCCESS." << std::endl;
    return ERROR_SUCCESS;
}

/*
*
*   LDEscaper functions
*
*/

DWORD GetProcessIdByName(LPCTSTR processName)
{
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 processInfo;
        processInfo.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &processInfo))
        {
            do
            {
                if (_tcscmp(processInfo.szExeFile, processName) == 0)
                {
                    processId = processInfo.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &processInfo));
        }
        CloseHandle(hSnapshot);
    }
    return processId;
}


BOOL Win32Hook::attachHooks()
{
    std::cout << "[f][win32][LDEscaper]: Attaching to API hooks..." << std::endl;
    DetourAttach(&reinterpret_cast<PVOID&>(ogEnumProcesses), ldeEnumProcesses);
    DetourAttach(&reinterpret_cast<PVOID&>(ogEnumProcessModules), ldeEnumProcessModules);
    DetourAttach(&reinterpret_cast<PVOID&>(ogExitProcess), ldeExitProcess);
    DetourAttach(&reinterpret_cast<PVOID&>(ogTerminateProcess), ldeTerminateProcess);
    DetourAttach(&reinterpret_cast<PVOID&>(ogEnumDisplayMonitors), ldeEnumDisplayMonitors);
    DetourAttach(&reinterpret_cast<PVOID&>(ogEnumDisplaySettingsA), ldeEnumDisplaySettingsA);
    DetourAttach(&reinterpret_cast<PVOID&>(ogIsDebuggerPresent), ldeIsDebuggerPresent);
    DetourAttach(&reinterpret_cast<PVOID&>(ogSetWindowPos), ldeSetWindowPos);
    DetourAttach(&reinterpret_cast<PVOID&>(ogGetForegroundWindow), ldeGetForegroundWindow);
    DetourAttach(&reinterpret_cast<PVOID&>(ogSetActiveWindow), ldeSetActiveWindow);
    DetourAttach(&reinterpret_cast<PVOID&>(ogShowWindow), ldeShowWindow);
    DetourAttach(&reinterpret_cast<PVOID&>(ogSetWindowLong), ldeSetWindowLong);
    DetourAttach(&reinterpret_cast<PVOID&>(ogSetWindowLongW), ldeSetWindowLongW);
    DetourAttach(&reinterpret_cast<PVOID&>(ogGetAsyncKeyState), ldeGetAsyncKeyState);
    DetourAttach(&reinterpret_cast<PVOID&>(ogSetWindowsHookExA), ldeSetWindowsHookExA);
    DetourAttach(&reinterpret_cast<PVOID&>(ogRegSetValueExA), ldeRegSetValueExA);
    DetourAttach(&reinterpret_cast<PVOID&>(ogRegDeleteKeyExA), ldeRegDeleteKeyExA);
    DetourAttach(&reinterpret_cast<PVOID&>(ogRegDeleteValueA), ldeRegDeleteValueA);
    DetourAttach(&reinterpret_cast<PVOID&>(ogSetFocus), ldeSetFocus);

    std::cout << "[f][win32][LDEscaper]: Finished attaching to API hooks!" << std::endl;
    
    // Enumerate all top-level windows and set the parent window as their parent
    //DetourAttach(&reinterpret_cast<PVOID&>(ogWndProc), ldeWndproc);
    return TRUE;
}

BOOL Win32Hook::detachHooks()
{
    std::cout << "[f][win32][LDEscaper]: Detaching from API hooks..." << std::endl;
    DetourDetach(&reinterpret_cast<PVOID&>(ogEnumProcesses), ldeEnumProcesses);
    DetourDetach(&reinterpret_cast<PVOID&>(ogEnumProcessModules), ldeEnumProcessModules);
    DetourDetach(&reinterpret_cast<PVOID&>(ogExitProcess), ldeExitProcess);
    DetourDetach(&reinterpret_cast<PVOID&>(ogTerminateProcess), ldeTerminateProcess);
    DetourDetach(&reinterpret_cast<PVOID&>(ogEnumDisplayMonitors), ldeEnumDisplayMonitors);
    DetourDetach(&reinterpret_cast<PVOID&>(ogEnumDisplaySettingsA), ldeEnumDisplaySettingsA);
    DetourDetach(&reinterpret_cast<PVOID&>(ogIsDebuggerPresent), ldeIsDebuggerPresent);
    DetourDetach(&reinterpret_cast<PVOID&>(ogSetWindowPos), ldeSetWindowPos);
    DetourDetach(&reinterpret_cast<PVOID&>(ogGetForegroundWindow), ldeGetForegroundWindow);
    DetourDetach(&reinterpret_cast<PVOID&>(ogSetActiveWindow), ldeSetActiveWindow);
    DetourDetach(&reinterpret_cast<PVOID&>(ogShowWindow), ldeShowWindow);
    DetourDetach(&reinterpret_cast<PVOID&>(ogSetWindowLong), ldeSetWindowLong);
    DetourDetach(&reinterpret_cast<PVOID&>(ogSetWindowLongW), ldeSetWindowLongW);
    DetourDetach(&reinterpret_cast<PVOID&>(ogGetAsyncKeyState), ldeGetAsyncKeyState);
    DetourDetach(&reinterpret_cast<PVOID&>(ogSetWindowsHookExA), ldeSetWindowsHookExA);
    DetourDetach(&reinterpret_cast<PVOID&>(ogRegSetValueExA), ldeRegSetValueExA);
    DetourDetach(&reinterpret_cast<PVOID&>(ogRegDeleteKeyExA), ldeRegDeleteKeyExA);
    DetourDetach(&reinterpret_cast<PVOID&>(ogRegDeleteValueA), ldeRegDeleteValueA);
    DetourDetach(&reinterpret_cast<PVOID&>(ogSetFocus), ldeSetFocus);
    //DetourDetach(&reinterpret_cast<PVOID&>(ogWndProc), ldeWndproc);
    return TRUE;
}
