#pragma once
#include <cstdint>




class Bitmask
{
public:
	Bitmask();

	void SetMask(Bitmask& anOther);

	uint32_t GetMask() const;

	bool GetBit(int aBitPos) const;

	void SetBit(int aBitPos, bool anOn);

	void SetBit(int aBitPos);

	void ClearBit(int aBitPos);

	void ClearAll();

private:

	uint32_t myBits; // 1.

};
