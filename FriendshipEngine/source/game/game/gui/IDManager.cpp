#include "pch.h"

#include "IDManager.h"
#include <cassert>

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
	assert(!myIDs[aID] && "ID already in use!");

	myIDs[aID] = true;

	return aID;
}

void MENU::IDManager::FreeID(ID aID)
{
	assert(aID <= MAX_ID && "ID is out of range!");
	myIDs[aID] = false;
}

void MENU::IDManager::FreeAllIDs()
{
	myIDs.reset();
}

MENU::IDManager::IDManager()
{
	myIDs.reset();
}
