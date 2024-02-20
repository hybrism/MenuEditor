#pragma once

typedef unsigned long long eid_t;
const eid_t INVALID_ENTITY = 0;
const eid_t MAX_ENTITIES = 0b1000000000000000;  // (32768) should be adjusted if needed
using Entity = eid_t;

#ifdef _DEBUG
// Vi behöver inte det här egentligen, men vi har det TILLS VIDARE på grund av debugging
struct EntityData
{
	eid_t id;
	int unityID;
};
#endif

