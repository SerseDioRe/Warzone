// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"
#include "Classes.h"
#include "Offsets.h"
#include "Memory.h"

Offsets* offsets       = nullptr;
ArrayNames* arrayNames = nullptr;
NopInternal* NoCheckOnUav = nullptr;

uintptr_t   moduleBase = 0;
bool              bUav = false;
int           uavValue = 0;
int          uavValue2 = 2;
int          health    = 0;

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

#ifdef DEBUG
    PRINT_WELCOME;
#endif // DEBUG

    offsets = new Offsets();
    NoCheckOnUav = new NopInternal((BYTE*)moduleBase + offsets->GetOffset(Offsets::CheckUav), 3);

	while(!KEY_MODULE_EJECT)
	{
#ifdef _DEBUG
        arrayNames = (ArrayNames*)((uintptr_t)moduleBase + offsets->GetOffset(Offsets::ArrayNames));
#endif // _DEBUG

        uavValue   = *(int*)(moduleBase + offsets->GetOffset(Offsets::Uav1));
        uavValue2  = *(int*)(moduleBase + offsets->GetOffset(Offsets::Uav2));
        health     = *(int*)(moduleBase + offsets->GetOffset(Offsets::Health));

#ifdef _DEBUG
        /*if (KEY_PRINT_ENTITIES)
        {
            for (int i = 0; i < 150; i++)
            {
                Entity* currentEntity = new Entity();
                memcpy(currentEntity->name, (char*)((uintptr_t)arrayNames + (i * 0x268) + 0x0), 16);
                std::cout << "#" << i << " " << currentEntity->name << '\n';
            }
        }*/
#endif // _DEBUG

        if(KEY_UAV_MANAGER)
            bUav = !bUav;

        if (bUav)
        {
            if (health >= 0 && health <= 100)
            {
                if (uavValue >= 3 && uavValue <= 5)
                    *(int*)((uintptr_t)moduleBase + offsets->GetOffset(Offsets::Uav1)) = 6;

                if (uavValue2 == 0 || uavValue2 == 1)
                {
                    *(int*)((uintptr_t)moduleBase + offsets->GetOffset(Offsets::Uav2)) = 33619969;
                    NoCheckOnUav->enable();
                }
            }
            else
                if(NoCheckOnUav->IsEnabled())
                    NoCheckOnUav->disable();
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

