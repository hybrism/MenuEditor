#pragma once
#include <memory>
#include <wrl/client.h>
#include "../../math/Vector.h"
#include "../../math/Matrix.h"
#include "../../math/Transform.h"

using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

//TOVE: ALPHABLEND
struct ID3D11BlendState;

class Texture;
class SpriteDrawer;
class VertexShader;
class PixelShader;
class ShaderDatabase;
struct SpriteInstanceData;
struct Sprite3DInstanceData;
struct SpriteSharedData;

struct VertexInstanced
{
	float X, Y, Z, W;      // position
	unsigned int myVertexIndex, unused1, unused2, unused3;
};

struct SpriteShaderInstanceData
{
	Transform transform;
	Vector4f color;
	Vector4f UV;
	Vector4f UVRect;
	Vector4f clipValue;
};

class SpriteBatchScope
{
	friend class SpriteDrawer;
public:
	~SpriteBatchScope();

	void Draw(const SpriteInstanceData& aInstance);
	void Draw(const SpriteInstanceData* aInstances, size_t aInstanceCount);
	void Draw(const Sprite3DInstanceData& aInstance);
	void Draw(const Sprite3DInstanceData* aInstances, size_t aInstanceCount);

private:
	SpriteBatchScope(SpriteDrawer& aSpriteDrawer)
		: mySpriteDrawer(&aSpriteDrawer) {}
	SpriteBatchScope(SpriteBatchScope&& scope) noexcept
		: mySpriteDrawer(scope.mySpriteDrawer)
		, myInstanceData(scope.myInstanceData)
		, myInstanceCount(scope.myInstanceCount)
	{
		scope.mySpriteDrawer = nullptr;
		scope.myInstanceData = nullptr;
		scope.myInstanceCount = 0;

	}
	
	SpriteBatchScope(const SpriteBatchScope&) = delete;
	void operator=(const SpriteBatchScope&) = delete;

	void UnMapAndRender();
	void Map();

	SpriteDrawer* mySpriteDrawer = nullptr;
	SpriteShaderInstanceData* myInstanceData = nullptr;
	size_t myInstanceCount = 0;
};

class SpriteDrawer
{
	friend class SpriteBatchScope;

public:
	SpriteDrawer();
	~SpriteDrawer();
	void Init();

	inline void Draw(const SpriteSharedData& aSharedData, const SpriteInstanceData& aInstance);
	inline void Draw(const SpriteSharedData& aSharedData, const SpriteInstanceData* aInstances, size_t aInstanceCount);
	inline void Draw(const SpriteSharedData& aSharedData, const Sprite3DInstanceData& aInstance);
	inline void Draw(const SpriteSharedData& aSharedData, const Sprite3DInstanceData* aInstances, size_t aInstanceCount);

	SpriteBatchScope BeginBatch(const SpriteSharedData& aSharedData, bool aUseDefaultVertexShader = true);

private:
	void EndBatch();

	bool CreateBuffer();

	ComPtr<ID3D11Buffer> myVertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> myInstanceBuffer = nullptr;
	VertexInstanced myVertices[6] = {};

	const VertexShader* myDefaultVertexShader;
	const VertexShader* my3DVertexShader;
	const PixelShader* myDefaultPixelShader;

	//TOTO: ALPHABLEND
	ComPtr<ID3D11BlendState> myBlendState;

	bool myIsLoaded = false;
	bool myIsInBatch = false;
};


void SpriteDrawer::Draw(const SpriteSharedData& aSharedData, const SpriteInstanceData& aInstance)
{
	SpriteBatchScope scope = BeginBatch(aSharedData);
	scope.Draw(aInstance);
}

void SpriteDrawer::Draw(const SpriteSharedData& aSharedData, const SpriteInstanceData* aInstances, size_t aInstanceCount)
{
	SpriteBatchScope scope = BeginBatch(aSharedData);
	scope.Draw(aInstances, aInstanceCount);
}

inline void SpriteDrawer::Draw(const SpriteSharedData& aSharedData, const Sprite3DInstanceData& aInstance)
{
	SpriteBatchScope scope = BeginBatch(aSharedData, false);
	scope.Draw(aInstance);
}

inline void SpriteDrawer::Draw(const SpriteSharedData& aSharedData, const Sprite3DInstanceData* aInstances, size_t aInstanceCount)
{
	SpriteBatchScope scope = BeginBatch(aSharedData, false);
	scope.Draw(aInstances, aInstanceCount);
}
