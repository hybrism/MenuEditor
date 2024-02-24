#include "pch.h"
#include "DebugLine.h"
#include <assets/ShaderDatabase.h>
#include "../graphics/GraphicsEngine.h"
#include "../shaders/PixelShader.h"

DebugLine::DebugLine()
{
	myStartPosition = { 0.f, 0.f,0.f };
	myEndPosition = { 0.f, 0.f,0.f };
	myColor = { 1.f, 1.f, 1.f, 1.f };

	const unsigned int vertexCount = 2;
	const unsigned int indexCount = 3;
	myVertices.resize(vertexCount);

	myVertices[0].position = { myStartPosition.x, myStartPosition.y, myStartPosition.z, 1 };
	myVertices[1].position = { myEndPosition.x, myEndPosition.y, myEndPosition.z, 1 };

	myVertices[0].uv.x = 0; myVertices[0].uv.y = 1;
	myVertices[1].uv.x = 0; myVertices[1].uv.y = 0;

	unsigned int indices[indexCount] = { 0, 1, 0 };

	myMesh.Initialize(myVertices.data(), vertexCount, indices, indexCount, true);
	myMesh.SetVertexShader(ShaderDatabase::GetVertexShader(VsType::DefaultPBRInstanced));
	myMesh.SetPixelShader(ShaderDatabase::GetPixelShader(PsType::DebugLine));

	SetColor(myColor);
	UpdateVertices();
}

DebugLine::DebugLine(const Vector3f& aStart, const Vector3f& aEnd, const Vector4f& aColor)
{
	myStartPosition = aStart;
	myEndPosition = aEnd;
	myColor = aColor;

	const unsigned int vertexCount = 2;
	const unsigned int indexCount = 3;
	myVertices.resize(vertexCount);

	myVertices[0].position = { myStartPosition.x, myStartPosition.y, myStartPosition.z, 1 };
	myVertices[1].position = { myEndPosition.x, myEndPosition.y, myEndPosition.z, 1 };

	myVertices[0].uv.x = 0; myVertices[0].uv.y = 1;
	myVertices[1].uv.x = 0; myVertices[1].uv.y = 0;

	unsigned int indices[indexCount] = { 0, 1, 0 };

	myMesh.Initialize(myVertices.data(), vertexCount, indices, indexCount, true);
	myMesh.SetVertexShader(ShaderDatabase::GetVertexShader(VsType::DefaultPBRInstanced));
	myMesh.SetPixelShader(ShaderDatabase::GetPixelShader(PsType::DebugLine));

	SetColor(myColor);
	UpdateVertices();
}

void DebugLine::SetStartAndEnd(const Vector3f& aStart, const Vector3f& aEnd)
{
	SetStartPosition(aStart);
	SetEndPosition(aEnd);
}

void DebugLine::SetStartPosition(const Vector3f& aStartPoint)
{
	myStartPosition = aStartPoint;
	myVertices[0].position = { myStartPosition.x, myStartPosition.y, myStartPosition.z, 1 };
	UpdateVertices();
}

void DebugLine::SetEndPosition(const Vector3f& aEndPoint)
{
	myEndPosition = aEndPoint;
	myVertices[1].position = { myEndPosition.x, myEndPosition.y, myEndPosition.z, 1 };
	UpdateVertices();
}

void DebugLine::DrawLine()
{
	MeshInstanceRenderData instanceData;
	instanceData.renderMode = RenderMode::LINELIST;
	instanceData.psType = myMesh.GetPixelShader()->GetType();
	instanceData.data = StaticMeshInstanceData{ myTransform };

	GraphicsEngine::GetInstance()->GetForwardRenderer().Render(&myMesh, instanceData);
}

void DebugLine::SetPosition(const Vector3f& aPosition)
{
	myTransform.SetPosition(aPosition);
}

void DebugLine::SetColor(const Vector4f& aColor)
{
	for (Vertex& v : myVertices)
	{
		v.color = aColor;
	}

	UpdateVertices();
}

void DebugLine::UpdateVertices()
{
	myMesh.UpdateVertices(myVertices.data(), static_cast<unsigned int>(myVertices.size()));
}
