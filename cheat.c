#include "./funcs.h"

int main(int a, char** b){
    /*
        Powershell to verify:
        Get-Process -Name "ac_client" | Select-Object Name, Id
     */
    DWORD pid = GetProcId("ac_client.exe");
    if(!pid){
        printf("Client not found, exiting.\n");
        return 1;
    }
    printf("Process found! ID: %d\n", (int)pid);

    DWORD modBase = GetModBase(pid, "ac_client.exe");
    if(!modBase){
        printf("Error finding module base!");
        return 1;
    }
    printf("Modbase: %d\n", (int)modBase);

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    if(!hProc){
        printf("Couldn't open process, exiting.\n");
        return 1;
    }

    // Cleanup
    if(hProc){
        CloseHandle(hProc);
    }
    printf("Exiting without errors!\n");
    return 0;
}