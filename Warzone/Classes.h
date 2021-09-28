#pragma once
#include "pch.h"

struct Vector3
{
	float x, y, z;
};

class cg_t
{
public:
	char pad_0000[392]; //0x0000
	Vector3 vecOrig; //0x0188
	char pad_0194[416]; //0x0194
	Vector3 viewAngle; //0x0334
	char pad_0340[112]; //0x0340
	int32_t N000000C4; //0x03B0
	int32_t Health; //0x03B4
	char pad_03B8[160]; //0x03B8
	int32_t N000000D8; //0x0458
	int32_t Uav; //0x045C
	char pad_0460[3128]; //0x0460
}; //Size: 0x1098
static_assert(sizeof(cg_t) == 0x1098);