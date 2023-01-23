# LockDownEscaper
__A LockDown Browser Bypass__

## Notice
This application is to be used for educational purposes only.

I, Carson Kelley, am not responsible for any damages that running this application may cause.

*It is important to note that I have not tested ANY of these features while actively taking a 'lockdown' test, as this is just simply for educational purposes. Although it should work just fine, there are no promises.*

## Installation
To inject, simply open LDEInjector.exe and follow the prompts. LockDown Browser will automatically open and injection will start.

## Features
The following changes are made once injection completes:

- Anti-VM techniques are disabled.

- Windows 32 API functions used to create hooks, like keyboard events have been disabled.

- Window 32 API functions that are used to see running applications/debuggers are disabled.

- NTDLL functions hooked to disable the ability to see running applications on the system.

- Always on top flag for window is always disabled allowing for easy task switching.

- Attempting to Create/Read/Write to registry values have been patched to disable things like disabling taskmanager.

- Certain LockDownBrowser functions have been hooked & disabled as well.
