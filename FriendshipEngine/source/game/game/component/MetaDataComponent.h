#pragma once
#ifndef _RELEASE
#include <ecs/component/Component.h>
#include <string>

struct MetaDataComponent : public Component<MetaDataComponent>
{
	int unityID = 0;
	char name[104] = "";
	unsigned int size = 0;
};
#endif