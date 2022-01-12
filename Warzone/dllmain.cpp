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
bool noSpread = false;
bool triggerBot      = false;
int crossHair        = 0;

uint64_t DecryptClientInfo(uint64_t imageBase, uint64_t peb) // 48 8b 04 c1 48 8b 1c 03 48 8b cb 48 8b 03 ff 90 98 00 00 00
{
    uint64_t rax = imageBase, rbx = imageBase, rcx = imageBase, rdx = imageBase, r8 = imageBase, rdi = imageBase, rsi = imageBase, r9 = imageBase, r10 = imageBase, r11 = imageBase, r12 = imageBase, r13 = imageBase, r14 = imageBase, r15 = imageBase;

    rbx = *(uintptr_t*)(imageBase + 0x1EF32838);
    if (!rbx)
        return rbx;
    rdx = peb;              //mov rdx, gs:[rax]
    r9 = imageBase + 0xEF81;           //lea r9, [0xFFFFFFFFFD94EE8D]
    rax = rbx;              //mov rax, rbx
    rax >>= 0x15;           //shr rax, 0x15
    rbx ^= rax;             //xor rbx, rax
    rcx = rbx;              //mov rcx, rbx
    r8 = 0;                 //and r8, 0xFFFFFFFFC0000000
    rcx >>= 0x2A;           //shr rcx, 0x2A
    rcx ^= rbx;             //xor rcx, rbx
    r8 = _rotl64(r8, 0x10);                 //rol r8, 0x10
    r8 ^= *(uintptr_t*)(imageBase + 0x76730C2);              //xor r8, [0x0000000004FB2F9E]
    rax = 0x3C38AA1670A95F19;               //mov rax, 0x3C38AA1670A95F19
    rbx = rdx;              //mov rbx, rdx
    rbx = ~rbx;             //not rbx
    rbx *= r9;              //imul rbx, r9
    r8 = _byteswap_uint64(r8);              //bswap r8
    rbx += rcx;             //add rbx, rcx
    rbx -= rdx;             //sub rbx, rdx
    rbx *= rax;             //imul rbx, rax
    rax = 0x4AA02D69120472A5;               //mov rax, 0x4AA02D69120472A5
    rbx -= rax;             //sub rbx, rax
    rbx *= *(uintptr_t*)(r8 + 0x11);              //imul rbx, [r8+0x11]
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

    WeaponCompleteDefArr* weapons = (WeaponCompleteDefArr*)(moduleBase + offsets->GetOffset(Offsets::WEAPON_COMPLETE_DEF));

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

            if(noRecoil)
            {
                for (auto w : weapons->weaponCompleteDefArr)
                {
                    if (w->weapDef)
                    {
                        for(int i = 0; i < 6; i++)
                        {
                            // CORRECT
                            w->weapDef->hipAngularGunKickDir[i] = 0.0F;          // 0x9BC
                            w->weapDef->hipAngularGunKickDev[i] = 0.0F;          // 0x9D4
                            w->weapDef->hipAngularGunKickStrengthMin[i] = 0.0F;
                            w->weapDef->hipAngularGunKickStrengthMax[i] = 0.0F;
                            w->weapDef->hipAngularGunKickPitchScale[i] = 0.0F;
                            w->weapDef->adsAngularGunKickDir[i] = 0.0F;
                            w->weapDef->adsAngularGunKickDev[i] = 0.0F;
                            w->weapDef->adsAngularGunKickStrengthMin[i] = 0.0F;
                            w->weapDef->adsAngularGunKickStrengthMax[i] = 0.0F;
                            w->weapDef->adsAngularGunKickPitchScale[i] = 0.0F;
                        }
                    }
                }
            }
        }

        if(KEY_SPREAD_MANAGER)
        {
            noSpread = !noSpread;

            if(noSpread)
            {
                for(auto w : weapons->weaponCompleteDefArr)
                {
                    if(w->weapDef)
                    {
                        w->weapDef->fHipSpreadDuckedDecay = 0.0F; // start no spread
                        w->weapDef->fHipSpreadProneDecay = 0.0F;
                        w->weapDef->hipSpreadSprintDecay = 0.0F;
                        w->weapDef->hipSpreadInAirDecay = 0.0F;
                        w->weapDef->fHipReticleSidePos = 0.0F;
                        w->weapDef->fAdsIdleAmount = 0.0F;
                        w->weapDef->fHipIdleAmount = 0.0F;
                        w->weapDef->adsIdleSpeed = 0.0F;
                        w->weapDef->hipIdleSpeed = 0.0F;
                        w->weapDef->fIdleCrouchFactor = 0.0F;
                        w->weapDef->fIdleProneFactor = 0.0F;
                        w->weapDef->fGunMaxPitch = 0.0F;
                        w->weapDef->fGunMaxYaw = 0.0F; // start no spread during moving and shooting
                        w->weapDef->fViewMaxPitch = 0.0F;
                        w->weapDef->fViewMaxYaw = 0.0F;
                        w->weapDef->adsIdleLerpStartTime = 0.0F;
                        w->weapDef->adsIdleLerpTime = 0.0F;
                        w->weapDef->slideSpreadMin = 0.0F;
                        w->weapDef->slideSpreadMax = 0.0F;
                        w->weapDef->slideSpreadDecayRate = 0.0F;
                        w->weapDef->slideSpreadFireAdd = 0.0F;
                        w->weapDef->slideSpreadTurnAdd = 0.0F; // end no spread

                        w->weapDef->swaySettings.hip.maxAngleSteadyAim = 0.0F;
                        w->weapDef->swaySettings.ads.adsSwayScale[0] = 0.0F;
                        w->weapDef->swaySettings.ads.adsSwayScale[1] = 0.0F;
                        w->weapDef->swaySettings.ads.adsSwayScale[2] = 0.0F;
                        w->weapDef->swaySettings.ads.swayTransitionLerpSpeed = 0.0F;
                        w->weapDef->swaySettings.shellShockScale = 0.0F;
                    }
                }
            }
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

        /*if (noRecoil)
        {
            if(numOfPlayers > 4)
               NoRecoil();
        }*/

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

        /*for (auto w : weapons->Weapons)
        {
            if (w)
            {
                w->iFireDelay = 200;
                w->iFireDelayAkimbo = 200;
                w->iFireTime = 200;
                w->iFireTimeAkimbo = 200;
                w->sprintOutTimerScale = 1000.0F;
            }
        }*/

        //for (auto w : weapons->weaponCompleteDefArr)
        //{
            //if (w->weapDef)
            //{
                // SPREAD
                /*w->weapDef->spread = 0.0F;
                w->weapDef->spread2 = 0.0F;
                w->weapDef->spread3 = 0.0F;
                w->weapDef->spread4 = 0.0F;
                w->weapDef->spread5 = 0.0F;
                w->weapDef->spread6 = 0.0F;
                w->weapDef->spread7 = 0.0F;
                w->weapDef->spread8 = 0.0F;
                w->weapDef->spread9 = 0.0F;
                w->weapDef->spread10 = 0.0F;
                w->weapDef->spread11 = 0.0F;
                w->weapDef->spread12 = 0.0F;
                w->weapDef->spread13 = 0.0F;
                w->weapDef->spread14 = 0.0F;
                w->weapDef->spread15 = 0.0F;
                w->weapDef->spread16 = 0.0F;
                w->weapDef->spread17 = 0.0F;
                w->weapDef->spread18 = 0.0F;
                w->weapDef->spread19 = 0.0F;
                w->weapDef->spread20 = 0.0F;
                w->weapDef->spread21 = 0.0F;
                w->weapDef->spread22 = 0.0F;
                w->weapDef->spread23 = 0.0F;
                w->weapDef->spread24 = 0.0F;
                w->weapDef->spread25 = 0.0F;
                w->weapDef->spread26 = 0.0F;
                w->weapDef->spread27 = 0.0F;
                w->weapDef->spread28 = 0.0F;
                w->weapDef->spread29 = 0.0F;
                w->weapDef->spread30 = 0.0F;
                w->weapDef->spread31 = 0.0F;
                w->weapDef->spread32 = 0.0F;
                w->weapDef->spread33 = 0.0F;
                w->weapDef->spread34 = 0.0F;
                w->weapDef->spread35 = 0.0F;
                w->weapDef->spread36 = 0.0F;
                w->weapDef->spread37 = 0.0F;
                w->weapDef->spread38 = 0.0F;
                w->weapDef->spread39 = 0.0F;
                w->weapDef->spread40 = 0.0F;
                w->weapDef->spread41 = 0.0F;
                w->weapDef->spread42 = 0.0F;
                w->weapDef->spread43 = 0.0F;
                w->weapDef->spread44 = 0.0F;
                w->weapDef->spread45 = 0.0F;
                w->weapDef->spread46 = 0.0F;
                w->weapDef->spread47 = 0.0F;
                w->weapDef->spread48 = 0.0F;
                w->weapDef->spread49 = 0.0F;
                w->weapDef->spread50 = 0.0F;
                w->weapDef->spread51 = 0.0F;
                w->weapDef->spread52 = 0.0F;
                w->weapDef->spread53 = 0.0F;*/

                // CRASH
                /*w->weapDef->spread54 = 0.0F;
                w->weapDef->spread55 = 0.0F;
                w->weapDef->spread56 = 0.0F;
                w->weapDef->spread57 = 0.0F;
                w->weapDef->spread58 = 0.0F;
                w->weapDef->spread59 = 0.0F;
                w->weapDef->spread60 = 0.0F;
                w->weapDef->spread61 = 0.0F;
                w->weapDef->spread62 = 0.0F;
                w->weapDef->spread63 = 0.0F;
                w->weapDef->spread64 = 0.0F;
                w->weapDef->spread65 = 0.0F;
                w->weapDef->spread66 = 0.0F;
                w->weapDef->spread67 = 0.0F;
                w->weapDef->spread68 = 0.0F;
                w->weapDef->spread69 = 0.0F;
                w->weapDef->spread70 = 0.0F;
                w->weapDef->spread71 = 0.0F;
                w->weapDef->spread72 = 0.0F;
                w->weapDef->spread73 = 0.0F;
                w->weapDef->spread74 = 0.0F;
                w->weapDef->spread75 = 0.0F;
                w->weapDef->spread76 = 0.0F;
                w->weapDef->spread77 = 0.0F;
                w->weapDef->spread78 = 0.0F;
                w->weapDef->spread79 = 0.0F;
                w->weapDef->spread80 = 0.0F;
                w->weapDef->spread81 = 0.0F;
                w->weapDef->spread82 = 0.0F;
                w->weapDef->spread83 = 0.0F;
                w->weapDef->spread84 = 0.0F;
                w->weapDef->spread85 = 0.0F;
                w->weapDef->spread86 = 0.0F;
                w->weapDef->spread87 = 0.0F;
                w->weapDef->spread88 = 0.0F;
                w->weapDef->spread89 = 0.0F;
                w->weapDef->spread90 = 0.0F;
                w->weapDef->spread91 = 0.0F;*/

                // NO RECOIL
                /*w->weapDef->spread92 = 0.0F;
                w->weapDef->spread93 = 0.0F;
                w->weapDef->spread94 = 0.0F;
                w->weapDef->spread95 = 0.0F;
                w->weapDef->spread96 = 0.0F;
                w->weapDef->spread97 = 0.0F;
                w->weapDef->spread98 = 0.0F;
                w->weapDef->spread99 = 0.0F;
                w->weapDef->spread100 = 0.0F;
                w->weapDef->spread101 = 0.0F;
                w->weapDef->spread102 = 0.0F;
                w->weapDef->spread103 = 0.0F;
                w->weapDef->spread104 = 0.0F;
                w->weapDef->spread105 = 0.0F;
                w->weapDef->spread106 = 0.0F;
                w->weapDef->spread107 = 0.0F;
                w->weapDef->spread108 = 0.0F;
                w->weapDef->spread109 = 0.0F;
                w->weapDef->spread110 = 0.0F;
                w->weapDef->spread111 = 0.0F;
                w->weapDef->spread112 = 0.0F;
                w->weapDef->spread113 = 0.0F;
                w->weapDef->spread114 = 0.0F;
                w->weapDef->spread115 = 0.0F;
                w->weapDef->spread116 = 0.0F;
                w->weapDef->spread117 = 0.0F;
                w->weapDef->spread118 = 0.0F;
                w->weapDef->spread119 = 0.0F;
                w->weapDef->spread120 = 0.0F;
                w->weapDef->spread121 = 0.0F;
                w->weapDef->spread122 = 0.0F;
                w->weapDef->spread123 = 0.0F;
                w->weapDef->spread124 = 0.0F;
                w->weapDef->spread125 = 0.0F;
                w->weapDef->spread126 = 0.0F;
                w->weapDef->spread127 = 0.0F;
                w->weapDef->spread128 = 0.0F;
                w->weapDef->spread129 = 0.0F;
                w->weapDef->spread130 = 0.0F;
                w->weapDef->spread131 = 0.0F;
                w->weapDef->spread132 = 0.0F;
                w->weapDef->spread133 = 0.0F;
                w->weapDef->spread134 = 0.0F;
                w->weapDef->spread135 = 0.0F;
                w->weapDef->spread136 = 0.0F;
                w->weapDef->spread137 = 0.0F;
                w->weapDef->spread138 = 0.0F;
                w->weapDef->spread139 = 0.0F;
                w->weapDef->spread140 = 0.0F;
                w->weapDef->spread141 = 0.0F;
                w->weapDef->spread142 = 0.0F;
                w->weapDef->spread143 = 0.0F;
                w->weapDef->spread144 = 0.0F;
                w->weapDef->spread145 = 0.0F;
                w->weapDef->spread146 = 0.0F;
                w->weapDef->spread147 = 0.0F;
                w->weapDef->spread148 = 0.0F;
                w->weapDef->spread149 = 0.0F;
                w->weapDef->spread150 = 0.0F;
                w->weapDef->spread151 = 0.0F;
                w->weapDef->spread152 = 0.0F;
                w->weapDef->spread153 = 0.0F;
                w->weapDef->spread154 = 0.0F;
                w->weapDef->spread155 = 0.0F;
                w->weapDef->spread156 = 0.0F;
                w->weapDef->spread157 = 0.0F;
                w->weapDef->spread158 = 0.0F;
                w->weapDef->spread159 = 0.0F;
                w->weapDef->spread160 = 0.0F;
                w->weapDef->spread161 = 0.0F;
                w->weapDef->spread162 = 0.0F;
                w->weapDef->spread163 = 0.0F;
                w->weapDef->spread164 = 0.0F;
                w->weapDef->spread165 = 0.0F;
                w->weapDef->spread166 = 0.0F;
                w->weapDef->spread167 = 0.0F;
                w->weapDef->spread168 = 0.0F;*/

                // TEST
                /*w->weapDef->spread169 = 0.0F;
                w->weapDef->spread170 = 0.0F;
                w->weapDef->spread171 = 0.0F;
                w->weapDef->spread172 = 0.0F;
                w->weapDef->spread173 = 0.0F;
                w->weapDef->spread174 = 0.0F;
                w->weapDef->spread175 = 0.0F;
                w->weapDef->spread176 = 0.0F;
                w->weapDef->spread177 = 0.0F;
                w->weapDef->spread178 = 0.0F;
                w->weapDef->spread179 = 0.0F;
                w->weapDef->spread180 = 0.0F;
                w->weapDef->spread181 = 0.0F;
                w->weapDef->spread182 = 0.0F;
                w->weapDef->spread183 = 0.0F;
                w->weapDef->spread184 = 0.0F;
                w->weapDef->spread185 = 0.0F;
                w->weapDef->spread186 = 0.0F;
                w->weapDef->spread187 = 0.0F;
                w->weapDef->spread188 = 0.0F;
                w->weapDef->spread189 = 0.0F;
                w->weapDef->spread190 = 0.0F;
                w->weapDef->spread191 = 0.0F;
                w->weapDef->spread192 = 0.0F;
                w->weapDef->spread193 = 0.0F;
                w->weapDef->spread194 = 0.0F;
                w->weapDef->spread195 = 0.0F;
                w->weapDef->spread196 = 0.0F;
                w->weapDef->spread197 = 0.0F;
                w->weapDef->spread198 = 0.0F;
                w->weapDef->spread199 = 0.0F;
                w->weapDef->spread200 = 0.0F;
                w->weapDef->spread201 = 0.0F;
                w->weapDef->spread202 = 0.0F;
                w->weapDef->spread203 = 0.0F;
                w->weapDef->spread204 = 0.0F;
                w->weapDef->spread205 = 0.0F;
                w->weapDef->spread206 = 0.0F;
                w->weapDef->spread207 = 0.0F;
                w->weapDef->spread208 = 0.0F;*/




            //}
        //}

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


