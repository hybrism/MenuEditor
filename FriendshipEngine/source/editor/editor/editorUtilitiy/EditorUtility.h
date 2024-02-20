#pragma once
#include <string>
#include <engine/math/Vector.h>

#define ToVec2f(arg) EditorUtility::ArrayToVec2f(arg)
#define ToVec3f(arg) EditorUtility::ArrayToVec3f(arg)

class EditorUtility
{
public:
	static Vector2f ArrayToVec2f(float aVec[2]);
	static Vector3f ArrayToVec3f(float aVec[3]);
};