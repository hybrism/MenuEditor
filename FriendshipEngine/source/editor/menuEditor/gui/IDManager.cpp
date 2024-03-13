#include "IDManager.h"
#include <cassert>

MENU::IDManager* MENU::IDManager::myInstance = nullptr;

MENU::IDManager* MENU::IDManager::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new IDManager();
	}
	return myInstance;
}

MENU::ID MENU::IDManager::GetFreeID()
{
	for (ID id = 0; id < myIDs.size(); id++)
	{
		if (myIDs[id])
			return UseID(id);
	}

	assert("No free IDs available!");
	return MAX_ID;
}

MENU::ID MENU::IDManager::UseID(ID aID)
{
	assert(aID <= MAX_ID && "ID is out of range!");
	assert(myIDs[aID] && "ID already in use!");

	myIDs[aID] = false;

	return aID;
}

void MENU::IDManager::FreeID(ID aID)
{
	assert(aID <= MAX_ID && "ID is out of range!");

	myIDs[aID] = true;
}

void MENU::IDManager::FreeAllIDs()
{
	myIDs.fill(true);
}

std::array<bool, MENU::MAX_ID> MENU::IDManager::GetIDs()
{
	return myIDs;
}

MENU::IDManager::IDManager()
{
	myIDs.fill(true);
}
