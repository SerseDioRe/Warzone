#pragma once

//Offsets
#define ADDRESS_CG_T 0x179141A0

//Options
#define KEY_MODULE_EJECT  GetAsyncKeyState(VK_END) & 1
#define KEY_UAV_MANAGER GetAsyncKeyState(VK_F4) & 1