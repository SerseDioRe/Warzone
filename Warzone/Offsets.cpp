#include "Offsets.h"

uintptr_t Offsets::GetOffset(Offset offset)
{
	switch (offset)
	{
	case Offsets::CG_T:
		return m_offsets[Offset::CG_T];
		break;
	case Offsets::GAMEMODE:
		return m_offsets[Offset::GAMEMODE];
		break;
	case Offsets::CHECKUPDATE:
		return m_offsets[Offset::CHECKUPDATE];
		break;
	case Offsets::RECOIL:
		return m_offsets[Offset::RECOIL];
		break;
	case Offsets::SHOTSFIREASSAULT:
		return m_offsets[Offset::SHOTSFIREASSAULT];
		break;
	case Offsets::SHOTSFIRE:
		return m_offsets[Offset::SHOTSFIRE];
		break;
	case Offsets::CROSSHAIR:
		return m_offsets[Offset::CROSSHAIR];
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