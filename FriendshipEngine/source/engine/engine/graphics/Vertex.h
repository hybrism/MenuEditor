#pragma once
#include "../math/Vector.h"

struct Vertex
{
	Vector4<float> position;
	Vector4<float> color;
	Vector2<float> uv;
	Vector3<float> normal;
	Vector3<float> binormal;
	Vector3<float> tangent;
};

struct AnimatedVertex : public Vertex
{
	Vector4<unsigned int> bones;
	Vector4<float> weights;
};

//bool operator==(const AnimatedVertex& other) const
//{
//	return memcmp(this, &other, sizeof(AnimatedVertex)) == 0;
//}

struct SimpleVertex
{
	Vector4<float> position;
	Vector4<float> color;
	Vector2<float> uv;
};
