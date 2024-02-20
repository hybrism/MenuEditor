#pragma once
#include <vector>
#include "engine\graphics\Vertex.h"

class Mesh;

struct SphereData
{
	SphereData() = default;
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;
};

class SpherePrimitive
{
public:
	SpherePrimitive();
	~SpherePrimitive() {};

	void ConstructSphere();

	 const SphereData& GetSphereData();

	 Mesh* GetMesh();
private:
	Mesh* mySphereMesh;
	SphereData mySphereData;
};