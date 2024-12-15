#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>

DWORD modbase;
DWORD pid;
DWORD hp_offset;

// offsets
DWORD plr_offset = 0x17E0A8;
DWORD hp_offset = 0xEC;

// Offset "ac_client.exe" + 0x17E0A8

DWORD GetProcId(char* name) {
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(hSnap, &pe)) {
        do {
            if (strcmp((char*)pe.szExeFile, name) == 0) {
                break;
            }
        } while (Process32Next(hSnap, &pe));
    }
    return pe.th32ProcessID;
}
DWORD GetModBase(DWORD pid, char* modName) {
    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(MODULEENTRY32);
    DWORD modBase = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE| TH32CS_SNAPMODULE32, pid);
    if(Module32First(hSnap, &modEntry)){
        do{
            if(strcmp((char*)modEntry.szModule, modName) == 0){
                modBase = (DWORD)modEntry.modBaseAddr;
                break;
            }
        } while(Module32Next(hSnap, &modEntry));
    }
    return modBase;
}