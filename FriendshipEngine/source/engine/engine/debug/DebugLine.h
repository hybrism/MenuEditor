#pragma once
#include "../math/Vector3.h"
#include "../math/Transform.h"
#include <vector>
#include "../graphics/Vertex.h"

class Mesh;

class DebugLine
{
public:
	DebugLine(const Vector3<float>& aStartPosition, const Vector3<float>& aEndPosition);
	DebugLine(const Vector3<float>& aStartPosition, const Vector3<float>& aEndPosition, const Vector4<float>& aColor);
	~DebugLine();

	void Init();
	void SetStartPosition(const Vector3<float>& aStartPoint);
	void SetEndPosition(const Vector3<float>& aEndPoint);

	void DrawLine();
	//void DrawFromTo(Vector3<float> aStartPoint, Vector3<float> aEndPoint);
	void SetPosition(const Vector3<float>& aPosition);
	Vector3<float> GetPosition() { return myTransform.GetPosition(); };

	void SetColor(const Vector4<float>& aColor);
	Vector3f GetStartPosition() { return myStartPosition; }
	Vector3f GetEndPosition() { return myEndPosition; }
private:
	void UpdateVertices();
	Transform myTransform = {};
	std::vector<int> myIndices = {};
	std::vector<Vertex> myVertices = {};

	Vector3<float> myStartPosition;
	Vector3<float> myEndPosition;
	Vector4<float> myColor;
	Mesh* myMesh = nullptr;
};