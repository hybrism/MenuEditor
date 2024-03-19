#include "pch.h"
#include "SpherePrimitive.h"
#include "engine\graphics\model\Mesh.h"


SpherePrimitive::SpherePrimitive()
{
}

void SpherePrimitive::ConstructSphere()
{
	SphereData data;


	int aLatDiv = 10;
	int aLongDiv = 10;
	float PI = 3.14f;

	constexpr float radius = 10.0f;
	const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
	const float lattitudeAngle = PI / aLatDiv;
	const float longitudeAngle = 2.0f * PI / aLongDiv;

	for (int iLat = 1; iLat < aLatDiv; iLat++)
	{
		const auto latBase = DirectX::XMVector3Transform(base,DirectX::XMMatrixRotationX(lattitudeAngle * iLat));
		for (int iLong = 0; iLong < aLongDiv; iLong++)
		{
			Vertex tempVert;
			DirectX::XMFLOAT3 calculatedPos;
			auto v = DirectX::XMVector3Transform(latBase,DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
			DirectX::XMStoreFloat3(&calculatedPos, v);

			tempVert.position = { calculatedPos.x, calculatedPos.y, calculatedPos.z, 1.0f };
			data.myVertices.emplace_back(tempVert);
		}
	}

	// Add the cap vertices
	const auto iNorthPole = static_cast<unsigned short>(data.myVertices.size());
	{
		Vertex northVert;
		DirectX::XMFLOAT3 northPos;
		DirectX::XMStoreFloat3(&northPos, base);

		northVert.position = { northPos.x, northPos.y, northPos.z , 1.0f };
		data.myVertices.emplace_back(northVert);
	}
	const auto iSouthPole = static_cast<unsigned short>(data.myVertices.size());
	{
		Vertex southVert;
		DirectX::XMFLOAT3 southPos;
		DirectX::XMStoreFloat3(&southPos, DirectX::XMVectorNegate(base));

		southVert.position = { southPos.x, southPos.y, southPos.z, 1.0f };
		data.myVertices.emplace_back(southVert);
	}

	const auto calcIdx = [aLatDiv, aLongDiv](unsigned short aILat, unsigned short aILong)
		{
			return aILat * aLongDiv + aILong;
		};

#pragma warning (push)
#pragma warning (disable : 4244)
	for (unsigned short iLat = 0; iLat < aLatDiv - 2; iLat++)
	{
		for (unsigned short iLong = 0; iLong < aLongDiv - 1; iLong++)
		{
			data.myIndices.push_back(calcIdx(iLat, iLong));
			data.myIndices.push_back(calcIdx(iLat + 1, iLong));
			data.myIndices.push_back(calcIdx(iLat, iLong + 1));
			data.myIndices.push_back(calcIdx(iLat, iLong + 1));
			data.myIndices.push_back(calcIdx(iLat + 1, iLong));
			data.myIndices.push_back(calcIdx(iLat + 1, iLong + 1));
		}
		// Wrap band
		data.myIndices.push_back(calcIdx(iLat, aLongDiv - 1));
		data.myIndices.push_back(calcIdx(iLat + 1, aLongDiv - 1));
		data.myIndices.push_back(calcIdx(iLat, 0));
		data.myIndices.push_back(calcIdx(iLat, 0));
		data.myIndices.push_back(calcIdx(iLat + 1, aLongDiv - 1));
		data.myIndices.push_back(calcIdx(iLat + 1, 0));
	}

	// Cap fans
	for (unsigned short iLong = 0; iLong < aLongDiv - 1; iLong++)
	{
		// North
		data.myIndices.push_back(iNorthPole);
		data.myIndices.push_back(calcIdx(0, iLong));
		data.myIndices.push_back(calcIdx(0, iLong + 1));
		// South
		data.myIndices.push_back(calcIdx(aLatDiv - 2, iLong + 1));
		data.myIndices.push_back(calcIdx(aLatDiv - 2, iLong));
		data.myIndices.push_back(iSouthPole);
	}
	// Wrap triangles
	// North
	data.myIndices.push_back(iNorthPole);
	data.myIndices.push_back(calcIdx(0, aLongDiv - 1));
	data.myIndices.push_back(calcIdx(0, 0));
	// South
	data.myIndices.push_back(calcIdx(aLatDiv - 2, 0));
	data.myIndices.push_back(calcIdx(aLatDiv - 2, aLongDiv - 1));
	data.myIndices.push_back(iSouthPole);

	mySphereData = data;

	mySphereMesh = new Mesh;

	for (int i = 0; i < data.myVertices.size(); i++)
	{
		data.myVertices[i].color = { 255,255,255,255 };
	}
	mySphereMesh->Initialize(data.myVertices.data(), data.myVertices.size(), data.myIndices.data(), data.myIndices.size(), true);
#pragma warning (pop)
}

const SphereData& SpherePrimitive::GetSphereData()
{
	return mySphereData;
}

Mesh* SpherePrimitive::GetMesh()
{
	return mySphereMesh;
}
