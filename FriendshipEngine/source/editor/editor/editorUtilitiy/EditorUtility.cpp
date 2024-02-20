#include "EditorUtility.h"

Vector2f EditorUtility::ArrayToVec2f(float aVec[2])
{
	return Vector2f(aVec[0], aVec[1]);
}

Vector3f EditorUtility::ArrayToVec3f(float aVec[3])
{
	return Vector3f(aVec[0], aVec[1], aVec[2]);
}
