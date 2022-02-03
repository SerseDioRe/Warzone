// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"
#include "Classes.h"
#include "Offsets.h"
#include "Memory.h"
#include <psapi.h>
#include <process.h>

Offsets* offsets           = nullptr;
playerState_s* playerState = nullptr;
uintptr_t isShootingAddress = 0;

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

    if (!Updated())
        return NULL;

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
                for(int count = 0; count < 962; count++)
                {
                    if(weapons->weaponCompleteDefArr[count]->weapDef)
                    {
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
                    if (weapons->weaponCompleteDefArr[count]->weapDef)
                    {
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
                    }
                }
            }
        }

        if(KEY_SPREAD_MANAGER)
        {
            noSpread = !noSpread;

            if(noSpread)
            {
                for (int count = 0; count < 962; count++)
                {
                    if (weapons->weaponCompleteDefArr[count]->weapDef)
                    {
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
                        valuesSpreadBackup[count][12] = weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw; 
                        valuesSpreadBackup[count][13] = weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch;
                        valuesSpreadBackup[count][14] = weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw;
                        valuesSpreadBackup[count][15] = weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime;
                        valuesSpreadBackup[count][16] = weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime;
                        valuesSpreadBackup[count][17] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin;
                        valuesSpreadBackup[count][18] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax;
                        valuesSpreadBackup[count][19] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate;
                        valuesSpreadBackup[count][20] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd;
                        valuesSpreadBackup[count][21] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd; 
                        // WRITE
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadDuckedDecay = 0.0F; 
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
                        weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw            = 0.0F; 
                        weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch         = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw           = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime  = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime       = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin        = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax        = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate  = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd    = 0.0F;
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd    = 0.0F; 
                    }
                }
            }
            else
            {
                for (int count = 0; count < 962; count++)
                {
                    if (weapons->weaponCompleteDefArr[count]->weapDef)
                    {
                        weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadDuckedDecay = valuesSpreadBackup[count][0];  
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
                        weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw            = valuesSpreadBackup[count][12];      
                        weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch         = valuesSpreadBackup[count][13];
                        weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw           = valuesSpreadBackup[count][14];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime  = valuesSpreadBackup[count][15];
                        weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime       = valuesSpreadBackup[count][16];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin        = valuesSpreadBackup[count][17];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax        = valuesSpreadBackup[count][18];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate  = valuesSpreadBackup[count][19];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd    = valuesSpreadBackup[count][20];
                        weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd    = valuesSpreadBackup[count][21];   
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
                        playerState->radarShowEnemyDirection = true;
                        playerState->radarMode = RadarMode::RADAR_MODE_CONSTANT;
                    }
                }
            }
        }

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


