#pragma once

//Offsets
#define ADDRESS_CG_T 0x180E4290                          // [[48 63 C1 48 69 C8 60 03 00 00 48 8D ? ? ? ? ? 48 03 C8 E9] - 0xA] lea - 0x18
#define ADDRESS_GAMEMODE 0x17CF1F78                      // 74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?
#define ADDRESS_CHECK_UPDATE 0x43CDEFC                   // 74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?

//Options
#define KEY_MODULE_EJECT GetAsyncKeyState(VK_END) & 1
#define KEY_UAV_MANAGER GetAsyncKeyState(VK_F4) & 1
#define KEY_RECOIL_MANAGER GetAsyncKeyState(VK_F5) & 1