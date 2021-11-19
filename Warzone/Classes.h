#pragma once
#include "pch.h"

struct Vector3
{
	float x, y, z;
};

class cg_t
{
public:
	char pad_0000[48]; //0x0000
	Vector3 vecOrig; //0x0030
	char pad_003C[416]; //0x003C
	Vector3 viewAngle; //0x01DC
	char pad_01E8[112]; //0x01E8
	int32_t N0000009A; //0x0258
	int32_t Health; //0x025C
	char pad_0260[160]; //0x0260
	int32_t N000000AF; //0x0300
	int32_t Uav; //0x0304
	char pad_0308[1408]; //0x0308
}; //Size: 0x0888
static_assert(sizeof(cg_t) == 0x888);