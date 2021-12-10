#pragma once

//Offsets
#define ADDRESS_CG_T 0x1E679B30                           // [[48 63 C1 48 69 C8 60 03 00 00 48 8D ? ? ? ? ? 48 03 C8 E9] + 0xA] lea - 0x18
#define ADDRESS_GAMEMODE 0x1E1E8B08                       // [74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?] - 0x17                     2B 05 6E3F9A13        - sub eax,[ADDRESS_GAMEMODE]
#define ADDRESS_CHECK_UPDATE 0x4114486                    // 74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?
#define ADDRESS_RECOIL 0x3C220                         // 41 8B 52 0C 4D 8D 4A 04 4D 8D 42 08 4C 89 95 ? ? ? ? 8B C2 4C 89 8D - 0x9

//Options
#define KEY_MODULE_EJECT GetAsyncKeyState(VK_END) & 1
#define KEY_UAV_MANAGER GetAsyncKeyState(VK_F4) & 1
#define KEY_RECOIL_MANAGER GetAsyncKeyState(VK_F5) & 1