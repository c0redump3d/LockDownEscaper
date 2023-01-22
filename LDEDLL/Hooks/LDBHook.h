#pragma once
#include "../Globals.h"
#include "MinHook.h"
#include <winternl.h>

#define LD_OFFSET(offset) ((char*) GetModuleHandle(NULL) + (offset))

    // TYPEDEFS ----------------------------------------------
    typedef int (*cldb_do_some_stuff_t)(int*);
    typedef BOOL (*cldb_do_some_other_stuff_t)(int*);
    typedef BOOL (*cldb_do_yet_more_stuff_t)();
    typedef int (*cldb_do_some_other_stuff_s_t)(int*);
    typedef LSTATUS (*disable_task_manager_t)(void*);
    typedef decltype(&EmptyClipboard) empty_clipboard_t;
    typedef void (*lockdown_log_t)(char*, ...);
    typedef decltype(&CreateFileA) create_file_t;
    typedef BOOL (*check_foreground_window_t)();
    typedef decltype(&NtQuerySystemInformation) nt_query_system_information_t;
    typedef decltype(&GetMonitorInfoW) get_monitor_info_t;
    typedef UINT (*lockdown_check_vm_t)();

    extern cldb_do_some_stuff_t og_do_some_stuff;
    extern cldb_do_some_other_stuff_t og_do_some_other_stuff;
    extern cldb_do_yet_more_stuff_t og_do_yet_more_stuff;
    extern cldb_do_some_other_stuff_s_t og_do_some_other_stuff_s;
    extern disable_task_manager_t og_disable_task_manager;
    extern empty_clipboard_t og_empty_clipboard;
    extern lockdown_log_t og_lockdown_log;
    extern create_file_t og_create_file;
    extern check_foreground_window_t og_check_foreground_window;
    extern nt_query_system_information_t og_nt_query_system_information;
    extern get_monitor_info_t og_get_monitor_info;
    extern lockdown_check_vm_t og_check_vm;

    static inline WNDPROC originalWndProc;
    
    class LDBHook
    {
    public:
        static BOOL attachHooks();
        static BOOL detachHooks();
        static BOOL init();
    };