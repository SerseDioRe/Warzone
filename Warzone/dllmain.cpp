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

#define QWORD unsigned __int64

Offsets* offsets = nullptr;

uintptr_t   moduleBase = 0;
uintptr_t           cg = 0;
bool              bUav = false;
int             health = 0;
int             GameMode = 0;
bool noRecoil = false;
bool triggerBot = false;
int crossHair = 0;


uint64_t DecryptClientInfo(uint64_t imageBase, uint64_t peb) // 48 8b 04 c1 48 8b 1c 03 48 8b cb 48 8b 03 ff 90 98 00 00 00
{
    uint64_t rax = imageBase, rbx = imageBase, rcx = imageBase, rdx = imageBase, r8 = imageBase, rdi = imageBase, rsi = imageBase, r9 = imageBase, r10 = imageBase, r11 = imageBase, r12 = imageBase, r13 = imageBase, r14 = imageBase, r15 = imageBase;

    rbx = *(uintptr_t*)(imageBase + 0x1EB678B8);
    if (!rbx)
        return rbx;
    rdx = ~peb;              //mov rdx, gs:[rax]
    r8 = imageBase;
    rcx = rdx;              //mov rcx, rdx
    rax = imageBase + 0x1740D79E;              //lea rax, [0x00000000151A30B0]
    rcx = ~rcx;             //not rcx
    rcx += rax;             //add rcx, rax
    rcx ^= rbx;             //xor rcx, rbx
    rcx -= rdx;             //sub rcx, rdx
    rcx -= r8;              //sub rcx, r8
    rcx -= 0xF13A;          //sub rcx, 0xF13A
    rax = rcx;              //mov rax, rcx
    rax >>= 0x13;           //shr rax, 0x13
    rcx ^= rax;             //xor rcx, rax
    rbx = rcx;              //mov rbx, rcx
    rax = 0;                //and rax, 0xFFFFFFFFC0000000
    rbx >>= 0x26;           //shr rbx, 0x26
    rax = _rotl64(rax, 0x10);               //rol rax, 0x10
    rbx ^= rcx;             //xor rbx, rcx
    rax ^= *(uintptr_t*)(imageBase + 0x72AF124);             //xor rax, [0x00000000050449EA]
    rax = _byteswap_uint64(rax);            //bswap rax
    rbx *= *(uintptr_t*)(rax + 0x7);              //imul rbx, [rax+0x07]
    rax = 0x7D037367013E30B7;               //mov rax, 0x7D037367013E30B7
    rbx *= rax;             //imul rbx, rax
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
        r12 += offsets->GetOffset(Offsets::RECOIL);
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

    //if (!Updated())
        //return NULL;

    MessageBox(0, "", "", MB_ICONINFORMATION);

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

        // DON'T USE THAT SHIT
        /*if (KEY_TRIGGERBOT_MANAGER)
        {
            triggerBot = !triggerBot;
        }*/
            
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

        /*if (triggerBot)
        {
            if(GameMode > 1)
            {
                crossHair = *(int*)(moduleBase + offsets->GetOffset(Offsets::CROSSHAIR));
                if(crossHair > 0)
                {
                    *(int*)(moduleBase + offsets->GetOffset(Offsets::SHOTSFIREASSAULT)) = 1;
                }else
                    *(int*)(moduleBase + offsets->GetOffset(Offsets::SHOTSFIREASSAULT)) = 0;
            }
        }*/

        /*int isShooting = *(int*)(moduleBase + offsets->GetOffset(Offsets::SHOTSFIREASSAULT));

        if(isShooting > 0)
        {
            *(float*)(moduleBase + 0x212004F0) = 0.0F;
            *(float*)(moduleBase + 0x21200518) = 0.0F;
            *(float*)(moduleBase + 0x21200540) = 0.0F;
        }*/

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


