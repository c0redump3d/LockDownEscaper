#include "LDEIO.h"

LDEIO::LDEIO()
{
    //Create console window.
    AllocConsole();
    //Redirect standard input and output to console window.
    FILE *fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    //Enable color output.
    DWORD dwMode;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hOutput, &dwMode);
    dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOutput, dwMode);
    //Set console window title.
    SetConsoleTitleA("LockDown Escaper");
    //Display ASCII art.
    displayASCII();
}

void LDEIO::writeLog(std::string message, int code)
{
    switch(code)
    {
    case LOG_SUCCESS:
            std::cout << GREEN << "[+]" << RESET << "[LDEscaper]: " << message << std::endl;
            break;
        case LOG_ERROR:
            std::cout << RED << "[-]" << RESET << "[LDEscaper]: " << message << std::endl;
            break;
        case LOG_WARNING:
            std::cout << YELLOW << "[!]" << RESET << "[LDEscaper]: " << message << std::endl;
            break;
        case LOG_INFO:
            std::cout << BLUE << "[i]" << RESET << "[LDEscaper]: " << message << std::endl;
            break;
        case LOG_INJECTOR:
            std::cout << MAGENTA << "[i]" << RESET << "[LDEscaper]: " << message << std::endl;
            break;
    }
}


void LDEIO::displayASCII()
{
    if(consoleCreated)
        return;
    system("CLS");
    std::cout << R"(

     /$$       /$$$$$$$        /$$$$$$$$                                                            
    | $$      | $$__  $$      | $$_____/                                                            
    | $$      | $$  \ $$      | $$        /$$$$$$$  /$$$$$$$  /$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$ 
    | $$      | $$  | $$      | $$$$$    /$$_____/ /$$_____/ |____  $$ /$$__  $$ /$$__  $$ /$$__  $$
    | $$      | $$  | $$      | $$__/   |  $$$$$$ | $$        /$$$$$$$| $$  \ $$| $$$$$$$$| $$  \__/
    | $$      | $$  | $$      | $$       \____  $$| $$       /$$__  $$| $$  | $$| $$_____/| $$      
    | $$$$$$$$| $$$$$$$/      | $$$$$$$$ /$$$$$$$/|  $$$$$$$|  $$$$$$$| $$$$$$$/|  $$$$$$$| $$      
    |________/|_______/       |________/|_______/  \_______/ \_______/| $$____/  \_______/|__/      
                                                                      | $$                          
                                                                      | $$                          
                                                                      |__/                          
    
    )" << std::endl;
    consoleCreated = true;
}

