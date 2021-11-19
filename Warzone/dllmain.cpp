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

#define OFFSET_CHARACTERINFO_RECOIL 0x38C84 //0F 85 ? ? ? ? 0F 2E 80 ? ? ? ? 0F 85 ? ? ? ? 4C 8D 96 ? ? ? ?

#define QWORD unsigned __int64

Offsets* offsets = nullptr;

uintptr_t   moduleBase = 0;
uintptr_t           cg = 0;
bool              bUav = false;
int             health = 0;
int             GameMode = 0;
bool noRecoil = false;


uint64_t DecryptClientInfo(uint64_t imageBase, uint64_t peb) // 48 8b 04 c1 48 8b 1c 03 48 8b cb 48 8b 03 ff 90 98 00 00 00
{
    uint64_t rax = 0ull, rbx = 0ull, rcx = 0ull, rdx = 0ull, r8 = 0ull, r9 = 0ull, r10 = 0ull;

    rbx = *(uint64_t*)(imageBase + 0x180D5F58);
    rcx = peb;
    rax = imageBase;
    rcx = ~rcx;
    r8 = imageBase + 0x5A665254;
    r8 -= rcx;
    r8 ^= rbx;
    r8 ^= rax;
    rax = 0x4608305C4A63DEBB;
    r8 *= rax;
    r8 -= rcx;
    rax = r8;
    rax >>= 0x13;
    r8 ^= rax;
    rax = r8;
    rcx = 0;
    rax >>= 0x26;
    rcx = _rotl64(rcx, 0x10);
    rax ^= r8;
    rcx ^= *(uint64_t*)(imageBase + 0x7241109);
    rcx = _byteswap_uint64(rcx);
    rbx = *(uint64_t*)(rcx + 0x7);
    rbx *= rax;

    return rbx;
}
    
void NoRecoil()
{
    auto not_peb = __readgsqword(0x60);
    uint64_t characterInfo_ptr = DecryptClientInfo(moduleBase, not_peb);
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

        if (KEY_UAV_MANAGER)
        {
            bUav = !bUav;
        }

        if (KEY_RECOIL_MANAGER)
        {
            noRecoil = !noRecoil;
        }
            

        if (bUav)
        {
            if (GameMode > 1)
            {
                if (cg != 0)
                {
                    health = *(int*)((uintptr_t)offsets->FindDMAAddy(cg, { 0x25C }));
                    if (health >= 0 && health <= 300)
                    {
                        *(int*)((uintptr_t)offsets->FindDMAAddy(cg, { 0x304 })) = 33619969;
                    }
                }
            }
        }

        if (noRecoil)
        {
            if(GameMode > 1)
               NoRecoil();
        }

        Sleep(1);
    }
   
    return true;
}

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


