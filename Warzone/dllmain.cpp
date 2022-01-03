// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"
#include "Classes.h"
#include "Offsets.h"
#include "Memory.h"
#include <psapi.h>
#include <process.h>

#define QWORD unsigned __int64

Offsets* offsets           = nullptr;
playerState_s* playerState = nullptr;

uintptr_t moduleBase = 0;
bool            bUav = false;
int           health = 0;
int     numOfPlayers = 0;
bool        noRecoil = false;
bool triggerBot      = false;
int crossHair        = 0;

uint64_t DecryptClientInfo(uint64_t imageBase, uint64_t peb) // 48 8b 04 c1 48 8b 1c 03 48 8b cb 48 8b 03 ff 90 98 00 00 00
{
    uint64_t rax = imageBase, rbx = imageBase, rcx = imageBase, rdx = imageBase, r8 = imageBase, rdi = imageBase, rsi = imageBase, r9 = imageBase, r10 = imageBase, r11 = imageBase, r12 = imageBase, r13 = imageBase, r14 = imageBase, r15 = imageBase;

    rbx = *(uintptr_t*)(imageBase + 0x1F0D3938);
    if (!rbx)
        return rbx;
    r8 = peb;               //mov r8, gs:[rax]
    r9 = imageBase;    rdx = 0x22CBFA5C133D766B;               //mov rdx, 0x22CBFA5C133D766B
    rax = rbx;              //mov rax, rbx
    rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
    rax >>= 0x25;           //shr rax, 0x25
    rbx ^= rax;             //xor rbx, rax
    rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
    rcx ^= *(uintptr_t*)(imageBase + 0x78210EC);             //xor rcx, [0x0000000005045739]
    rax = 0x2C925E0599A4412F;               //mov rax, 0x2C925E0599A4412F
    rbx *= rdx;             //imul rbx, rdx
    rcx = ~rcx;             //not rcx
    rbx += rax;             //add rbx, rax
    rbx *= *(uintptr_t*)(rcx + 0x9);              //imul rbx, [rcx+0x09]
    rbx -= r8;              //sub rbx, r8
    rbx += r9;              //add rbx, r9
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

    while (!(KEY_MODULE_EJECT))
    {
        numOfPlayers = *(int*)           (moduleBase + offsets->GetOffset(Offsets::NUM_OF_PLAYERS));
        playerState  = *(playerState_s**)(moduleBase + offsets->GetOffset(Offsets::PLAYERSTATE_S));

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
            if (numOfPlayers > 4)
            {
                if(playerState)
                {
                    if(playerState->Health >= 0 && playerState->Health <= 300)
                    {
                        //playerState->radarEnabled = true;
                        playerState->radarShowEnemyDirection = true;
                        playerState->radarMode = RadarMode::RADAR_MODE_CONSTANT;
                    }
                }
            }
        }

        if (noRecoil)
        {
            if(numOfPlayers > 4)
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


