#include <Windows.h>
#include <winuser.h>
#include <TlHelp32.h>
#include <stdio.h>
/*
Notes: use uintptr_t instead of DWORD to avoid warnings
*/
// Offset "ac_client.exe" + 0x17E0A8
uintptr_t modbase;
uintptr_t pid;

// offsets
const uintptr_t plr_offset = 0x17E0A8;
const uintptr_t hp_offset = 0xEC;
// pos offsets
const uintptr_t posx_offset = 0x2C;
const uintptr_t posy_offset = 0x30;
const uintptr_t posz_offset = 0x28;
const uintptr_t playername_offset = 0x205;
const uintptr_t camx = 0x34;
const uintptr_t camy = 0x38;
// incremented by 4 first value is ???
// const uintptr_t entlistptr = 0x18ac04;
const uintptr_t entcountptr = 0x18AC0C;

int infiniteHpEnabled = 0;
int plrptr, hp, entcount = 0; 
const int newhp = 999;

BYTE oldAmmoPatch[2] = { 0 };
const BYTE patch[2] = { 0x90, 0x90 };

void InfiniteHp(HANDLE hProc){
    if(!plrptr){
        if(!ReadProcessMemory(hProc, (LPCVOID)(modbase + plr_offset), &plrptr, sizeof(plrptr), 0)){
            printf("[inf hp] rmf plroff.\n");
        }
    }
    if(!WriteProcessMemory(hProc, (LPVOID)(plrptr + hp_offset), &newhp, sizeof(newhp), 0)){
        printf("[inf hp] rmf hpoff.\n");
    }
}
int LoopEntList(HANDLE hProc){
    if (!ReadProcessMemory(hProc, (LPCVOID)(modbase + entcountptr), &entcount, sizeof(entcount), 0)) {
        printf("[loopent] Failed to read entity count. Error: %lu\n", GetLastError());
        return 1;
    }
    /*
        Viisi tuntia kesti saada toimimaan tämä paska.
    */
    // Variables for the pointer chain
    int offset1, offset2, posx, posy, posz;
    char entityName[20] = "notfound";
    ReadProcessMemory(hProc, (LPCVOID)(modbase + 0x18ac04), &offset1, 4, 0);
    for(int i = 1; i <= entcount; i++){
        ReadProcessMemory(hProc, (LPCVOID)(offset1 + (i*4)), &offset2, 4, 0);
        ReadProcessMemory(hProc, (LPCVOID)(offset2 + 0x205), &entityName, 20, 0);
        ReadProcessMemory(hProc, (LPCVOID)(offset2 + posx_offset), &posx, sizeof(int), 0);
        ReadProcessMemory(hProc, (LPCVOID)(offset2 + posy_offset), &posy, sizeof(int), 0);
        ReadProcessMemory(hProc, (LPCVOID)(offset2 + posz_offset), &posz, sizeof(int), 0);
        printf("Entity name [i: %d]: %s\n\n - x:%d\n - y:%d\n - z:%d\n", i, entityName, posx, posy, posz);
    }
    return 0;
}
int PathInfiniteAmmo(HANDLE hProc, int state){
    // Save old patch
    if(oldAmmoPatch[0] == 0){
        if(!ReadProcessMemory(hProc, (LPCVOID)(modbase + 0xC73EF), &oldAmmoPatch, sizeof(oldAmmoPatch), 0)){
            printf("Infinite ammo patch failed.");
            return 1;
        }
    }
    if(state){
        if(!WriteProcessMemory(hProc, (LPVOID)(modbase + 0xC73EF), &patch, sizeof(patch), 0)){
            printf("Infinite ammo patch failed.");
            return 1;
        }
    } else {
        if(!WriteProcessMemory(hProc, (LPVOID)(modbase + 0xC73EF), &oldAmmoPatch, sizeof(oldAmmoPatch), 0)){
            printf("Ammo patch restoration failed.");
            return 1;
        }
    }
    return 0;
}

void GetProcId(char* name, uintptr_t* out) {
    PROCESSENTRY32* pe = (PROCESSENTRY32*)malloc(sizeof(PROCESSENTRY32));
    pe->dwSize = sizeof(PROCESSENTRY32);
    HANDLE* hSnap = (HANDLE*)malloc(sizeof(HANDLE));
    *hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(*hSnap, pe) && *hSnap) {
        do {
            if (strcmp((char*)pe->szExeFile, name) == 0) {
                break;
            }
        } while (Process32Next(*hSnap, pe));
    }
    *out = pe->th32ProcessID;
    free(pe);
    free(hSnap);
}
void GetModBase(uintptr_t* pid, char* modName, uintptr_t* out) {
    MODULEENTRY32* modEntry = (MODULEENTRY32*)malloc(sizeof(MODULEENTRY32));
    modEntry->dwSize = sizeof(MODULEENTRY32);
    HANDLE* hSnap = (HANDLE*)malloc(sizeof(HANDLE));
    *hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, *pid);
    if(Module32First(*hSnap, modEntry) && *hSnap){
        do{
            if(strcmp((char*)modEntry->szModule, modName) == 0){
                *out = (uintptr_t)modEntry->modBaseAddr;
                break;
            }
        } while(Module32Next(*hSnap, modEntry));
    }
    free(modEntry);
    free(hSnap);
}