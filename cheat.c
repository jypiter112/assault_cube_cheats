#include "./funcs.h"

int main(int a, char** b){
    /*
        Powershell to verify:
        Get-Process -Name "ac_client" | Select-Object Name, Id
     */
    // Get PID
    GetProcId("ac_client.exe", &pid);
    if(!pid){
        printf("Client not found, exiting.\n");
        return 1;
    }
    printf("Process found! ID: %d\n", (int)pid);

    // Get module base address
    GetModBase(&pid, "ac_client.exe", &modbase);
    if(!modbase){
        printf("Error finding module base!");
        return 1;
    }
    printf("Modbase: 0x%x\n", (int)modbase);
    
    // Open the process
    HANDLE hProc = (HANDLE)calloc(1, sizeof(HANDLE));
    hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    if(!hProc){
        printf("Couldn't open process, exiting.\n");
        return 1;
    }

    // Read memory
    int plrptr = 0;
    if(!ReadProcessMemory(hProc, (LPCVOID)(modbase + plr_offset), &plrptr, sizeof(plrptr), 0)){
        printf("Reading memory failed.\n");
    }
    int hp;
    if(!ReadProcessMemory(hProc, (LPCVOID)(plrptr + hp_offset), &hp, sizeof(hp), 0)){
        printf("Reading memory failed.\n");
    }
    
    printf("HP:%d\n", hp);
    LoopEntList(hProc);
    while(1){
        InfiniteHp(hProc);
        LoopEntList(hProc);
        Sleep(50);
    }
    // Cleanup
    if(hProc){
        CloseHandle(hProc);
    }
    free(hProc);
    return 0;
}