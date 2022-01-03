#pragma once
#include "pch.h"
#include "Macro.h"

class Offsets
{
public:
	enum Offset
	{
		PLAYERSTATE_S,
		NUM_OF_PLAYERS,
		CHECKUPDATE,
		RECOIL,
		SHOTSFIREASSAULT,
		SHOTSFIRE,
		CROSSHAIR,
	};
private:
	uintptr_t m_offsets[7] = { ADDRESS_PLAYERSTATE_S, ADDRESS_NUM_OF_PLAYERS, ADDRESS_CHECK_UPDATE, ADDRESS_RECOIL, ADDRESS_SHOTSFIREASSAULT,
	ADDRESS_SHOTSFIRE, ADDRESS_CROSSHAIR };
public:
	uintptr_t GetOffset(Offset offset);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
};