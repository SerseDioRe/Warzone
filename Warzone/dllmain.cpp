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
uintptr_t isShootingAddress = 0;

float newAngleX = 85.0F;
float newAngleY = 85.0F;

uintptr_t moduleBase = 0;
bool            bUav = false;
int           health = 0;
int     numOfPlayers = 0;
bool        noRecoil = false;
bool noSpread = false;
bool triggerBot      = false;
int crossHair        = 0;

float valuesRecoilBackup[962][60];
float valuesSpreadBackup[962][22];

uintptr_t jmpBackAddyViewAngles;

void __declspec(naked) writeableViewAngles() // 14 bytes
{
    __asm
    {
        movss xmm0, newAngleX;
        movss   dword ptr[rbx + rdi * 4 + 0x000001DC], xmm0;
        movss xmm0, newAngleY;
        movss   dword ptr[rbx + rdi * 4 + 0x000001E0], xmm0;
        jmp[jmpBackAddyViewAngles];
    }
}

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

void* DetourFunction64(void* pSource, void* pDestination, int dwLen)
{
    DWORD MinLen = 14;

    if (dwLen < MinLen) return NULL;

    BYTE stub[] = {
    0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
    };

    void* pTrampoline = VirtualAlloc(0, dwLen + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    DWORD dwOld = 0;
    VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

    DWORD64 retto = (DWORD64)pSource + dwLen;

    // trampoline
    memcpy(stub + 6, &retto, 8);
    memcpy((void*)((DWORD_PTR)pTrampoline), pSource, dwLen);
    memcpy((void*)((DWORD_PTR)pTrampoline + dwLen), stub, sizeof(stub));

    // orig
    memcpy(stub + 6, &pDestination, 8);
    memcpy(pSource, stub, sizeof(stub));

    for (int i = MinLen; i < dwLen; i++)
    {
        *(BYTE*)((DWORD_PTR)pSource + i) = 0x90;
    }

    VirtualProtect(pSource, dwLen, dwOld, &dwOld);
    return (void*)((DWORD_PTR)pTrampoline);
}

void PatchBytes(BYTE* destination, BYTE* source, size_t size)
{
    DWORD oldProtect;
    VirtualProtect((LPVOID)destination, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(destination, source, size);
    VirtualProtect(destination, size, oldProtect, &oldProtect);
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

    if (!Updated())
        return NULL;

    // FINALLY AIMBOT VIEW ANGLES
    //2A57977
    //PatchBytes((BYTE*)moduleBase + 0x2983391, (BYTE*)"\xF3\x0F\x10\x05\x49\x3F\x21\x1E\xF3\x0F\x11\x83\xDC\x01\x00\x00\xF3\x0F\x10\x05\x3D\x3F\x21\x1E\xF3\x0F\x11\x83\xE0\x01\x00\x00\x90\x90\x90", 35);
    //PatchBytes((BYTE*)moduleBase + 0x2A57977, (BYTE*)"\xF3\x0F\x10\x05\x49\x3F\x21\x1E\xF3\x0F\x11\x83\xDC\x01\x00\x00\xF3\x0F\x10\x05\x3D\x3F\x21\x1E\xF3\x0F\x11\x83\xE0\x01\x00\x00\x90\x90\x90\x90", 36);
    //PatchBytes((BYTE*)moduleBase + offsets->GetOffset(Offsets::WRITE_VIEW_ANGLES), (BYTE*)"\xF3\x0F\x10\x05\x49\x3F\x21\x1E\xF3\x0F\x11\x83\xDC\x01\x00\x00\xF3\x0F\x10\x05\x3D\x3F\x21\x1E\xF3\x0F\x11\x83\xE0\x01\x00\x00\x90\x90\x90\x90", 36);

    while (!(KEY_MODULE_EJECT))
    {
        numOfPlayers = *(int*)           (moduleBase + offsets->GetOffset(Offsets::NUM_OF_PLAYERS));
        playerState  = *(playerState_s**)(moduleBase + offsets->GetOffset(Offsets::PLAYERSTATE_S));

        if (KEY_UAV_MANAGER)
        {
            bUav = !bUav;
        }

        // FINALLY AIMBOT VIEW ANGLES
        //*(float*)(moduleBase + 0x218E0A7C) = 85.0F;
        //*(float*)(moduleBase + 0x218E0A80) = 85.0F;

        if (KEY_RECOIL_MANAGER)
        {
            //2D5AE70
            noRecoil = !noRecoil;

            if(noRecoil)
            {
                //for (auto w : weapons->weaponCompleteDefArr)
                for(int count = 0; count < 962; count++)
                {
                    //if (w->weapDef)
                    if(weapons->weaponCompleteDefArr[count]->weapDef)
                    {
                        // BACKUP
                        valuesRecoilBackup[count][0]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[0];
                        valuesRecoilBackup[count][1]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[1];
                        valuesRecoilBackup[count][2]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[2];
                        valuesRecoilBackup[count][3]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[3];
                        valuesRecoilBackup[count][4]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[4];
                        valuesRecoilBackup[count][5]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[5];
                        valuesRecoilBackup[count][6]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[0];
                        valuesRecoilBackup[count][7]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[1];
                        valuesRecoilBackup[count][8]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[2];
                        valuesRecoilBackup[count][9]  = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[3];
                        valuesRecoilBackup[count][10] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[4];
                        valuesRecoilBackup[count][11] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[5];
                        valuesRecoilBackup[count][12] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[0];
                        valuesRecoilBackup[count][13] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[1];
                        valuesRecoilBackup[count][14] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[2];
                        valuesRecoilBackup[count][15] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[3];
                        valuesRecoilBackup[count][16] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[4];
                        valuesRecoilBackup[count][17] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[5];
                        valuesRecoilBackup[count][18] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[0];
                        valuesRecoilBackup[count][19] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[1];
                        valuesRecoilBackup[count][20] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[2];
                        valuesRecoilBackup[count][21] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[3];
                        valuesRecoilBackup[count][22] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[4];
                        valuesRecoilBackup[count][23] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[5];
                        valuesRecoilBackup[count][24] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[0];
                        valuesRecoilBackup[count][25] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[1];
                        valuesRecoilBackup[count][26] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[2];
                        valuesRecoilBackup[count][27] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[3];
                        valuesRecoilBackup[count][28] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[4];
                        valuesRecoilBackup[count][29] = weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[5];
                        valuesRecoilBackup[count][30] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[0];
                        valuesRecoilBackup[count][31] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[1];
                        valuesRecoilBackup[count][32] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[2];
                        valuesRecoilBackup[count][33] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[3];
                        valuesRecoilBackup[count][34] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[4];
                        valuesRecoilBackup[count][35] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[5];
                        valuesRecoilBackup[count][36] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[0];
                        valuesRecoilBackup[count][37] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[1];
                        valuesRecoilBackup[count][38] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[2];
                        valuesRecoilBackup[count][39] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[3];
                        valuesRecoilBackup[count][40] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[4];
                        valuesRecoilBackup[count][41] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[5];
                        valuesRecoilBackup[count][42] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[0];
                        valuesRecoilBackup[count][43] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[1];
                        valuesRecoilBackup[count][44] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[2];
                        valuesRecoilBackup[count][45] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[3];
                        valuesRecoilBackup[count][46] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[4];
                        valuesRecoilBackup[count][47] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[5];
                        valuesRecoilBackup[count][48] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[0];
                        valuesRecoilBackup[count][49] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[1];
                        valuesRecoilBackup[count][50] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[2];
                        valuesRecoilBackup[count][51] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[3];
                        valuesRecoilBackup[count][52] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[4];
                        valuesRecoilBackup[count][53] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[5];
                        valuesRecoilBackup[count][54] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[0];
                        valuesRecoilBackup[count][55] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[1];
                        valuesRecoilBackup[count][56] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[2];
                        valuesRecoilBackup[count][57] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[3];
                        valuesRecoilBackup[count][58] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[4];
                        valuesRecoilBackup[count][59] = weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[5];

                        for(int countGunKick = 0; countGunKick < 6; countGunKick++)
                        {
                            // WRITE
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[countGunKick]         = 0.0F; // 0x9BC
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[countGunKick]         = 0.0F;
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[countGunKick] = 0.0F;
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[countGunKick] = 0.0F;
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[countGunKick]  = 0.0F;
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[countGunKick]         = 0.0F;
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[countGunKick]         = 0.0F;
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[countGunKick] = 0.0F;
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[countGunKick] = 0.0F;
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[countGunKick]  = 0.0F;
                        }
                    }
                }
            }
            else
            {
                for (int count = 0; count < 962; count++)
                {
                    //if (w->weapDef)
                    if (weapons->weaponCompleteDefArr[count]->weapDef)
                    {
                        // CORRECT
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[0]         = valuesRecoilBackup[count][0];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[1]         = valuesRecoilBackup[count][1];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[2]         = valuesRecoilBackup[count][2];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[3]         = valuesRecoilBackup[count][3];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[4]         = valuesRecoilBackup[count][4];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[5]         = valuesRecoilBackup[count][5];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[0]         = valuesRecoilBackup[count][6];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[1]         = valuesRecoilBackup[count][7];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[2]         = valuesRecoilBackup[count][8];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[3]         = valuesRecoilBackup[count][9];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[4]         = valuesRecoilBackup[count][10];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[5]         = valuesRecoilBackup[count][11];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[0] = valuesRecoilBackup[count][12];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[1] = valuesRecoilBackup[count][13];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[2] = valuesRecoilBackup[count][14];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[3] = valuesRecoilBackup[count][15];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[4] = valuesRecoilBackup[count][16];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[5] = valuesRecoilBackup[count][17];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[0] = valuesRecoilBackup[count][18];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[1] = valuesRecoilBackup[count][19];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[2] = valuesRecoilBackup[count][20];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[3] = valuesRecoilBackup[count][21];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[4] = valuesRecoilBackup[count][22];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[5] = valuesRecoilBackup[count][23];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[0]  = valuesRecoilBackup[count][24];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[1]  = valuesRecoilBackup[count][25];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[2]  = valuesRecoilBackup[count][26];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[3]  = valuesRecoilBackup[count][27];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[4]  = valuesRecoilBackup[count][28];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[5]  = valuesRecoilBackup[count][29];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[0]         = valuesRecoilBackup[count][30];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[1]         = valuesRecoilBackup[count][31];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[2]         = valuesRecoilBackup[count][32];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[3]         = valuesRecoilBackup[count][33];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[4]         = valuesRecoilBackup[count][34];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[5]         = valuesRecoilBackup[count][35];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[0]         = valuesRecoilBackup[count][36];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[1]         = valuesRecoilBackup[count][37];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[2]         = valuesRecoilBackup[count][38];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[3]         = valuesRecoilBackup[count][39];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[4]         = valuesRecoilBackup[count][40];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[5]         = valuesRecoilBackup[count][41];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[0] = valuesRecoilBackup[count][42];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[1] = valuesRecoilBackup[count][43];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[2] = valuesRecoilBackup[count][44];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[3] = valuesRecoilBackup[count][45];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[4] = valuesRecoilBackup[count][46];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[5] = valuesRecoilBackup[count][47];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[0] = valuesRecoilBackup[count][48];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[1] = valuesRecoilBackup[count][49];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[2] = valuesRecoilBackup[count][50];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[3] = valuesRecoilBackup[count][51];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[4] = valuesRecoilBackup[count][52];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[5] = valuesRecoilBackup[count][53];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[0]  = valuesRecoilBackup[count][54];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[1]  = valuesRecoilBackup[count][55];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[2]  = valuesRecoilBackup[count][56];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[3]  = valuesRecoilBackup[count][57];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[4]  = valuesRecoilBackup[count][58];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[5]  = valuesRecoilBackup[count][59];

                        /*for (int i = 0; i < 6; i++)
                        {
                            // CORRECT
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDir[i]         = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickDev[i]         = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMin[i] = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickStrengthMax[i] = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->hipAngularGunKickPitchScale[i]  = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDir[i]         = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickDev[i]         = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMin[i] = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickStrengthMax[i] = valuesRecoilBackup[count][i];
                            weapons->weaponCompleteDefArr[count]->weapDef->adsAngularGunKickPitchScale[i]  = valuesRecoilBackup[count][i];
                        }*/
                    }
                }
            }
        }

        if(KEY_SPREAD_MANAGER)
        {
            noSpread = !noSpread;

            if(noSpread)
            {
                //for (auto w : weapons->weaponCompleteDefArr)
                for (int count = 0; count < 962; count++)
                {
                    //if (w->weapDef)
                    if (weapons->weaponCompleteDefArr[count]->weapDef)
                    {
                        // BACKUP
                        valuesSpreadBackup[count][0]  = weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadDuckedDecay;
                        valuesSpreadBackup[count][1]  = weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadProneDecay;
                        valuesSpreadBackup[count][2]  = weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadSprintDecay;
                        valuesSpreadBackup[count][3]  = weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadInAirDecay;
                        valuesSpreadBackup[count][4]  = weapons->weaponCompleteDefArr[count]->weapDef->fHipReticleSidePos;
                        valuesSpreadBackup[count][5]  = weapons->weaponCompleteDefArr[count]->weapDef->fAdsIdleAmount;
                        valuesSpreadBackup[count][6]  = weapons->weaponCompleteDefArr[count]->weapDef->fHipIdleAmount;
                        valuesSpreadBackup[count][7]  = weapons->weaponCompleteDefArr[count]->weapDef->adsIdleSpeed;
                        valuesSpreadBackup[count][8]  = weapons->weaponCompleteDefArr[count]->weapDef->hipIdleSpeed;
                        valuesSpreadBackup[count][9]  = weapons->weaponCompleteDefArr[count]->weapDef->fIdleCrouchFactor;
                        valuesSpreadBackup[count][10] = weapons->weaponCompleteDefArr[count]->weapDef->fIdleProneFactor;
                        valuesSpreadBackup[count][11] = weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxPitch;
                        valuesSpreadBackup[count][12] = weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw; // start no spread during moving and shooting
                        valuesSpreadBackup[count][13] = weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch;
                        valuesSpreadBackup[count][14] = weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw;
                        valuesSpreadBackup[count][15] = weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime;
                        valuesSpreadBackup[count][16] = weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime;
                        valuesSpreadBackup[count][17] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin;
                        valuesSpreadBackup[count][18] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax;
                        valuesSpreadBackup[count][19] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate;
                        valuesSpreadBackup[count][20] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd;
                        valuesSpreadBackup[count][21] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd; // end no spread
                        // WRITE
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadDuckedDecay = 0.0F; // start no spread
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadProneDecay  = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadSprintDecay  = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadInAirDecay   = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipReticleSidePos    = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fAdsIdleAmount        = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipIdleAmount        = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleSpeed          = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->hipIdleSpeed          = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fIdleCrouchFactor     = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fIdleProneFactor      = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxPitch          = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw            = 0.0F; // start no spread during moving and shooting
                        weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch         = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw           = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime  = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime       = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin        = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax        = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate  = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd    = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd    = 0.0F; // end no spread
                    }
                }
            }
            else
            {
                //for (auto w : weapons->weaponCompleteDefArr)
                for (int count = 0; count < 962; count++)
                {
                    //if (w->weapDef)
                    if (weapons->weaponCompleteDefArr[count]->weapDef)
                    {
                        // WRITE
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadDuckedDecay = valuesSpreadBackup[count][0];  // start no spread
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadProneDecay  = valuesSpreadBackup[count][1];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadSprintDecay  = valuesSpreadBackup[count][2];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadInAirDecay   = valuesSpreadBackup[count][3];
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipReticleSidePos    = valuesSpreadBackup[count][4];
                        weapons->weaponCompleteDefArr[count]->weapDef->fAdsIdleAmount        = valuesSpreadBackup[count][5];
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipIdleAmount        = valuesSpreadBackup[count][6];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleSpeed          = valuesSpreadBackup[count][7];
                        weapons->weaponCompleteDefArr[count]->weapDef->hipIdleSpeed          = valuesSpreadBackup[count][8];
                        weapons->weaponCompleteDefArr[count]->weapDef->fIdleCrouchFactor     = valuesSpreadBackup[count][9];
                        weapons->weaponCompleteDefArr[count]->weapDef->fIdleProneFactor      = valuesSpreadBackup[count][10];
                        weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxPitch          = valuesSpreadBackup[count][11];
                        weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw            = valuesSpreadBackup[count][12];           // start no spread during moving and shooting
                        weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch         = valuesSpreadBackup[count][13];
                        weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw           = valuesSpreadBackup[count][14];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime  = valuesSpreadBackup[count][15];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime       = valuesSpreadBackup[count][16];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin        = valuesSpreadBackup[count][17];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax        = valuesSpreadBackup[count][18];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate  = valuesSpreadBackup[count][19];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd    = valuesSpreadBackup[count][20];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd    = valuesSpreadBackup[count][21];   // end no spread
                    }
                }
            }
        }

        // DON'T USE THAT SHIT
        if (KEY_TRIGGERBOT_MANAGER)
        {
            triggerBot = !triggerBot;
        }
            
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

        if (triggerBot)
        {
            if (numOfPlayers > 4)
            {
                if (playerState)
                {
                    if (playerState->Health >= 0 && playerState->Health <= 300)
                    {
                        if(playerState->crosshair == 16)
                        {
                            *(int*)(moduleBase + offsets->GetOffset(Offsets::SHOTSFIREASSAULT)) = 1;
                            *(int*)(moduleBase + offsets->GetOffset(Offsets::SHOTSFIRE)) = 1;
                        }
                        else
                        {
                            *(int*)(moduleBase + offsets->GetOffset(Offsets::SHOTSFIREASSAULT)) = 0;
                            *(int*)(moduleBase + offsets->GetOffset(Offsets::SHOTSFIRE)) = 0;
                        }
                    }
                }
            }
        }

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

    //PatchBytes((BYTE*)moduleBase + 0x2983391, (BYTE*)"\x0F\x28\xC3\xF3\x0F\x58\xC7\xF3\x0F\x10\xC8\x66\x0F\x3A\x0A\xD1\x01\xF3\x0F\x5C\xDA\xF3\x41\x0F\x59\xDB\xF3\x0F\x11\x9C\xBE\xDC\x01\x00\x00", 35);
    //PatchBytes((BYTE*)moduleBase + 0x2A57977, (BYTE*)"\xF3\x0F\x10\xC8\x66\x0F\x3A\x0A\xF1\x01\xF3\x0F\x5C\xFE\x0F\x28\x74\x24\x30\xF3\x0F\x59\x3D\xBA\xD0\xAB\x04\xF3\x0F\x11\xBC\x9F\xDC\x01\x00\x00", 36);
    //PatchBytes((BYTE*)moduleBase + offsets->GetOffset(Offsets::WRITE_VIEW_ANGLES), (BYTE*)"\x0F\x28\xC6\xF3\x41\x0F\x58\xC0\xF3\x0F\x10\xC8\x66\x0F\x3A\x0A\xD1\x01\xF3\x0F\x5C\xF2\xF3\x41\x0F\x59\xF2\xF3\x0F\x11\xB4\xBB\xDC\x01\x00\x00", 36);
   
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


