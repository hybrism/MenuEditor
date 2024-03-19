#pragma once
#include <vector>
#include "engine\graphics\Vertex.h"

class Mesh;

struct CubeData
{
	CubeData() = default;
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;
};

class CubePrimitive
{
public:
	CubePrimitive();
	~CubePrimitive() {};

	void ConstructCube();

	const CubeData& GetCubeData();

	Mesh* GetMesh();
private:
	Mesh* myCubeMesh;
	CubeData myCubeData;
};