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
    //Set console window title.
    SetConsoleTitleA("LockDown Escaper");
    //Display ASCII art.
    displayASCII();
}

void LDEIO::displayASCII()
{
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
}

