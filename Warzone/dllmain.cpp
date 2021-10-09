// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"
#include "Classes.h"
#include "Offsets.h"
#include "Memory.h"
#include <psapi.h>
#include <process.h>

#define RVA(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

#define INRANGE(x,a,b)	(x >= a && x <= b) 
#define getBits( x )	(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )	(getBits(x[0]) << 4 | getBits(x[1]))

Offsets* offsets = nullptr;

uintptr_t   moduleBase = 0;
uintptr_t           cg = 0;
bool              bUav = false;
int             health = 0;
int             GameMode = 0;

__int64 find_pattern(__int64 range_start, __int64 range_end, const char* pattern) {
    const char* pat = pattern;
    __int64 firstMatch = NULL;
    __int64 pCur = range_start;
    __int64 region_end;
    MEMORY_BASIC_INFORMATION mbi{};
    while (sizeof(mbi) == VirtualQuery((LPCVOID)pCur, &mbi, sizeof(mbi))) {
        if (pCur >= range_end - strlen(pattern))
            break;
        if (!(mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READWRITE))) {
            pCur += mbi.RegionSize;
            continue;
        }
        region_end = pCur + mbi.RegionSize;
        while (pCur < region_end)
        {
            if (!*pat)
                return firstMatch;
            if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
                if (!firstMatch)
                    firstMatch = pCur;
                if (!pat[1] || !pat[2])
                    return firstMatch;

                if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
                    pat += 3;
                else
                    pat += 2;
            }
            else {
                if (firstMatch)
                    pCur = firstMatch;
                pat = pattern;
                firstMatch = 0;
            }
            pCur++;
        }
    }
    return NULL;
}

ULONG WINAPI Init()
{
    while (moduleBase == 0)
    {
        moduleBase = (uintptr_t)GetModuleHandle(NULL);
        Sleep(30);
    }

    offsets = new Offsets();

    while (!KEY_MODULE_EJECT)
    {
        cg = (uintptr_t)(moduleBase + offsets->GetOffset(Offsets::CG_T));
        GameMode = *(int*)(moduleBase + offsets->GetOffset(Offsets::GAMEMODE));

#ifdef _DEBUG
        std::cout << *(int*)offset2 << '\n';
#endif

        if (KEY_UAV_MANAGER)
            bUav = !bUav;

        if (bUav)
        {
            if (GameMode > 1)
            {
                if (cg != 0)
                {
                    health = *(int*)((uintptr_t)offsets->FindDMAAddy(cg, { 0x25C }));
                    if (health >= 0 && health <= 100)
                    {
                        *(int*)((uintptr_t)offsets->FindDMAAddy(cg, { 0x304 })) = 33619969;
                    }
                }
            }
        }

        Sleep(1);
    }
   
    return true;
}

/*DWORD WINAPI CallOfDutyModernWarfare(HMODULE hModule)
{
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif // _DEBUG

    MessageBox(0, "Injected!", "SUCCESS", MB_ICONINFORMATION);

    while (moduleBase == 0)
    {
        moduleBase = (uintptr_t)GetModuleHandle(NULL);
        Sleep(30);
    }

#ifdef _DEBUG
    printf("[+] base: 0x%llX\n", moduleBase);
#endif

    offsets = new Offsets();*/

    /*MODULEINFO moduleInfo;
    if (!GetModuleInformation((HANDLE)-1, GetModuleHandle(NULL), &moduleInfo, sizeof(MODULEINFO)) || !moduleInfo.lpBaseOfDll) {
#ifdef _DEBUG
        printf("[-] failed to get base module information");
        fclose(f);
        FreeConsole();
#endif

        FreeLibraryAndExitThread(hModule, 0);
        return 0;
    }

    __int64 searchStart = (__int64)moduleInfo.lpBaseOfDll;
    __int64 searchEnd = (__int64)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage;

    auto resolve_lea = [](__int64 addr) -> __int64 {
        return *(int*)(addr + 3) + addr + 7;
    };

    auto resolve_sub = [](__int64 addr) -> __int64 {
        return *(int*)(addr + 2) + addr + 6;
    };

    uintptr_t patternScanResult = find_pattern(searchStart, searchEnd, "48 63 C1 48 69 C8 60 03 00 00 48 8D ? ? ? ? ? 48 03 C8 E9");
    uintptr_t patternScanResult2 = find_pattern(searchStart, searchEnd, "74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?");

#ifdef _DEBUG
    printf("[+] pattern scan result: 0x%llX\n", patternScanResult);
    printf("[+] pattern scan result2: 0x%llX\n", patternScanResult2 - 0x17);
#endif

    uintptr_t offset = resolve_lea(patternScanResult + 0xA);
    uintptr_t offset2 = resolve_sub(patternScanResult2 - 0x17);

    offset -= 0x18;

#ifdef _DEBUG
    printf("[+] uav offset: 0x%llX (0x%llX)\n", offset, offset - moduleBase);
    printf("[+] uav offset: 0x%llX (0x%llX)\n", offset2, offset2 - moduleBase);
#endif*/

    /*while (!KEY_MODULE_EJECT)
    {
        //cg = (uintptr_t)(moduleBase + (offset - moduleBase));
        cg = (uintptr_t)(moduleBase + offsets->GetOffset(Offsets::CG_T));
        GameMode = *(int*)(moduleBase + offsets->GetOffset(Offsets::GAMEMODE));

#ifdef _DEBUG
        std::cout << *(int*)offset2 << '\n';
#endif

        if (KEY_UAV_MANAGER) 
            bUav = !bUav;

        if (bUav)
        {
            if (GameMode > 1)
            {
                if (cg != 0)
                {
                    health = *(int*)((uintptr_t)offsets->FindDMAAddy(cg, { 0x25C }));
                    if (health >= 0 && health <= 100)
                    {
                        *(int*)((uintptr_t)offsets->FindDMAAddy(cg, { 0x304 })) = 33619969;
                    }
                }
            }
        }

        Sleep(1);
    }

#ifdef _DEBUG
    fclose(f);
    FreeConsole();
#endif // _DEBUG

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}*/

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        _beginthreadex(0, 0, (_beginthreadex_proc_type)Init, 0, 0, 0);
        break;
    }

    return TRUE;
}


