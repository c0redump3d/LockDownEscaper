#pragma once
#include "Globals.h"

static inline enum
{
    LOG_SUCCESS = 0,
    LOG_ERROR = 1,
    LOG_WARNING = 2,
    LOG_INFO = 3,
    LOG_INJECTOR = 4
};

class LDEIO
{
public:
    LDEIO();
    void writeLog(std::string message, int code); // Write a message to the log file.
    void displayASCII(); // Display ASCII art.
private:
    bool consoleCreated = false;
};
