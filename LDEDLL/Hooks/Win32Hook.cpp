#include "Win32Hook.h"

#include <set>

/*
*
*   Win32 Running Processes Hook
*
*/
BOOL WINAPI ldeEnumProcesses(DWORD *lpidProcess, DWORD cb, DWORD *cbNeeded) {
    std::cout << "[f][win32][LDEscaper]: EnumProcesses called, will return a fake list of running processes." << std::endl;
    ldeio.writeLog("EnumProcesses called, will return a fake list of running processes.", LOG_INFO);
    return NULL;
}

BOOL WINAPI ldeEnumProcessModules(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded) {
    ldeio.writeLog("EnumProcessModules called, will return a fake list of running process modules.", LOG_INFO);
    return NULL;
}

/*
*
*   Win32 Terminate Process Hooks
*
*/

VOID WINAPI ldeExitProcess(UINT uExitCode) {
    ldeio.writeLog("ExitProcess called, but process will not close.", LOG_INFO);
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
    ldeio.writeLog("EnumDisplayMonitors called, will return fake monitor specifications.", LOG_INFO);
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

BOOL WINAPI ldeSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if (uFlags & SWP_NOZORDER) {
        ldeio.writeLog("SetWindowPos called, TopMost flag disabled.", LOG_INFO);
         // Remove the topmost flag
         hWndInsertAfter = HWND_NOTOPMOST;
    }
    //Disable the ability to hide the taskbar.
    if (hWnd == FindWindowA("Shell_TrayWnd", NULL)) {
        ldeio.writeLog("SetWindowPos called, ignoring call to hide taskbar.", LOG_INFO);
        return TRUE;
    }
    return Win32Hook::ogSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
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
    
    ldeio.writeLog("GetForegroundWindow called, will return parent process.", LOG_INFO);
    return hParentWnd;
}

HWND WINAPI ldeSetActiveWindow(HWND hWnd)
{
    ldeio.writeLog("SetActiveWindow called, will return NULL.", LOG_INFO);
    return NULL;
}

BOOL WINAPI ldeShowWindow(HWND hWnd, int nCmdShow) {
    // If the window is maximized, change it to normal
    if (nCmdShow == SW_SHOWMAXIMIZED) {
        ldeio.writeLog("ShowWindow called, will return SW_SHOWNORMAL.", LOG_INFO);
        // Change to SW_SHOWNORMAL instead of minimizing the window
        nCmdShow = SW_SHOWNORMAL;
    }
    //Disable the ability to hide the taskbar.
    if (hWnd == FindWindowA("Shell_TrayWnd", NULL)) {
        ldeio.writeLog("ShowWindow called, blocking request to hide taskbar.", LOG_INFO);
        return TRUE;
    }
    
    return Win32Hook::ogShowWindow(hWnd, nCmdShow);
}

LONG WINAPI ldeSetWindowLongW(HWND hWnd, int nIndex, LONG dwNewLong) {
    if (nIndex == GWL_STYLE) {
        if (dwNewLong & WS_EX_TOPMOST) {
            dwNewLong &= ~WS_EX_TOPMOST; // remove the fullscreen flag
            ldeio.writeLog("SetWindowLongW called, will remove TopMost flag.", LOG_INFO);
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


/*
 *
 *  Win32 Keyboard Hooks
 *
 */
SHORT WINAPI ldeGetAsyncKeyState(int vKey)
{
    ldeio.writeLog("GetAsyncKeyState called, will return NULL.", LOG_INFO);
    return NULL;
}

/*
 *
 *  Win32 Windows Hooks
 *
 */

HHOOK WINAPI ldeSetWindowsHookExA(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) {
    ldeio.writeLog("SetWindowsHookExA called, will return NULL.", LOG_INFO);
    // return NULL to prevent the application from setting any Windows hooks
    return NULL;
}

HANDLE WINAPI ldeCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {

    if (dwShareMode == 0)
    {
        dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        ldeio.writeLog("Intercepted Anti-Debug (CreateFileA); dwShareMode = 7.", LOG_WARNING);
    }

    return Win32Hook::ogCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI ldeCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    
    if (dwShareMode == 0)
    {
        dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        ldeio.writeLog("Intercepted Anti-Debug (CreateFileW); dwShareMode = 7.", LOG_WARNING);
    }

    return Win32Hook::ogCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

void WINAPI ldeGetSystemInfo(LPSYSTEM_INFO lpSystemInfo) {
    lpSystemInfo->dwNumberOfProcessors = 1;
    lpSystemInfo->dwActiveProcessorMask = 1;
    lpSystemInfo->dwPageSize = 4096;
    lpSystemInfo->lpMinimumApplicationAddress = (PVOID)0x10000;
    lpSystemInfo->lpMaximumApplicationAddress = (PVOID)0x7FFFFFFF;
    lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_PENTIUM;
    lpSystemInfo->dwAllocationGranularity = 0x10000;
    lpSystemInfo->wProcessorLevel = 5;
    lpSystemInfo->wProcessorRevision = 0;
}

BOOL WINAPI ldeGetVersionExW(LPOSVERSIONINFOW lpVersionInformation) {
    lpVersionInformation->dwMajorVersion = 10;
    lpVersionInformation->dwMinorVersion = 0;
    lpVersionInformation->dwBuildNumber = 7601;
    lpVersionInformation->dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
    return TRUE;
}

/*
 *
 *  Win32 Registry Hooks
 *
 */

LONG WINAPI ldeRegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData) {
    ldeio.writeLog("RegSetValueExA called(" + (std::string)lpValueName + "), will return ERROR_SUCCESS.", LOG_INFO);
    return ERROR_SUCCESS;
}

LONG WINAPI ldeRegDeleteKeyExA(HKEY hKey, LPCSTR lpSubKey, REGSAM samDesired, DWORD Reserved) {
    ldeio.writeLog("RegDeleteKeyExA called(" + (std::string)lpSubKey + "), will return ERROR_SUCCESS.", LOG_WARNING);
    return ERROR_SUCCESS;
}

LONG WINAPI ldeRegDeleteValueA(HKEY hKey, LPCSTR lpValueName) {
    ldeio.writeLog("RegDeleteValueA called(" + (std::string)lpValueName + "), will return ERROR_SUCCESS.", LOG_WARNING);
    return ERROR_SUCCESS;
}

/*
*
*   LDEscaper functions
*
*/

BOOL Win32Hook::attachHooks()
{
    ldeio.writeLog("Attaching to WIN32 API functions...", LOG_INJECTOR);
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
    DetourAttach(&reinterpret_cast<PVOID&>(ogCreateFileA), ldeCreateFileA);
    DetourAttach(&reinterpret_cast<PVOID&>(ogCreateFileW), ldeCreateFileW);
    DetourAttach(&reinterpret_cast<PVOID&>(ogGetSystemInfo), ldeGetSystemInfo);
    DetourAttach(&reinterpret_cast<PVOID&>(ogGetVersionExW), ldeGetVersionExW);
    ldeio.writeLog("Finished hooking WIN32 API functions!", LOG_SUCCESS);
    return TRUE;
}

BOOL Win32Hook::detachHooks()
{
    ldeio.writeLog("Detaching from WIN32 API functions...", LOG_INJECTOR);
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
    DetourDetach(&reinterpret_cast<PVOID&>(ogCreateFileA), ldeCreateFileA);
    DetourDetach(&reinterpret_cast<PVOID&>(ogCreateFileW), ldeCreateFileW);
    DetourDetach(&reinterpret_cast<PVOID&>(ogGetSystemInfo), ldeGetSystemInfo);
    DetourDetach(&reinterpret_cast<PVOID&>(ogGetVersionExW), ldeGetVersionExW);
    return TRUE;
}
