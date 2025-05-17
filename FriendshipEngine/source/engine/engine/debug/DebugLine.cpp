#include "pch.h"
#include <utility/Error.h>

#include "DebugLine.h"
#include <assets/ShaderDatabase.h>
#include "../graphics/GraphicsEngine.h"
#include "../shaders/PixelShader.h"

DebugLine::DebugLine()
{
	int i = 0;
	i++;
	//myStartPosition = { 0.f, 0.f,0.f };
	//myEndPosition = { 0.f, 0.f,0.f };
	//myColor = { 1.f, 1.f, 1.f, 1.f };

	//const unsigned int vertexCount = 2;
	//const unsigned int indexCount = 3;
	//myVertices.resize(vertexCount);

	//myVertices[0].position = { myStartPosition.x, myStartPosition.y, myStartPosition.z, 1 };
	//myVertices[1].position = { myEndPosition.x, myEndPosition.y, myEndPosition.z, 1 };

	//myVertices[0].uv.x = 0; myVertices[0].uv.y = 1;
	//myVertices[1].uv.x = 0; myVertices[1].uv.y = 0;

	//unsigned int indices[indexCount] = { 0, 1, 0 };

	//myMesh.Initialize(myVertices.data(), vertexCount, indices, indexCount, true);
	//myMesh.SetVertexShader(ShaderDatabase::GetVertexShader(VsType::DefaultPBRInstanced));
	//myMesh.SetPixelShader(ShaderDatabase::GetPixelShader(PsType::DebugLine));

	//SetColor(myColor);
}

DebugLine::DebugLine(const Vector3f& , const Vector3f& , const Vector4f& )
{
	//myStartPosition = aStart;
	//myEndPosition = aEnd;
	//myColor = aColor;

	//const unsigned int vertexCount = 2;
	//const unsigned int indexCount = 3;
	//myVertices.resize(vertexCount);

	//myVertices[0].position = { myStartPosition.x, myStartPosition.y, myStartPosition.z, 1 };
	//myVertices[1].position = { myEndPosition.x, myEndPosition.y, myEndPosition.z, 1 };

	//myVertices[0].uv.x = 0; myVertices[0].uv.y = 1;
	//myVertices[1].uv.x = 0; myVertices[1].uv.y = 0;

	//unsigned int indices[indexCount] = { 0, 1, 0 };

	//myMesh.Initialize(myVertices.data(), vertexCount, indices, indexCount, true);
	//myMesh.SetVertexShader(ShaderDatabase::GetVertexShader(VsType::DefaultPBRInstanced));
	//myMesh.SetPixelShader(ShaderDatabase::GetPixelShader(PsType::DebugLine));

	//SetColor(myColor);
	//UpdateVertices();
}

void DebugLine::SetStartAndEnd(const Vector3f& , const Vector3f& )
{
	//SetStartPosition(aStart);
	//SetEndPosition(aEnd);
}

void DebugLine::SetStartPosition(const Vector3f& )
{
	//myStartPosition = aStartPoint;
	//myVertices[0].position = { myStartPosition.x, myStartPosition.y, myStartPosition.z, 1 };
}

void DebugLine::SetEndPosition(const Vector3f& )
{
	//myEndPosition = aEndPoint;
	//myVertices[1].position = { myEndPosition.x, myEndPosition.y, myEndPosition.z, 1 };
}

void DebugLine::DrawLine()
{
	if (!myPrintError)
	{
		PrintW("[DrawLine.cpp] This class is not in use anymore! Please use DebugRenderer instead :) It lives in GraphicsEngine");
		myPrintError = true;
	}
	/*MeshInstanceRenderData instanceData;
	instanceData.renderMode = RenderMode::LINELIST;
	instanceData.psType = myMesh.GetPixelShader()->GetType();
	instanceData.data = StaticMeshInstanceData{ myTransform };
	instanceData;*/

	//StaticMeshInstanceData instanceData{};

	//instanceData.transform.SetPosition(myTransform.GetPosition());
	//

	//GraphicsEngine::GetInstance()->GetForwardRenderer().Render(
	//	static_cast<Mesh*>(&myMesh),
	//	{
	//			instanceData,
	//			VsType::DefaultPBRInstanced,
	//			PsType::DebugLine,
	//			RenderMode::LINELIST,
	//			{}
	//	});

	//	
	//GraphicsEngine::GetInstance()->GetForwardRenderer().Render(&myMesh, instanceData);
}

void DebugLine::SetPosition(const Vector3f& )
{
	//myTransform.SetPosition(aPosition);
}

void DebugLine::SetColor(const Vector4f& )
{
	//for (Vertex& v : myVertices)
	//{
	//	v.color = aColor;
	//}
}
