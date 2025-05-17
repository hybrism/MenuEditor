#include "pch.h"
#include "QuadPrimitive.h"
#include "engine\graphics\model\Mesh.h"

QuadPrimitive::QuadPrimitive()
{
	
}

QuadPrimitive::~QuadPrimitive()
{
}

void QuadPrimitive::Construct()
{
	constexpr float size = 50.0f;

	Vertex vert;

	// front face
	vert.position = { -size, -size, 0, 1.f };
	vert.uv = {0,0};
	myQuadData.myVertices.push_back(vert);
	vert.position = { size, -size, 0, 1.f };
	vert.uv = {1,0};
	myQuadData.myVertices.push_back(vert);
	vert.position = { size, size, 0, 1.f };
	vert.uv = {1,1};
	myQuadData.myVertices.push_back(vert);
	vert.position = { -size, size, 0, 1.f };
	vert.uv = {0,1};
	myQuadData.myVertices.push_back(vert);

	std::vector<unsigned int> indices{
		0, 2, 1,
		0, 3, 2,
	};
	myQuadData.myIndices = indices;

	myMesh = new Mesh();

	myMesh->Initialize(myQuadData.myVertices.data(), myQuadData.myVertices.size(), myQuadData.myIndices.data(), myQuadData.myIndices.size(), true);
}

const QuadData& QuadPrimitive::GetCubeData()
{
	return myQuadData;
}

Mesh* QuadPrimitive::GetMesh()
{
	return myMesh;
}
