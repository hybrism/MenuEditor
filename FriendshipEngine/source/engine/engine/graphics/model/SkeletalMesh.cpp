#include "pch.h"
#include "SkeletalMesh.h"
#include "../GraphicsEngine.h"
#include "../../utility/Error.h"
#include "../GraphicsData.h"
#include <cassert>
#include <iostream>
#include "../animation\AnimationPlayer.h"
#include <DirectXMath.h>

#include "../../shaders/PixelShader.h"
#include "../../shaders/VertexShader.h"
#ifndef _RELEASE
#include "../../debug/DebugLine.h"
#endif

SkeletalMesh::SkeletalMesh() = default;
SkeletalMesh::~SkeletalMesh()
{
}

bool SkeletalMesh::Initialize(
	AnimatedVertex aVerts[],
	const size_t& aVertexCount,
	unsigned int aIndices[],
	const size_t& aIndexCount,
	const bool& aUseDynamicVertexBuffer
)
{
	myVertexSize = sizeof(AnimatedVertex);

	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->DX().GetDevice();
	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = static_cast<UINT>(myVertexSize * aVertexCount);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		if (aUseDynamicVertexBuffer)
		{
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		D3D11_SUBRESOURCE_DATA vertexData = { 0 };

		vertexData.pSysMem = aVerts;
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
		{
			PrintE("Could not create Vertex Buffer [SkeletalMesh.cpp]");
			return false;
		}
	}

	{
		// Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * aIndexCount);
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = aIndices;
		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData,
			&myIndexBuffer);
		if (FAILED(result))
		{
			PrintE("Could not create Vertex Buffer [SkeletalMesh.cpp]");
			return false;
		}
	}

	myIndexCount = static_cast<unsigned int>(aIndexCount);

	for (int i = 0; i < MAX_ANIMATION_BONES; i++)
	{
		myPose.transform[i] = DirectX::XMMatrixIdentity();
	}
	myPose.count = MAX_ANIMATION_BONES;

	return true;
}

void SkeletalMesh::Render(const DirectX::XMMATRIX& aTransform, const VertexShader* aVS, const PixelShader* aPS, const RenderMode& aRenderMode) const
{
	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->DX().GetContext();

	// Object Buffer
	{
		auto objectBuffer = instance->GetObjectBuffer();

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		auto result = context->Map(objectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

		if (FAILED(result))
		{
			PrintE("Could not map ObjectBuffer [SkeletalMesh.cpp]");
			return;
		}

		ObjectBufferData* data = (ObjectBufferData*)mappedBuffer.pData;
		data->objectTransform = aTransform; // column 4 ger icke 0 resultat in i shadern????

		context->Unmap(objectBuffer.Get(), 0);
		context->VSSetConstantBuffers((UINT)BufferSlots::Object, 1, objectBuffer.GetAddressOf());
		context->PSSetConstantBuffers((UINT)BufferSlots::Object, 1, objectBuffer.GetAddressOf());
	}

	BindTextures();

	if (aPS) { aPS->PrepareRender(); }
	if (aVS) { aVS->PrepareRender(BufferSlots::Bone, (void*)myPose.transform, sizeof(DirectX::XMMATRIX) * MAX_ANIMATION_BONES); }

	unsigned int stride = myVertexSize;
	unsigned int offset = 0;
	context->IASetVertexBuffers((UINT)BufferSlots::Frame, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)aRenderMode);

	context->DrawIndexed(myIndexCount, 0, 0);

	instance->IncrementDrawCalls();
}


void SkeletalMesh::RenderSkeleton(const DirectX::XMMATRIX& aTransform)
{
#ifndef _RELEASE

	for (int i = 0; i < (int)myDebugLines.size() - 1; ++i)
	{
		if (mySkeleton.bones[i].childrenIds.size() == 0) continue;

		auto& line = myDebugLines[i];
		DirectX::XMVECTOR pos0 = { 0, 0, 0, 1 };

		auto modelSpace0 = mySkeleton.GetBoneModelSpaceTransform(myPose, i).GetMatrix() * aTransform;
		auto modelSpace1 = mySkeleton.GetBoneModelSpaceTransform(myPose, i+1).GetMatrix() * aTransform;

		auto p0 = DirectX::XMVector4Transform(pos0, modelSpace0);
		auto p1 = DirectX::XMVector4Transform(pos0, modelSpace1);

		Vector3f startPos = { p0.m128_f32[0], p0.m128_f32[1], p0.m128_f32[2] };
		Vector3f endPos = { p1.m128_f32[0], p1.m128_f32[1], p1.m128_f32[2] };

		line.SetStartPosition(startPos);
		line.SetEndPosition(endPos);
		
		line.DrawLine();
	}
#else
	aTransform;
#endif
}

void SkeletalMesh::SetSkeleton(const Skeleton& aSkeleton)
{
	mySkeleton = aSkeleton;
#ifndef _RELEASE
	myDebugLines.clear();
	myDebugLines.resize(mySkeleton.bones.size(), DebugLine({ 0, 0, 0 }, { 1, 1, 1 }));
#endif
}

const Skeleton& SkeletalMesh::GetSkeleton() const
{
	return mySkeleton;
}

void SkeletalMesh::SetPose(const Pose& aLocalSpacePose)
{
	if (aLocalSpacePose.count == 0)
	{
		memcpy(myPose.transform, aLocalSpacePose.transform, sizeof(DirectX::XMMATRIX) * MAX_ANIMATION_BONES);
		return;
	}
	Pose modelSpacePose;
	mySkeleton.ConvertPoseToModelSpace(aLocalSpacePose, modelSpacePose);

	mySkeleton.ApplyBindPoseInverse(modelSpacePose, myPose.transform);
}

void SkeletalMesh::ResetPose()
{
	for (int i = 0; i < MAX_ANIMATION_BONES; i++)
	{
		myPose.transform[i] = DirectX::XMMatrixIdentity();
	}
}