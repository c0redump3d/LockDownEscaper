#pragma once

#include <string>
#include <Windows.h>
#include <Userenv.h>
#include <iostream>
#include <ostream>
#include <fstream>

#include "Colors.h"

//The name of the DLL to inject. This will be stored in the root directory of the application.
#define DLL_NAME "LDEDLL.dll"
#define EXE_LOC "C:\\Program Files (x86)\\Respondus\\LockDown Browser\\LockDownBrowser.exe"

class Inject
{
public:
    static BOOL injectToProcess(std::string exeLoc); // Find application and inject dll.
    static BOOL dll_exists(); // Check if the DLL exists in the root directory of the application.
private:
    static BOOL injectDLL(DWORD pid); // Inject the DLL into the process.
};
