#pragma once

//Offsets
#define ADDRESS_CG_T 0x1745E5E0

//Options
#define KEY_MODULE_EJECT  GetAsyncKeyState(VK_END) & 1
#define KEY_UAV_MANAGER GetAsyncKeyState(VK_F4) & 1

/* IT WORKS FOR 2 VERSIONS
5 asm op but is head function, real offset is + 0xA
\x48\x63\xC1\x48\x69\xC8\x60\x03\x00\x00\x48\x8D\x00\x00\x00\x00\x00\x48\x03\xC8\xE9
xxxxxxxxxxxx ? ? ? ? ? xxxx
48 63 C1 48 69 C8 60 03 00 00 48 8D ? ? ? ? ? 48 03 C8 E9

.text:00000000048E0E60                         sub_48E0E60     proc near
.text:00000000048E0E60 48 63 C1                                movsxd  rax, ecx
.text:00000000048E0E63 48 69 C8 60 03 00 00                    imul    rcx, rax, 360h
.text:00000000048E0E6A 48 8D 05 87 D7 B7 12                    lea     rax, unk_1745E5F8
.text:00000000048E0E71 48 03 C8                                add     rcx, rax
.text:00000000048E0E74 E9 A7 01 9A FD                          jmp     sub_2281020
.text:00000000048E0E74                         sub_48E0E60     endp
*/

/* IT WORKS FOR 2 VERSIONS
1 asm op but is call function, real offset is result + 0xA
E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 63 CE
*/

/* IT WORKS ONLY FOR 1 VERSION
3 asm op
\x48\x8D\x00\x00\x00\x00\x00\x48\x03\xC8\xE9
xx?????xxxx
48 8D ? ? ? ? ? 48 03 C8 E9

.text:0000000004B1CF6A 48 8D 05 87 66 C4 12                    lea     rax, unk_177635F8
.text:0000000004B1CF71 48 03 C8                                add     rcx, rax
.text:0000000004B1CF74 E9 97 89 88 FD                          jmp     sub_23A5910
*/

/* IT WORKS ONLY FOR 1 VERSION
5 asm op
\x48\x8D\x00\x00\x00\x00\x00\x48\x03\xC8\xE9\x00\x00\x00\x00\x33\xC0\xE9
xx?????xxxx????xxx
48 8D ? ? ? ? ? 48 03 C8 E9 ? ? ? ? 33 C0 E9
*/

/* IT WORKS ONLY FOR 1 VERSION
7 asm op
\x48\x63\xC1\x48\x69\xC8\x60\x03\x00\x00\x48\x8D\x00\x00\x00\x00\x00\x48\x03\xC8\xE9\x00\x00\x00\x00\x33\xC0\xE9
xxxxxxxxxxxx?????xxxx????xxx
48 63 C1 48 69 C8 60 03 00 00 48 8D ? ? ? ? ? 48 03 C8 E9 ? ? ? ? 33 C0 E9
*/