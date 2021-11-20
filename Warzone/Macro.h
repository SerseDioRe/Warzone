#pragma once

//Offsets
#define ADDRESS_CG_T 0x1807C390                          // [[48 63 C1 48 69 C8 60 03 00 00 48 8D ? ? ? ? ? 48 03 C8 E9] + 0xA] lea - 0x18
#define ADDRESS_GAMEMODE 0x17C8A178                      // [74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?] - 0x17                     2B 05 6E3F9A13        - sub eax,[ADDRESS_GAMEMODE]
#define ADDRESS_CHECK_UPDATE 0x42E621B                   // 74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?

//Options
#define KEY_MODULE_EJECT GetAsyncKeyState(VK_END) & 1
#define KEY_UAV_MANAGER GetAsyncKeyState(VK_F4) & 1
#define KEY_RECOIL_MANAGER GetAsyncKeyState(VK_F5) & 1