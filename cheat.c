#include "./funcs.h"

int main(int a, char** b){
    /*
        Powershell to verify:
        Get-Process -Name "ac_client" | Select-Object Name, Id
     */
    pid = GetProcId("ac_client.exe");
    if(!pid){
        printf("Client not found, exiting.\n");
        return 1;
    }
    printf("Process found! ID: %d\n", (int)pid);

    modbase = GetModBase(pid, "ac_client.exe");
    if(!modbase){
        printf("Error finding module base!");
        return 1;
    }
    printf("Modbase: 0x%x\n", (int)modbase);

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    if(!hProc){
        printf("Couldn't open process, exiting.\n");
        return 1;
    }

    // Main stuff here
    int plrptr = 0;
    if(!ReadProcessMemory(hProc, (LPCVOID)(modbase + plr_offset), &plrptr, sizeof(plrptr), 0)){
        printf("Reading memory failed.\n");
    }
    int hp;
    if(!ReadProcessMemory(hProc, (LPCVOID)(plrptr + hp_offset), &hp, sizeof(hp), 0)){
        printf("Reading memory failed.\n");
    }
    printf("HP:%d\n", hp);

    // Cleanup
    if(hProc){
        CloseHandle(hProc);
    }
    printf("Exiting without errors!\n");
    return 0;
}