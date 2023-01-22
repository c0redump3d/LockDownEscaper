#include "Inject.h"

/***
*     /$$       /$$$$$$$        /$$$$$$$$                                                            
 *    | $$      | $$__  $$      | $$_____/                                                            
 *    | $$      | $$  \ $$      | $$        /$$$$$$$  /$$$$$$$  /$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$ 
 *    | $$      | $$  | $$      | $$$$$    /$$_____/ /$$_____/ |____  $$ /$$__  $$ /$$__  $$ /$$__  $$
 *    | $$      | $$  | $$      | $$__/   |  $$$$$$ | $$        /$$$$$$$| $$  \ $$| $$$$$$$$| $$  \__/
 *    | $$      | $$  | $$      | $$       \____  $$| $$       /$$__  $$| $$  | $$| $$_____/| $$      
 *    | $$$$$$$$| $$$$$$$/      | $$$$$$$$ /$$$$$$$/|  $$$$$$$|  $$$$$$$| $$$$$$$/|  $$$$$$$| $$      
 *    |________/|_______/       |________/|_______/  \_______/ \_______/| $$____/  \_______/|__/      
 *                                                                      | $$                          
 *                                                                      | $$                          
 *                                                                      |__/
 *
 *  Created by Carson Kelley - 1/2023 - FOR EDUCATIONAL USE ONLY
 *                                                                     
 */
int main(int argc, char* argv[])
{
    std::cout << "LockDown Escaper - v1 - "<< RED << "FOR EDUCATIONAL USE ONLY" << RESET << std::endl;
    std::cout << "Created by Carson Kelley" << std::endl;

    //Display terms of use.
    std::cout << std::endl << "By using this application, you agree to the following:" << std::endl;
    std::cout << "1. This application is for educational use only." << std::endl;
    std::cout << "2. This application is not to be used for malicious purposes." << std::endl;
    std::cout << "3. This application is not to be used to bypass any academic honesty measures." << std::endl;
    std::cout << "4. I (Carson Kelley), am not responsible for any damage caused by this application." << std::endl << std::endl;

    //Make user agree to terms to continue.
    char input = ' ';
    while(input != 'y' && input != 'Y' && input != 'n' && input != 'N')
    {
        std::cout << "Type y to agree and continue, or n to exit: ";
        std::cin >> input;
    
        //If user has agreed to the terms, continue.
        if(input == 'y' || input == 'Y')
        {
            std::cout << "[LDEInjector]: Starting..." << std::endl;
            const BOOL succeeded = Inject::injectToProcess(EXE_LOC);

            if(succeeded)
            {
                std::cout << GREEN << "[+]" << RESET << "[LDEInjector]: Injection succeeded!" << std::endl;
            }else
            {
                std::cout << RED << "[-]" << RESET << "[LDEInjector]: Injection failed!" << std::endl;
            }
            
        }else if(input == 'n' || input == 'N')
        {
            std::cout << "Exiting..." << std::endl;
            return -2;
        }
    }
    
    // Sleep for 3.5 seconds to allow user to read output.
    Sleep(3500);
    
    return 0;
}
