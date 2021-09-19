#include "Offsets.h"

uintptr_t Offsets::GetOffset(Offset offset)
{
	switch (offset)
	{
	case Offsets::Uav1:
		return m_offsets[Offset::Uav1];
		break;
	case Offsets::Uav2:
		return m_offsets[Offset::Uav2];
		break;
	case Offsets::ArrayNames:
		return m_offsets[Offset::ArrayNames];
		break;
	case Offsets::IsFiring:
		return m_offsets[Offset::IsFiring];
		break;
	default:
		break;
	}
}