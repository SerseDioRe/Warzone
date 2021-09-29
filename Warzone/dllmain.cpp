// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"
#include "Classes.h"
#include "Offsets.h"
#include "Memory.h"

Offsets* offsets       = nullptr;

uintptr_t   moduleBase = 0;
uintptr_t           cg = 0;
bool              bUav = false;
int             health = 0;

// ingame 17084918

DWORD WINAPI CallOfDutyModernWarfare(HMODULE hModule)
{
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif // _DEBUG

	while(moduleBase == 0)
    {
        moduleBase = (uintptr_t)GetModuleHandle(NULL);
        Sleep(30);
    }

    offsets = new Offsets();

	while(!KEY_MODULE_EJECT)
	{
        cg         = (uintptr_t)(moduleBase + offsets->GetOffset(Offsets::CG_T));
        health     = *(int*)((uintptr_t)offsets->FindDMAAddy(cg, { 0x25C }));

        if(KEY_UAV_MANAGER)
            bUav = !bUav;

        if (bUav)
        {
            if (cg != 0) 
            {
                if (health >= 0 && health <= 100)
                {
                    *(int*)((uintptr_t)offsets->FindDMAAddy(cg, { 0x304 })) = 33619969;
                }
            }
        }
	}

#ifdef _DEBUG
    fclose(f);
    FreeConsole();
#endif // _DEBUG

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)CallOfDutyModernWarfare, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

