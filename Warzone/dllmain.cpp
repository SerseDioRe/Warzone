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

#define OFFSET_CHARACTERINFO_RECOIL 0x810BC //0F 85 ? ? ? ? 0F 2E 80 ? ? ? ? 0F 85 ? ? ? ? 4C 8D 96 ? ? ? ?

#define QWORD unsigned __int64

Offsets* offsets = nullptr;

uintptr_t   moduleBase = 0;
uintptr_t           cg = 0;
bool              bUav = false;
int             health = 0;
int             GameMode = 0;
bool noRecoil = false;


uint64_t DecryptCharacterInfoPtr(uint64_t imageBase, uint64_t peb) // 48 8b 04 c1 48 8b 1c 03 48 8b cb 48 8b 03 ff 90 98 00 00 00
{
    uint64_t RAX = imageBase, RBX = imageBase, RCX = imageBase, RDX = imageBase, R8 = imageBase, RDI = imageBase, RSI = imageBase, R9 = imageBase, R10 = imageBase, R11 = imageBase, R12 = imageBase, R13 = imageBase, R14 = imageBase, R15 = imageBase;

    RBX = *(uint64_t*)(imageBase + 0x18003B58);
    if (RBX == 0) {
        return 0;
    }
    uint64_t RSP_0x58 = imageBase;
    RSP_0x58 = 0xC; // mov byte ptr [rsp+58h],0Ch
    // movzx eax,byte ptr [rsp+58h]
    RAX = _rotr64(RAX, 0xAD);
    // movzx eax,al
    RDX = peb; // mov rdx,gs:[rax]
    RAX = RBX;
    RAX >>= 0x20;
    RBX ^= RAX;
    RAX = RBX;
    RAX >>= 0x28;
    RCX = 0x0;
    RAX ^= RDX;
    RCX = _rotl64(RCX, 0x10);
    RCX ^= *(uint64_t*)(imageBase + 0x73A10F3);
    RAX ^= RBX;
    RDX = 0xC3A82EA730153563;
    RAX *= RDX;
    RCX = _byteswap_uint64(RCX);
    RDX = 0x7585B6D633F56AA9;
    RAX += RDX;
    RBX = *(uint64_t*)(RCX + 0x7);
    RBX *= RAX;
    return RBX;

    /*uint64_t RAX = imageBase, RBX = imageBase, RCX = imageBase, RDX = imageBase, R8 = imageBase, RDI = imageBase, RSI = imageBase, R9 = imageBase, R10 = imageBase, R11 = imageBase, R12 = imageBase, R13 = imageBase, R14 = imageBase, R15 = imageBase;

    RBX = *(uint64_t*)(imageBase + 0x18003B58);   // mov     rbx, cs:qword_18003B58
    RDX = peb;                                    // mov     rdx, gs:[rax]
    if (RBX == 0) {                               // test    rbx, rbx
        return 0;
    }
    RCX = *(uint64_t*)(imageBase + 0x168);        // mov     rcx, [rbp+168h]  ??
    RAX = RBX;                                    // mov     rax, rbx
    RAX >>= 0x20;                                 // shr     rax, 20h
    RBX ^= RAX;                                   // xor     rbx, rax
    RAX = (imageBase + 0x3BB);                    // lea     rax, cs:3BBh
    RCX -= RAX;                                   // sub     rcx, rax
    RAX = RBX;                                    // mov     rax, rbx
    RAX >= 0x28;                                  // shr     rax, 28h
    RCX &= 0x0FFFFFFFFC0000000;                   // and     rcx, 0FFFFFFFFC0000000h
    RAX ^= RDX;                                   // xor     rax, rdx
    RCX = _rotl64(RCX, 0x10);                     // rol     rcx, 10h
    RCX ^= *(uint64_t*)(imageBase + 0x73A10F3);   // xor     rcx, cs:qword_73A10F3
    RAX ^= RBX;                                   // xor     rax, rbx
    RDX = 0x0C3A82EA730153563;                    // mov     rdx, 0C3A82EA730153563h
    RAX *= RDX;                                   // imul    rax, rdx
    _byteswap_uint64(RCX);                        // bswap   rcx
    RDX = 0x7585B6D633F56AA9;                     // mov     rdx, 7585B6D633F56AA9h
    RAX += RDX;                                   // add     rax, rdx
    RBX = *(uint64_t*)(RCX + 0x7);                // mov     rbx, [rcx+7]
    RBX *= RAX;                                   // imul    rbx, rax
    return RBX;*/


    /*uint64_t RAX = imageBase, RBX = imageBase, RCX = imageBase, RDX = imageBase, R8 = imageBase, RDI = imageBase, RSI = imageBase, R9 = imageBase, R10 = imageBase, R11 = imageBase, R12 = imageBase, R13 = imageBase, R14 = imageBase, R15 = imageBase;

    RBX = *(uint64_t*)(imageBase + 0x17E21C88); //driver::read<uint64_t>(imageBase + 0x18044A88); 48 8B 1D ? ? ? ? 89 7D ? || 48 8b 04 c1 48 8b 1c 03 48 8b cb 48 8b 03 ff 90 98 00 00 00
    if (RBX == 0) {
        return 0;
    }
    RCX = peb;
    RAX = 0x507343EDDC5832C3;
    RCX *= RAX;
    RAX = 0x214C49CDB728D2B5;
    RBX *= RAX;
    // lea     rax, cs:0E01h
    RBX -= RCX;
    // mov     rcx, [rbp+5Fh]
    RCX -= RAX;
    RAX = RBX;
    RAX >>= 0x20;
    RCX &= 0x0FFFFFFFFC0000000;
    RAX ^= RBX;
    RCX = _rotl64(RCX, 0x10);
    RCX ^= *(uint64_t*)(imageBase + 0x71BC0E7);
    RCX = ~RCX;
    RBX = *(uint64_t*)(RCX + 0x13); 
    RBX *= RAX;
    RAX = RBX;
    RAX >>= 0x16;
    RBX ^= RAX;
    RAX = RBX;
    RAX >>= 0x2C;
    RBX ^= RAX;
    return RBX;*/

    /*RBX = *(uint64_t*)(imageBase + 0x18044A88); //driver::read<uint64_t>(imageBase + 0x18044A88); 48 8B 1D ? ? ? ? 89 7D ?
    if (RBX == 0) {
        return 0;
    }
    RAX = imageBase;
    RDX = RBX + RAX;
    RAX = 0xF7722301C0F805AF;
    RDX *= RAX;
    RAX = RDX;
    RAX >>= 0x5;
    RDX ^= RAX;
    RAX = RDX;
    RAX >>= 0xA;
    RDX ^= RAX;
    RAX = RDX;
    RAX >>= 0x14;
    RDX ^= RAX;
    RAX = RDX;
    RAX >>= 0x28;
    RDX ^= RAX;
    RAX = RDX;
    RAX >>= 0x12;
    RDX ^= RAX;
    RAX = RDX;
    RCX = 0x0;
    RAX >>= 0x24;
    RCX = _rotl64(RCX, 0x10);
    RAX ^= RDX;
    RCX ^= *(uint64_t*)(imageBase + 0x7425100); //driver::read<uint64_t>(imageBase + 0x7425100);
    RCX = ~RCX;
    RBX = *(uint64_t*)(RCX + 0xB); //driver::read<uint64_t>(RCX + 0xB);
    RBX *= RAX;
    RAX = 0xF02663FD564FD7EB;
    RBX *= RAX;
    return RBX;*/
}
    

void NoRecoil()
{
    auto not_peb = __readgsqword(0x60);
    uint64_t characterInfo_ptr = DecryptCharacterInfoPtr(moduleBase, not_peb);
    if (characterInfo_ptr)
    {
        // up, down
        QWORD r12 = characterInfo_ptr;
        r12 += OFFSET_CHARACTERINFO_RECOIL;
        QWORD rsi = r12 + 0x4;
        DWORD edx = *(QWORD*)(r12 + 0xC);
        DWORD ecx = (DWORD)r12;
        ecx ^= edx;
        DWORD eax = (DWORD)((QWORD)ecx + 0x2);
        eax *= ecx;
        ecx = (DWORD)rsi;
        ecx ^= edx;
        DWORD udZero = eax;
        //left, right
        eax = (DWORD)((QWORD)ecx + 0x2);
        eax *= ecx;
        DWORD lrZero = eax;
        *(DWORD*)(r12) = udZero;
        *(DWORD*)(rsi) = lrZero;
    }
}

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

bool Updated()
{
    BYTE m_checkUpdate[2] = { 0x74, 0x1D };

    for (int count{ 0 }; count < 2; ++count)
    {
        if (((BYTE*)(moduleBase + offsets->GetOffset(Offsets::CHECKUPDATE)))[count] == m_checkUpdate[count])
            return true;
    }

    return false;
}

ULONG WINAPI Init()
{
    while (moduleBase == 0)
    {
        moduleBase = (uintptr_t)GetModuleHandle(NULL);
        Sleep(30);
    }

    offsets = new Offsets();

    if (!Updated())
        return NULL;

    while (!KEY_MODULE_EJECT)
    {
        cg = (uintptr_t)(moduleBase + offsets->GetOffset(Offsets::CG_T));
        GameMode = *(int*)(moduleBase + offsets->GetOffset(Offsets::GAMEMODE));

#ifdef _DEBUG
        std::cout << *(int*)offset2 << '\n';
#endif

        if (KEY_UAV_MANAGER)
            bUav = !bUav;

        if (KEY_RECOIL_MANAGER)
            noRecoil = !noRecoil;

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

        if(noRecoil)
        {
            if(GameMode > 1)
               NoRecoil();
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


