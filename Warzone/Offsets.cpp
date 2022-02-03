#include "Offsets.h"

uintptr_t Offsets::GetOffset(Offset offset)
{
	switch (offset)
	{
	case Offsets::PLAYERSTATE_S:
		return m_offsets[Offset::PLAYERSTATE_S];
		break;
	case Offsets::NUM_OF_PLAYERS:
		return m_offsets[Offset::NUM_OF_PLAYERS];
		break;
	case Offsets::CHECKUPDATE:
		return m_offsets[Offset::CHECKUPDATE];
		break;
	case Offsets::SHOTSFIREASSAULT:
		return m_offsets[Offset::SHOTSFIREASSAULT];
		break;
	case Offsets::SHOTSFIRE:
		return m_offsets[Offset::SHOTSFIRE];
		break;
	case Offsets::WEAPON_COMPLETE_DEF:
		return m_offsets[Offset::WEAPON_COMPLETE_DEF];
		break;
	default:
		break;
	}
}

uintptr_t Offsets::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	if (ptr != 0)
	{
		uintptr_t addr = ptr;
		for (unsigned int i = 0; i < offsets.size(); ++i)
		{
			addr = *(uintptr_t*)addr;
			addr += offsets[i];
		}
		return addr;
	}
	else
		return 0;
}