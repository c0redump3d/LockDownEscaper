#include "LDBHook.h"
#include <detours.h>

cldb_do_some_stuff_t og_do_some_stuff = nullptr;
cldb_do_some_other_stuff_t og_do_some_other_stuff = nullptr;
cldb_do_yet_more_stuff_t og_do_yet_more_stuff = nullptr;
cldb_do_some_other_stuff_s_t og_do_some_other_stuff_s = nullptr;
disable_task_manager_t og_disable_task_manager = nullptr;
empty_clipboard_t og_empty_clipboard = nullptr;
lockdown_log_t og_lockdown_log = nullptr;
create_file_t og_create_file = nullptr;
check_foreground_window_t og_check_foreground_window = nullptr;
nt_query_system_information_t og_nt_query_system_information = nullptr;
get_monitor_info_t og_get_monitor_info = nullptr;
lockdown_check_vm_t og_check_vm = nullptr;
WNDPROC ogWndProc = nullptr;

NTSTATUS WINAPI nt_query_system_information(SYSTEM_INFORMATION_CLASS SystemInformationClass,
                                                       PVOID SystemInformation, ULONG SystemInformationLength,
                                                       PULONG ReturnLength)
{
    const auto status = og_nt_query_system_information(SystemInformationClass, SystemInformation,
                                                       SystemInformationLength,
                                                       ReturnLength);

    if (SystemProcessInformation == SystemInformationClass && status == ERROR_SUCCESS)
    {
        auto previous = PSYSTEM_PROCESS_INFORMATION(SystemInformation);
        auto current = PSYSTEM_PROCESS_INFORMATION((PUCHAR)previous + previous->NextEntryOffset);
        auto count = 0u;

        while (previous->NextEntryOffset != NULL)
        {
            previous->NextEntryOffset = 0;
            previous = current;
            current = PSYSTEM_PROCESS_INFORMATION((PUCHAR)current + current->NextEntryOffset);
        }

        //std::cout << "[f][LDBdll][LDEscaper]: Hid " << count << " processes" << std::endl;
    }

    return status;
}

extern int _cdecl ldeDoSomeStuff(int* a1)
{
    ldeio.writeLog("CLDBDoSomeStuff has been called!", LOG_WARNING);
    if (*a1 >> 6 & 1)
    {
        auto v1 = 1024;
        v1 += 2048;
        auto v6 = v1 + 2048;
        *a1 += 2 * v6;

        return v6;
    }
    return 0;
}

extern bool _cdecl ldeDoSomeOtherStuff(int* a1)
{
    ldeio.writeLog("CLDBDoSomeOtherStuff has been called!", LOG_WARNING);
    *a1 += 2048;
    return TRUE;
}

extern bool _cdecl ldeDoYetMoreStuff()
{
    ldeio.writeLog("CLDBDoYetMoreStuff has been called!", LOG_WARNING);
    return TRUE;
}

extern int _cdecl ldeDoSomeOtherStuffS(int* a1)
{
    return 1024 + 4096 + 2048;
}

BOOL LDBHook::attachHooks()
{
    ldeio.writeLog("Attaching to LDB functions...", LOG_INJECTOR);
    
    DetourAttach(&reinterpret_cast<PVOID&>(og_do_some_stuff), ldeDoSomeStuff);
    DetourAttach(&reinterpret_cast<PVOID&>(og_do_some_other_stuff), ldeDoSomeOtherStuff);
    DetourAttach(&reinterpret_cast<PVOID&>(og_do_yet_more_stuff), ldeDoYetMoreStuff);
    DetourAttach(&reinterpret_cast<PVOID&>(og_do_some_other_stuff_s), ldeDoSomeOtherStuffS);
    
    ldeio.writeLog("Finished hooking LDB functions!", LOG_SUCCESS);
    
    ldeio.writeLog("Attaching to ntdll to hook NtQuerySystemInformation...", LOG_INJECTOR);
    
    og_nt_query_system_information = reinterpret_cast<nt_query_system_information_t>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation"));
    if(og_nt_query_system_information != nullptr)
    {
        DetourAttach(&reinterpret_cast<PVOID&>(og_nt_query_system_information), nt_query_system_information);
        ldeio.writeLog("Successfully hooked NtQuerySystemInformation!", LOG_SUCCESS);
        return TRUE;
    }
    
    ldeio.writeLog("Failed to hook NtQuerySystemInformation!", LOG_ERROR);
    return FALSE;
}

BOOL LDBHook::detachHooks()
{
    ldeio.writeLog("Detaching LDB hooks...", LOG_INJECTOR);
    DetourDetach(&reinterpret_cast<PVOID&>(og_do_some_stuff), ldeDoSomeStuff);
    DetourDetach(&reinterpret_cast<PVOID&>(og_do_some_other_stuff), ldeDoSomeOtherStuff);
    DetourDetach(&reinterpret_cast<PVOID&>(og_do_yet_more_stuff), ldeDoYetMoreStuff);
    DetourDetach(&reinterpret_cast<PVOID&>(og_do_some_other_stuff_s), ldeDoSomeOtherStuffS);
    DetourDetach(&reinterpret_cast<PVOID&>(og_nt_query_system_information), nt_query_system_information);
    return TRUE;
}

BOOL LDBHook::init()
{
    ldeio.writeLog("Initializing LDBHook...", LOG_INJECTOR);
    //Make sure MinHook is initialized.
    if(MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoSomeStuff@@YAHPAH@Z", &ldeDoSomeStuff, (void**)&og_do_some_stuff) != MH_OK)
    {
        ldeio.writeLog("Failed to hook CLDBDoSomeStuff!", LOG_ERROR);
        return FALSE;
    }
    if(MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoSomeOtherStuff@@YAHPAH@Z", &ldeDoSomeOtherStuff, (void**)&og_do_some_other_stuff) != MH_OK)
    {
        ldeio.writeLog("Failed to hook CLDBDoSomeOtherStuff!", LOG_ERROR);
        return FALSE;
    }
    if(MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoYetMoreStuff@@YAHPAH@Z", &ldeDoYetMoreStuff, (void**)&og_do_yet_more_stuff) != MH_OK)
    {
        ldeio.writeLog("Failed to hook CLDBDoYetMoreStuff!", LOG_ERROR);
        return FALSE;
    }
    if(MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoSomeOtherStuffs@@YAHPAH@Z", &ldeDoSomeOtherStuffS, (void**)&og_do_some_other_stuff_s) != MH_OK)
    {
        ldeio.writeLog("Failed to hook CLDBDoSomeOtherStuffs!", LOG_ERROR);
        return FALSE;
    }
    
    //Very important to enable the hooks.
    MH_EnableHook(MH_ALL_HOOKS);
    
    ldeio.writeLog("LockDownBrowser.dll successfully hooked!", LOG_SUCCESS);
    
    return TRUE;
}