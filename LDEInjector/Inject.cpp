#include "Inject.h"

BOOL Inject::injectDLL(DWORD pid)
{
    // Open the process with all access.
    const HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    
    // If the process is null, return false.
    if (process == nullptr) {
        std::cout << RED << "[-]" << RESET << "[LDEInjector]: Injection Step 1 - Failed to open process (pid is NULL)." << std::endl;
        return FALSE;
    }

    // Allocate memory in the process for the DLL name.
    const LPVOID remoteString = VirtualAllocEx(process, nullptr, strlen(DLL_NAME) + 1, MEM_RESERVE | MEM_COMMIT,
                                               PAGE_READWRITE);
    if (remoteString == nullptr) {

        std::cout << RED << "[-]" << RESET << "[LDEInjector]: Injection Step 2 - Failed to allocate memory in process (PID: " << pid << ")." << std::endl;
        // Close the process handle.
        CloseHandle(process);
        return FALSE;
    }

    // Write the DLL name to the process.
    if (!WriteProcessMemory(process, remoteString, DLL_NAME, strlen(DLL_NAME) + 1, NULL)) {
        std::cout << RED << "[-]" << RESET << "[LDEInjector]: Injection Step 3 - Failed to write to process (PID: " << pid << ")." << std::endl;
        // Free the memory we allocated in the process.
        VirtualFreeEx(process, remoteString, strlen(DLL_NAME) + 1, MEM_RELEASE);
        // Close the process handle.
        CloseHandle(process);
        return FALSE;
    }

    // Get the address of LoadLibraryA.
    const auto loadLibraryAddress = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA"));
    if (loadLibraryAddress == nullptr) {
        std::cout << RED << "[-]" << RESET << "[LDEInjector]: Injection Step 4 - Failed to get address of LoadLibraryA." << std::endl;
        // Free the memory we allocated in the process.
        VirtualFreeEx(process, remoteString, strlen(DLL_NAME) + 1, MEM_RELEASE);
        // Close the process handle.
        CloseHandle(process);
        return FALSE;
    }

    // Create a remote thread in the process to load the DLL.
    const HANDLE thread = CreateRemoteThread(process, nullptr, 0, loadLibraryAddress, remoteString, 0, nullptr);
    if (thread == nullptr) {
        std::cout << RED << "[-]" << RESET << "[LDEInjector]: Injection Step 5 - Failed to create remote thread in process (PID: " << pid << ")." << std::endl;
        // Free the memory we allocated in the process.
        VirtualFreeEx(process, remoteString, strlen(DLL_NAME) + 1, MEM_RELEASE);
        // Close the process handle.
        CloseHandle(process);
        return FALSE;
    }

    std::cout << GREEN << "[+]" << RESET << "[LDEInjector]: Remote thread has been created, waiting for thread to finish." << std::endl;

    // Wait for the thread to finish.
    WaitForSingleObject(thread, INFINITE);
    
    std::cout << GREEN << "[+]" << RESET << "[LDEInjector]: Remote thread has finished. Injection has completed." << std::endl;

    // Free the memory we allocated in the process.
    VirtualFreeEx(process, remoteString, strlen(DLL_NAME) + 1, MEM_RELEASE);

    // Close the thread and process handles.
    CloseHandle(thread);
    CloseHandle(process);
    return TRUE;
}

BOOL Inject::dll_exists()
{
    // Check if the DLL exists.
    const std::ifstream dllfile(DLL_NAME, std::ios::binary);
    if (!dllfile)
    {
        std::cout << RED << "[-]" << RESET << "[LDEInjector]: Unable to find dll file to inject. Please make sure \'" << DLL_NAME << "\' exists in the root directory of the injector." << std::endl;
        return FALSE;
    }
    return TRUE;
}

BOOL Inject::injectToProcess(std::string exeLoc)
{
    // Check if the DLL exists.
    if(dll_exists())
    {
        // Get the token of the current process.
        HANDLE hToken;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE | TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY, &hToken)) {
            std::cout << RED << "[-]" << RESET << "[LDEInjector]: Error getting process token." << std::endl;
            return FALSE;
        }

        // Duplicate the token to obtain an elevated token.
        HANDLE hElevatedToken;
        if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, nullptr, SecurityIdentification, TokenPrimary, &hElevatedToken)) {
            CloseHandle(hToken);
            std::cout << RED << "[-]" << RESET << "[LDEInjector]: Error duplicating user token." << std::endl;
            return FALSE;
        }

        // Close the original token.
        CloseHandle(hToken);

        // Start exe with the elevated token.
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // Start the process with the elevated token.
        //TODO: Fix later, hardcoded path
        if (!CreateProcessAsUser(hElevatedToken, L"C:\\Program Files (x86)\\Respondus\\LockDown Browser\\LockDownBrowser.exe", nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) {
            CloseHandle(hElevatedToken);
            std::cout << RED << "[-]" << RESET << "[LDEInjector]: Error starting LockDownBrowser. Please make sure LDB is installed at \'" << exeLoc << "\'." << std::endl;
            return FALSE;
        }

        // Inject DLL into the exe
        if (!injectDLL(pi.dwProcessId)) {
            std::cout << RED << "[-]" << RESET << "[LDEInjector]: Error injecting DLL." << std::endl;
            return FALSE;
        }
        
        //Wait a lil bit for initialization
        Sleep(100);
        // Resume the process.
        ResumeThread(pi.hThread);

        // Close the handles.
        CloseHandle(hElevatedToken);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        return TRUE;
    }

    return FALSE;
}

