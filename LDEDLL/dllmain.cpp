#include "LDEIO.h"
#include "Hooks/LDBHook.h"
#include "Hooks/Win32Hook.h"

#pragma comment(lib, "MinHook.x86.lib")

LDEIO ldeio;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if(MH_Initialize() == MH_OK)
            std::cout << "[f][LDEscaper]: MinHook initialized successfully!" << std::endl;
        else
        {
            MessageBoxA(NULL, "Failed to initialize MinHook.", "LDE Error", MB_OK | MB_ICONERROR);
            break;
        }
        if(!LDBHook::init())
        {
            MessageBoxA(NULL, "Failed to initialize LDBHook.", "LDE Error", MB_OK | MB_ICONERROR);
            break;
        }
        ldeio = LDEIO();
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