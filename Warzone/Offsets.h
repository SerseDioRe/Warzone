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
		CHECKUPDATE,
		RECOIL,
		SHOTSFIREASSAULT,
		SHOTSFIRE,
		CROSSHAIR,
	};
private:
	uintptr_t m_offsets[7] = { ADDRESS_CG_T, ADDRESS_GAMEMODE, ADDRESS_CHECK_UPDATE, ADDRESS_RECOIL, ADDRESS_SHOTSFIREASSAULT,
	ADDRESS_SHOTSFIRE, ADDRESS_CROSSHAIR };
public:
	uintptr_t GetOffset(Offset offset);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
};