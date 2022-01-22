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
		SHOTSFIREASSAULT,
		SHOTSFIRE,
		WEAPON_COMPLETE_DEF,
	};
private:
	uintptr_t m_offsets[7] = { ADDRESS_PLAYERSTATE_S, ADDRESS_NUM_OF_PLAYERS, ADDRESS_CHECK_UPDATE, ADDRESS_SHOTSFIREASSAULT,
	ADDRESS_SHOTSFIRE, ADDRESS_WEAPON_COMPLETE_DEF };
public:
	uintptr_t GetOffset(Offset offset);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
};