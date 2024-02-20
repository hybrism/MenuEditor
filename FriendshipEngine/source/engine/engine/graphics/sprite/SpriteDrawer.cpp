#include "pch.h"
#include "SpriteDrawer.h"

#include "../../Paths.h"

#include "shaders/VertexShader.h"
#include "shaders/PixelShader.h"
#include "../../utility/Error.h"
#include "Sprite.h"
#include "../GraphicsEngine.h"
#include "../../../assets/ShaderDatabase.h"

constexpr size_t BATCH_SIZE = 1024;

#pragma region SpriteBatchScope

SpriteBatchScope::~SpriteBatchScope()
{
	if (mySpriteDrawer != nullptr)
	{
		UnMapAndRender();
		mySpriteDrawer->EndBatch();
		mySpriteDrawer = nullptr;
	}
}

void SpriteBatchScope::Draw(const SpriteInstanceData& aInstance)
{
	Draw(&aInstance, 1);
};

void SpriteBatchScope::Draw(const SpriteInstanceData* aInstances, size_t aInstanceCount)
{
	assert(myInstanceCount < BATCH_SIZE);

	Vector2<unsigned int> resolution = GraphicsEngine::GetInstance()->GetViewportDimensions();

	for (int i = 0; i < aInstanceCount; i++)
	{
		const SpriteInstanceData& instance = aInstances[i];

		if (instance.myIsHidden)
			continue;

		SpriteShaderInstanceData& shaderInstance = myInstanceData[myInstanceCount];

		Vector2f pivot = Vector2f(-instance.pivot.x, instance.pivot.y);

		Matrix2x2f scalingMatrix = Matrix2x2f::CreateScaleMatrix(Vector2f((instance.scale.x) * instance.scaleMultiplier.x, (instance.scale.y) * instance.scaleMultiplier.y));
		Matrix2x2f rotationMatrix = Matrix2x2f::CreateRotation(instance.rotation);

		Matrix2x2f m = scalingMatrix * rotationMatrix;
		Vector2f p = pivot * m + instance.position;

		Transform t;
		t.SetPosition({ p.x, p.y, 1.f });
		t.Scale({m(1,1), m(2,2), 1.f});

		shaderInstance.transform = t;

		shaderInstance.UVRect.x = instance.textureRect.startX;
		shaderInstance.UVRect.y = instance.textureRect.endY;
		shaderInstance.UVRect.z = instance.textureRect.endX;
		shaderInstance.UVRect.w = instance.textureRect.startY;

		shaderInstance.UV.x = instance.UV.x;
		shaderInstance.UV.y = instance.UV.y;
		shaderInstance.UV.z = instance.UVscale.x;
		shaderInstance.UV.w = instance.UVscale.y;

		shaderInstance.color = instance.color;

		shaderInstance.clipValue.x = instance.clip.left;
		shaderInstance.clipValue.y = instance.clip.right;
		shaderInstance.clipValue.z = instance.clip.down;
		shaderInstance.clipValue.w = instance.clip.upper;

		myInstanceCount++;
		if (myInstanceCount >= BATCH_SIZE)
		{
			UnMapAndRender();
			Map();
		}
	}
}

void SpriteBatchScope::Draw(const Sprite3DInstanceData& aInstance)
{
	Draw(&aInstance, 1);
}

void SpriteBatchScope::Draw(const Sprite3DInstanceData* aInstances, size_t aInstanceCount)
{
	assert(myInstanceCount < BATCH_SIZE);

	for (int i = 0; i < aInstanceCount; i++)
	{
		const Sprite3DInstanceData& instance = aInstances[i];

		if (instance.myIsHidden)
			continue;

		SpriteShaderInstanceData& shaderInstance = myInstanceData[myInstanceCount];

		shaderInstance.transform = instance.myTransform;

		shaderInstance.UVRect.x = instance.textureRect.startX;
		shaderInstance.UVRect.y = instance.textureRect.endY;
		shaderInstance.UVRect.z = instance.textureRect.endX;
		shaderInstance.UVRect.w = instance.textureRect.startY;

		shaderInstance.UV.x = instance.UV.x;
		shaderInstance.UV.y = instance.UV.y;
		shaderInstance.UV.z = instance.UVscale.x;
		shaderInstance.UV.w = instance.UVscale.y;

		shaderInstance.color = instance.color;

		shaderInstance.clipValue.x = instance.clip.left;
		shaderInstance.clipValue.y = instance.clip.right;
		shaderInstance.clipValue.z = instance.clip.down;
		shaderInstance.clipValue.w = instance.clip.upper;

		myInstanceCount++;
		if (myInstanceCount >= BATCH_SIZE)
		{
			UnMapAndRender();
			Map();
		}
	}
}

void SpriteBatchScope::UnMapAndRender()
{
	assert(mySpriteDrawer);

	ID3D11DeviceContext* context = GraphicsEngine::GetInstance()->GetContext();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Unmap(mySpriteDrawer->myInstanceBuffer.Get(), 0);

	//1 QUAD = 6 Indices
	context->DrawInstanced(6, (UINT)myInstanceCount, 0, 0);

#ifdef _DEBUG
	GraphicsEngine::GetInstance()->IncrementDrawCalls();
#endif
	myInstanceData = nullptr;
	myInstanceCount = 0;
}

void SpriteBatchScope::Map()
{
	assert(mySpriteDrawer);
	assert(myInstanceCount == 0);

	D3D11_MAPPED_SUBRESOURCE mappedObjectResource;
	HRESULT result = GraphicsEngine::GetInstance()->GetContext()->Map(mySpriteDrawer->myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObjectResource);
	if (FAILED(result))
	{
		PrintE("[SpriteDrawer.cpp] Error in rendering! Function: SpriteBatchScope::Map()");
		return;
	}

	myInstanceData = static_cast<SpriteShaderInstanceData*>(mappedObjectResource.pData);
}
#pragma endregion //!SpriteBatchScope


#pragma region SpriteDrawer

SpriteDrawer::SpriteDrawer()
{
}

SpriteDrawer::~SpriteDrawer()
{
}

void SpriteDrawer::Init()
{
	D3D11_BUFFER_DESC objectBufferDesc;
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(SpriteShaderInstanceData) * BATCH_SIZE;
	objectBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;

	HRESULT result;
	result = GraphicsEngine::GetInstance()->GetDevice()->CreateBuffer(&objectBufferDesc, nullptr, myInstanceBuffer.ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		PrintE("[SpriteDrawer.cpp] Object Buffer error");
		return;
	}

	myDefaultPixelShader = ShaderDatabase::GetPixelShader(PsType::Sprite);
	myDefaultVertexShader = ShaderDatabase::GetVertexShader(VsType::InstancedSprite);
	my3DVertexShader = ShaderDatabase::GetVertexShader(VsType::Instanced3DSprite);

	myIsLoaded = CreateBuffer();
}

bool SpriteDrawer::CreateBuffer()
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	float startSize = 1.0f;
	float theZ = 0.0f;
	myVertices[0].X = -0;
	myVertices[0].Y = -startSize;
	myVertices[0].Z = theZ;
	myVertices[0].W = 1.0f;
	myVertices[0].myVertexIndex = 0;

	myVertices[1].X = -0;
	myVertices[1].Y = -0;
	myVertices[1].Z = theZ;
	myVertices[1].W = 1.0f;
	myVertices[1].myVertexIndex = 1;

	myVertices[2].X = startSize;
	myVertices[2].Y = -startSize;
	myVertices[2].Z = theZ;
	myVertices[2].W = 1.0f;
	myVertices[2].myVertexIndex = 2;

	myVertices[3].X = startSize;
	myVertices[3].Y = -0;
	myVertices[3].Z = theZ;
	myVertices[3].W = 1.0f;
	myVertices[3].myVertexIndex = 3;

	myVertices[4].X = startSize;
	myVertices[4].Y = -startSize;
	myVertices[4].Z = theZ;
	myVertices[4].W = 1.0f;
	myVertices[4].myVertexIndex = 4;

	myVertices[5].X = -0;
	myVertices[5].Y = -0;
	myVertices[5].Z = theZ;
	myVertices[5].W = 1.0f;
	myVertices[5].myVertexIndex = 5;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexInstanced) * 6;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = myVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT hr = GraphicsEngine::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, myVertexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		PrintE("[SpriteDrawer.cpp] Buffer error");
		return false;
	}

	return true;
}

SpriteBatchScope SpriteDrawer::BeginBatch(const SpriteSharedData& aSharedData, bool aUseDefaultVertexShader)
{
	assert(myIsLoaded);
	assert(!myIsInBatch);
	myIsInBatch = true;

	aSharedData.texture->Bind(ShaderTextureSlot::Cubemap);

	if (aUseDefaultVertexShader)
	{
		myDefaultVertexShader->PrepareRender();
	}
	else
	{
		my3DVertexShader->PrepareRender();
	}

	myDefaultPixelShader->PrepareRender();

	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* spriteBuffer[2];

	strides[0] = sizeof(VertexInstanced);
	strides[1] = sizeof(SpriteShaderInstanceData);

	offsets[0] = 0;
	offsets[1] = 0;

	spriteBuffer[0] = myVertexBuffer.Get();
	spriteBuffer[1] = myInstanceBuffer.Get();

	GraphicsEngine::GetInstance()->GetContext()->IASetVertexBuffers(0, 2, spriteBuffer, strides, offsets);

	SpriteBatchScope scope(*this);
	scope.Map();

	return scope;
}

void SpriteDrawer::EndBatch()
{
	assert(myIsInBatch);
	myIsInBatch = false;
}

#pragma endregion //!SpriteDrawer