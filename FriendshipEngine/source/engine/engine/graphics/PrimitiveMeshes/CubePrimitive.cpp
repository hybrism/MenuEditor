#include "pch.h"
#include "CubePrimitive.h"
#include "engine\graphics\model\Mesh.h"

CubePrimitive::CubePrimitive()
{
	
}

void CubePrimitive::ConstructCube()
{
	float size = 1.0f;

	Vertex vert;

	// front face
	vert.position = { -size, -size, size ,1.f };
	vert.uv = {1,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, -size, size ,1.f };
	vert.uv = {0,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, size, size,1.f };
	vert.uv = {0,1};
	myCubeData.myVertices.push_back(vert);
	vert.position = { -size, size, size, 1.f };
	vert.uv = {1,1};
	myCubeData.myVertices.push_back(vert);
	// back face
	vert.position = { size, -size, -size, 1.f };
	vert.uv = {1,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { -size, -size, -size, 1.f };
	vert.uv = {0,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { -size, size, -size, 1.f };
	vert.uv = {0,1};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, size, -size, 1.f };
	vert.uv = {1,1};
	myCubeData.myVertices.push_back(vert);
	// right face
	vert.position = { size, -size, size, 1.f };
	vert.uv = {1,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, -size, -size, 1.f };
	vert.uv = {0,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, size, -size, 1.f };
	vert.uv = {0,1};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, size, size, 1.f };;
	vert.uv = {1,1};
	myCubeData.myVertices.push_back(vert);
	// left face
	vert.position = { -size, -size, -size, 1.f };
	vert.uv = {1,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { -size, -size, size, 1.f };
	vert.uv = {0,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { -size, size, size, 1.f };
	vert.uv = {0,1};
	myCubeData.myVertices.push_back(vert);
	vert.position = { -size, size, -size, 1.f };
	vert.uv = {1,1};
	myCubeData.myVertices.push_back(vert);
	// top face
	vert.position = { -size, size, size, 1.f };
	vert.uv = {1,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, size, size, 1.f };
	vert.uv = {0,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, size, -size, 1.f };
	vert.uv = {0,1};
	myCubeData.myVertices.push_back(vert);
	vert.position = { -size, size, -size, 1.f };
	vert.uv = {1,1};
	myCubeData.myVertices.push_back(vert);
	// bottom face
	vert.position = { -size, -size, -size, 1.f };
	vert.uv = {1,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, -size, -size, 1.f };
	vert.uv = {0,0};
	myCubeData.myVertices.push_back(vert);
	vert.position = { size, -size, size, 1.f };
	vert.uv = {0,1};
	myCubeData.myVertices.push_back(vert);
	vert.position = { -size, -size, size,  1.f };
	vert.uv = {1,1};
	myCubeData.myVertices.push_back(vert);


	std::vector<unsigned int> indices{
		0, 2, 1,
		0, 3, 2,

		// back face
		4, 6, 5,
		4, 7, 6,

		// right face
		8, 10, 9,
		8, 11, 10,

		// left face
		12, 14, 13,
		12, 15, 14,

		// top face
		16, 18, 17,
		16, 19, 18,

		// bottom face
		20, 22, 21,
		20, 23, 22
	};
	myCubeData.myIndices = indices;


	myCubeMesh = new Mesh;

	myCubeMesh->Initialize(myCubeData.myVertices.data(), myCubeData.myVertices.size(), myCubeData.myIndices.data(), myCubeData.myIndices.size(), true);
}

const CubeData& CubePrimitive::GetCubeData()
{
	return myCubeData;
}

Mesh* CubePrimitive::GetMesh()
{
	return myCubeMesh;
}
