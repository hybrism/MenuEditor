#pragma once

typedef unsigned long long eid_t;
constexpr eid_t INVALID_ENTITY = 0;
constexpr eid_t MAX_ENTITIES = 0b1000000000000000;  // (32768) should be adjusted if needed
using Entity = eid_t;

#ifndef _RELEASE
// Vi beh�ver inte det h�r egentligen, men vi har det TILLS VIDARE p� grund av debugging
struct EntityData
{
	eid_t id;
	int unityID;
};
#endif

