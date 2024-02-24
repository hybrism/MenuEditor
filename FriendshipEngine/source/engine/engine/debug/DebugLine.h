#pragma once
#include <vector>
#include "../math/Vector.h"
#include "../math/Transform.h"
#include "../graphics/Vertex.h"
#include "../graphics/model/Mesh.h"

class PixelShader;

class DebugLine
{
public:
	DebugLine();
	DebugLine(const Vector3f& aStart, const Vector3f& aEnd, const Vector4f& aColor = { 1.f, 1.f, 1.f, 1.f });
	
	void SetStartAndEnd(const Vector3f& aStart, const Vector3f& aEnd);
	void SetStartPosition(const Vector3f& aStartPoint);
	void SetEndPosition(const Vector3f& aEndPoint);

	void DrawLine();

	void SetPosition(const Vector3f& aPosition);
	void SetColor(const Vector4f& aColor);

	Vector3f GetStartPosition() { return myStartPosition; }
	Vector3f GetEndPosition() { return myEndPosition; }

private:
	void UpdateVertices();

	Transform myTransform = {};
	std::vector<Vertex> myVertices = {};

	Vector3f myStartPosition;
	Vector3f myEndPosition;
	Vector4f myColor;

	Mesh myMesh;

};