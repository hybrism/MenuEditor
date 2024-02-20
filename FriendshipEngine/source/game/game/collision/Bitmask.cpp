#include "pch.h"
#include "Bitmask.h"

Bitmask::Bitmask() : myBits(0)
{
}

void Bitmask::SetMask(Bitmask& anOther)
{
	myBits = anOther.GetMask();
}

uint32_t Bitmask::GetMask() const
{
	return myBits;
}

bool Bitmask::GetBit(int aBitPos) const
{
	return (myBits & (1 << aBitPos)) != 0;
}

void Bitmask::SetBit(int aBitPos, bool anOn)
{
	if (anOn)
		SetBit(aBitPos);
	else
		ClearBit(aBitPos);
}

void Bitmask::SetBit(int aBitPos)
{
	myBits = myBits | 1 << aBitPos;
}

void Bitmask::ClearBit(int aBitPos)
{
	myBits = myBits & ~(1 << aBitPos);
}

void Bitmask::ClearAll()
{
	myBits = 0;
}
