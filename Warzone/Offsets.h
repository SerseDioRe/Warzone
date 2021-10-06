#pragma once
#include "pch.h"
#include "Macro.h"

class Offsets
{
public:
	enum Offset
	{
		CG_T,
		GAMEMODE,
	};
private:
	uintptr_t m_offsets[2] = { ADDRESS_CG_T, ADDRESS_GAMEMODE };
public:
	uintptr_t GetOffset(Offset offset);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
};