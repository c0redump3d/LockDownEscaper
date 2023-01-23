#include "Hooks/LDBHook.h"
#include "Hooks/Win32Hook.h"

#pragma comment(lib, "MinHook.x86.lib")

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ldeio = LDEIO();
        
        if(MH_Initialize() != MH_OK)
        {
            ldeio.writeLog("Failed to initialize MinHook.", LOG_ERROR);
            break;
        }
        
        ldeio.writeLog("MinHook initialized successfully!", LOG_SUCCESS);
        
        if(!LDBHook::init())
        {
            ldeio.writeLog("Failed to initialize LDBHook.", LOG_ERROR);
            break;
        }
        
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        Win32Hook::attachHooks();
        LDBHook::attachHooks();
        DetourTransactionCommit();
        break;
    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        Win32Hook::detachHooks();
        LDBHook::detachHooks();
        DetourTransactionCommit();
        break;
    }
	
    return TRUE;
}