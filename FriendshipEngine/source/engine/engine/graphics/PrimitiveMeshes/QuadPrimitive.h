#pragma once
#include <vector>
#include "engine\graphics\Vertex.h"

class Mesh;

struct QuadData
{
	QuadData() = default;
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;
};

class QuadPrimitive
{
public:
	QuadPrimitive();
	~QuadPrimitive();

	void Construct();

	const QuadData& GetCubeData();

	Mesh* GetMesh();
private:
	Mesh* myMesh;
	QuadData myQuadData;
};