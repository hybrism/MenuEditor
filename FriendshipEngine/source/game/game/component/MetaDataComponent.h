#pragma once
#ifdef _DEBUG
#include <ecs/component/Component.h>
#include <string>

struct MetaDataComponent : public Component<MetaDataComponent>
{
	int unityID = 0;
	char name[104] = "";
};
#endif