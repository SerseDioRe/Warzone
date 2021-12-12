#pragma once

// Offsets
#define ADDRESS_CG_T 0x1EA5DE30                           // [[48 63 C1 48 69 C8 60 03 00 00 48 8D ? ? ? ? ? 48 03 C8 E9] + 0xA] lea - 0x18
#define ADDRESS_GAMEMODE 0x1E5CCF08                       // [74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?] - 0x17                     2B 05 6E3F9A13        - sub eax,[ADDRESS_GAMEMODE]
#define ADDRESS_CHECK_UPDATE 0x4510B39                    // 74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?
#define ADDRESS_RECOIL 0x19B28                            // [4C 8D 9E ? ? ? ? B3 ?] [[41 8B 52 0C 4D 8D 4A 04 4D 8D 42 08 4C 89 95 ? ? ? ? 8B C2 4C 89 8D] - 0x9] [0F 85 ? ? ? ? 0F 2E 80 ? ? ? ? 0F 85 ? ? ? ? 4C 8D 96 ? ? ? ?]

// Testing
#define ADDRESS_SHOTSFIREASSAULT 0x1E5C9128               // [42 80 BC 3E ? ? ? ? ? 75 ? 42 80 BC 3E ? ? ? ? ? 74 ? 48 83 0A ?] [80 BC 0B ? ? ? ? ? 75 ? 80 BC 0B ? ? ? ? ? 74 ? 49 83 4D ? ? 41 8B 06 0F BA E8 ?]
#define ADDRESS_SHOTSFIRE 0x1E5C9129                      // [42 80 BC 3E ? ? ? ? ? 75 ? 42 80 BC 3E ? ? ? ? ? 74 ? 48 83 0A ?] [80 BC 0B ? ? ? ? ? 75 ? 80 BC 0B ? ? ? ? ? 74 ? 49 83 4D ? ? 41 8B 06 0F BA E8 ?]
#define ADDRESS_CROSSHAIR 0xB36316C                       // [48 8D 05 ? ? ? ? 48 89 6C 24 ? 4C 03 F0] lea + 0x20

// Options
#define KEY_MODULE_EJECT GetAsyncKeyState(VK_END) & 1
#define KEY_UAV_MANAGER GetAsyncKeyState(VK_F4) & 1
#define KEY_RECOIL_MANAGER GetAsyncKeyState(VK_F5) & 1

// Testing
#define KEY_TRIGGERBOT_MANAGER GetAsyncKeyState(VK_F6) & 1