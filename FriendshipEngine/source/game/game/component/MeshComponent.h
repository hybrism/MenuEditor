#pragma once
#include <ecs/component/Component.h>
#include <string>

enum class MeshType
{
	Static,
	Skeletal
};

struct MeshComponent : public Component<MeshComponent>
{
	size_t id = 0;
	MeshType type = MeshType::Static;
	Transform offset{};
	int vertexPaintedIndex = -1;
	bool shouldDisregardDepth = false;
	bool shouldRender = true;
};