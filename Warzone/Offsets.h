#pragma once
#include "pch.h"
#include "Macro.h"

class Offsets
{
public:
	enum Offset
	{
		CG_T,
	};
private:
	uintptr_t m_offsets[3] = { ADDRESS_CG_T };
public:
	uintptr_t GetOffset(Offset offset);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
};