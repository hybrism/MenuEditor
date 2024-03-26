#pragma once

#include "../../math/Vector.h"
#include "../../math/Matrix.h"
#include "../../math/Transform.h"

#include "../Vertex.h"
#include "../Texture.h"

class VertexShader;

struct TextureRect
{
	float startX;
	float startY;
	float endX;
	float endY;
};

struct ClipValue
{
	float left = 0.f;
	float right = 0.f;
	float down = 0.f;
	float upper = 0.f;
};

struct SpriteInstanceData
{
	Vector2<float> position = { 0.f, 0.f };
	Vector2<float> pivot = { 0.5f, 0.5f };
	Vector2<float> size = { 0.f, 0.f };
	Vector2<float> scaleMultiplier = { 1.f, 1.f };
	Vector2<float> UV = { 0.f, 0.f };
	Vector2<float> UVscale = { 1.f, 1.f };
	Vector4<float> color = { 1.f, 1.f, 1.f, 1.f };
	TextureRect textureRect = { 0.f, 0.f, 1.f, 1.f };
	ClipValue clip = { 0.f, 0.f, 0.f, 0.f }; // values from 0.f - 1.f
	float rotation = 0.f;
	bool isHidden = false;
};

struct Sprite3DInstanceData
{
	Transform myTransform = {};
	Vector2<float> UV = { 0.f, 0.f };
	Vector2<float> UVscale = { 1.f, 1.f };
	Vector4<float> color = { 1.f, 1.f, 1.f, 1.f };
	TextureRect textureRect = { 0.f, 0.f, 1.f, 1.f };
	ClipValue clip = { 0.f, 0.f, 0.f, 0.f }; // values from 0.f - 1.f
	float rotation = 0.f;
	bool isHidden = false;
};

struct SpriteSharedData
{
	Texture* texture = nullptr;
	VertexShader* myCustomShader = nullptr;
};