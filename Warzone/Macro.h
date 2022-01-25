﻿#pragma once

// Offsets
#define ADDRESS_PLAYERSTATE_S 0x1F1E1E60                  // [[48 63 C1 48 69 C8 60 03 00 00 48 8D ? ? ? ? ? 48 03 C8 E9] + 0xA] lea - 0x18
#define ADDRESS_NUM_OF_PLAYERS 0x1ED50728                 // [74 1D 48 8B ? ? ? ? ? 48 2B ? ? ? ? ? 2B 05 ? ? ? ?] - 0x17                     2B 05 6E3F9A13        - sub eax,[ADDRESS_GAMEMODE]
#define ADDRESS_CHECK_UPDATE 0x4A3EF2F                    // [74 1D 48 8B ? ? ? ? ? 48 2B ? ? ? ? ? 2B 05 ? ? ? ?]              
#define ADDRESS_WEAPON_COMPLETE_DEF 0x1BBD0E20            // [48 89 74 24 ? 41 56 48 83 EC ? 4C 8B F2 48 8B F1 E8 ? ? ? ?]

// Testing
#define ADDRESS_SHOTSFIREASSAULT 0x1ED4C928               // [42 80 BC 3E ? ? ? ? ? 75 ? 42 80 BC 3E ? ? ? ? ? 74 ? 48 83 0A ?] [80 BC 0B ? ? ? ? ? 75 ? 80 BC 0B ? ? ? ? ? 74 ? 49 83 4D ? ? 41 8B 06 0F BA E8 ?]
#define ADDRESS_SHOTSFIRE 0x1ED4C929                      // [42 80 BC 3E ? ? ? ? ? 75 ? 42 80 BC 3E ? ? ? ? ? 74 ? 48 83 0A ?] [80 BC 0B ? ? ? ? ? 75 ? 80 BC 0B ? ? ? ? ? 74 ? 49 83 4D ? ? 41 8B 06 0F BA E8 ?]
#define ADDRESS_WRITE_VIEW_ANGLES 0x36CCB41               // [F3 0F 11 B4 ?? DC 01 ?? ?? 48 FF ?? 48 83 ?? 03]

// Options
#define KEY_MODULE_EJECT GetAsyncKeyState(VK_END) & 1
#define KEY_UAV_MANAGER GetAsyncKeyState(VK_F4) & 1
#define KEY_RECOIL_MANAGER GetAsyncKeyState(VK_F5) & 1
#define KEY_SPREAD_MANAGER GetAsyncKeyState(VK_F6) & 1

// Testing
#define KEY_TRIGGERBOT_MANAGER GetAsyncKeyState(VK_F7) & 1