#pragma once
#include <array>

namespace MENU
{
	using ID = unsigned int;
	const ID MAX_ID = 512;
	const ID INVALID_ID = UINT_MAX;

	class IDManager
	{
	public:
		IDManager(IDManager& aOther) = delete;
		void operator=(const IDManager& aOther) = delete;

		static IDManager* GetInstance();

		ID GetFreeID();
		ID UseID(ID aID);
		void FreeID(ID aID);
		void FreeAllIDs();

		std::array<bool, MAX_ID> GetIDs();

	private:
		IDManager();

		std::array<bool, MAX_ID> myIDs;

		static IDManager* myInstance;
	};
}