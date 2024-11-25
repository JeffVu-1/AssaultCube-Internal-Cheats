#include <iostream>
#include <windows.h>
#include <string>
#include <TlHelp32.h>

DWORD GetProdId(const char* procName) {

    DWORD ProcId = 0;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hsnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hsnap, &procEntry)) {
            do {

                if (!_stricmp(procEntry.szExeFile, procName)) {
                    ProcId = procEntry.th32ProcessID;
                    break;
                }

            } while (Process32Next(hsnap, &procEntry));
        }  
    }
    CloseHandle(hsnap);
    return ProcId;

}

int main() {
        char currentDir[MAX_PATH];
        if (GetModuleFileNameA(NULL, currentDir, MAX_PATH) == 0) {
            std::cerr << "Failed to get the current directory." << std::endl;
            return 1;
        }

        std::string directoryPath = currentDir;
        size_t lastSlashIndex = directoryPath.find_last_of("\\/");
        if (lastSlashIndex != std::string::npos) {
            directoryPath = directoryPath.substr(0, lastSlashIndex + 1);
        }

        std::string DLLPATH = directoryPath + "AssaultCubeDLL.dll";

        const char* DLLPATHCStr = DLLPATH.c_str();
        const char* procname = "ac_client.exe";
        DWORD procId = 0;

		std::cout << "Waiting for game to start..." << std::endl;
        while (!procId) {
            procId = GetProdId(procname);
            Sleep(10);
        }

		std::cout << "Game found, injecting DLL..." << std::endl; 

        HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

        if (hProc && hProc != INVALID_HANDLE_VALUE)
        {
            void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            if (loc)
            {
                WriteProcessMemory(hProc, loc, DLLPATHCStr, strlen(DLLPATHCStr) + 1, 0);
            }
            HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

            if (hThread)
            {
                CloseHandle(hThread);
            }
        }
        if (hProc)
        {
            CloseHandle(hProc);
        }
        return 0;
}

