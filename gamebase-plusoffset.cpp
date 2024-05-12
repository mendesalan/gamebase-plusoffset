#include <iostream>
#include <windows.h>
#include <TlHelp32.h>

// Prefered Image Base Address: 0x00400000
// Fixed Module Pointer: 0x38F7E4

DWORD GetBaseAddress(DWORD processId)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to create snapshot" << std::endl;
        return 0;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &moduleEntry))
    {
        CloseHandle(hSnapshot);
        return (DWORD)moduleEntry.modBaseAddr;
    }

    CloseHandle(hSnapshot);
    return 0;
}

int main(int argc, char* argv[])
{
    HWND hwnd = FindWindowA(0, "GTA: Vice City");

    if (!hwnd)
    {
        std::cout << "GTA: Vice City window not found!" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "GTA: Vice City window found!" << std::endl;

        DWORD pId;
        GetWindowThreadProcessId(hwnd, &pId);
        std::cout << "Process ID: " << pId << std::endl;

        HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);

        if (!hProc)
        {
            std::cout << "Cannot open Process: " << hProc << std::endl;
        }
        else
        {
            std::cout << "Process opened successfully!" << std::endl;

            uintptr_t baseAddress = GetBaseAddress(pId);
            std::cout << "Base Address: " << std::hex << std::uppercase << baseAddress << std::endl;

            uintptr_t modulePointer = 0x38F7E4;
            uintptr_t entityAddr = baseAddress + modulePointer;

            std::cout << baseAddress << " + " << modulePointer << " = " << std::hex << std::uppercase << entityAddr << std::endl;
                        
            int entityPointer;

            // Read the content at the memory position
            if (!ReadProcessMemory(hProc, LPCVOID(entityAddr), &entityPointer, sizeof(entityPointer), nullptr)) {
                std::cerr << "Failed to read memory at address 0x" << std::hex << entityAddr << ". Error code: " << GetLastError() << std::endl;
                CloseHandle(hProc);
                return 1;
            }

            // Print the value that the memory position points to
            std::cout << "Pointer's value 0x" << std::hex << entityAddr << ": " << entityPointer << std::endl;
            
            float healthValue = 101.f;
            uintptr_t healthAddr = entityPointer + 0x354;
            
            std::cout << "Health Address: 0x" << std::hex << healthAddr << std::endl;
            
            int isSuccessful = WriteProcessMemory(hProc, (LPVOID)healthAddr, &healthValue, (DWORD)sizeof(healthValue), NULL);

            if(isSuccessful == 0) 
            {
				std::cout << "Failed to write memory" << std::endl;
			}
            else 
            {
				std::cout << "Health value changed to: " << healthValue << std::endl;
			}
                                               
            CloseHandle(hProc);
        }
    }

    system("PAUSE");
    return 0;
}
