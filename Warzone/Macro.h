#pragma once

//Offsets
#define OFFSET_UAV1 0x14F7818C
#define OFFSET_UAV2 0x14F78184
#define OFFSET_ARRAY_NAMES 0x1B4B2097  // not updated
#define OFFSET_ISFIRING 0x175F7348     // not updated

//Options
#define KEY_MODULE_EJECT  GetAsyncKeyState(VK_END) & 1
#define PRINT_WELCOME std::cout << "Welcome into ModernWarfare.exe\n";
#define KEY_PRINT_ENTITIES GetAsyncKeyState(VK_F1) & 1
#define KEY_UAV_MANAGER GetAsyncKeyState(VK_F4) & 1
#define KEY_NO_SPREAD GetAsyncKeyState(VK_F5) & 1