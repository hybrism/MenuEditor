#include "pch.h"
#include "DebugLine.h"

#include "../math/Vector4.h"
#include "../graphics/model/Mesh.h"
#include <assets/ShaderDatabase.h>

DebugLine::DebugLine(const Vector3<float>& aStartPosition, const Vector3<float>& aEndPosition)
{
	myStartPosition = aStartPosition;
	myEndPosition = aEndPosition;
	myColor = { 0, 1, 0, 1 };
}

DebugLine::DebugLine(const Vector3<float>& aStartPosition, const Vector3<float>& aEndPosition, const Vector4<float>& aColor)
{
	myStartPosition = aStartPosition;
	myEndPosition = aEndPosition;
	myColor = aColor;
}

DebugLine::~DebugLine()
{
	delete myMesh;
}

void DebugLine::Init()
{
	const unsigned int vertexCount = 2;
	const unsigned int indexCount = 3;

	myVertices.resize(vertexCount);

	myVertices[0].position = { myStartPosition.x, myStartPosition.y, myStartPosition.z, 1 };
	myVertices[1].position = { myEndPosition.x, myEndPosition.y, myEndPosition.z, 1 };

	myVertices[0].uv.x = 0; myVertices[0].uv.y = 1;
	myVertices[1].uv.x = 0; myVertices[1].uv.y = 0;

	myVertices[0].color = { 0,1,0,1 };
	myVertices[1].color = { 0,1,0,1 };


	unsigned int indices[indexCount] = { 0, 1, 0 };

	myMesh = new Mesh();
	myMesh->Initialize(myVertices.data(), vertexCount, indices, indexCount, true);
	myMesh->SetVertexShader(ShaderDatabase::GetVertexShader(VsType::DefaultPBRInstanced));
	myMesh->SetPixelShader(ShaderDatabase::GetPixelShader(PsType::DebugLine));
}

void DebugLine::SetStartPosition(const Vector3<float>& aStartPoint)
{
	myStartPosition = aStartPoint;
	myVertices[0].position = { myStartPosition.x, myStartPosition.y, myStartPosition.z, 1 };
	UpdateVertices();
}

void DebugLine::SetEndPosition(const Vector3<float>& aEndPoint)
{
	myEndPosition = aEndPoint;
	myVertices[1].position = { myEndPosition.x, myEndPosition.y, myEndPosition.z, 1 };
	UpdateVertices();
}

#include "../graphics/GraphicsEngine.h"
#include "../shaders/PixelShader.h"
void DebugLine::DrawLine()
{
	// you need to initialize the DebugLine first, DONT DO THIS DURING RENDER
	if (myMesh == nullptr) { return; }

	auto* instance = GraphicsEngine::GetInstance();
	//auto* context = instance->GetContext();
	//context->OMSetRenderTargets(1, instance->GetBackBuffer().GetAddressOf(), nullptr);
	instance;
	MeshInstanceRenderData instanceData;
	instanceData.renderMode = RenderMode::LINELIST;
	instanceData.psType = myMesh->GetPixelShader()->GetType();
	instanceData.data = StaticMeshInstanceData{ myTransform };
	instanceData;
	//instance->GetForwardRenderer().Render(myMesh, instanceData);
	//myMesh->Render(myTransform.GetMatrix(), myMesh->GetVertexShader(), myMesh->GetPixelShader(), RenderMode::LINELIST);
	
	//instance->SetBackBufferAsActiveTarget();
}

void DebugLine::SetPosition(const Vector3<float>& aPosition)
{
	myTransform.SetPosition(aPosition);
}

void DebugLine::SetColor(const Vector4<float>& aColor)
{
	for (auto& v : myVertices)
	{
		v.color = aColor;
	}
	UpdateVertices();
}

void DebugLine::UpdateVertices()
{
	myMesh->UpdateVertices(myVertices.data(), static_cast<unsigned int>(myVertices.size()));
}
