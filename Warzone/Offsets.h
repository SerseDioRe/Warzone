#pragma once
#include "pch.h"
#include "Macro.h"

class Offsets
{
public:
	enum Offset
	{
		Uav1,
		Uav2,
		ArrayNames,
		IsFiring,
		Health,
		CheckUav,
	};
private:
	uintptr_t m_offsets[6] = { OFFSET_UAV1, OFFSET_UAV2, OFFSET_ARRAY_NAMES, OFFSET_ISFIRING, OFFSET_HEALTH, OFFSET_CHECKUAV };
public:
	uintptr_t GetOffset(Offset offset);
};