#pragma once
#include "Globals.h"

class LDEIO
{
public:
    LDEIO();
    void writeLog(std::string message, int code); // Write a message to the log file.
    void displayASCII(); // Display ASCII art.
};
