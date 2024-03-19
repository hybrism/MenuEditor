#pragma once
#include <bitset>

namespace MENU
{
	using ID = unsigned int;
	const ID MAX_ID = 512;
	const ID INVALID_ID = UINT_MAX;

	class IDManager
	{
	public:
		IDManager();

		ID GetFreeID();
		ID UseID(ID aID);
		void FreeID(ID aID);
		void FreeAllIDs();

	private:
		std::bitset<MAX_ID> myIDs;
	};
}