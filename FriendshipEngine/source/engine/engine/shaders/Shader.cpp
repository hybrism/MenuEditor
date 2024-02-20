#include "pch.h"
#include "Shader.h"
#include "../Paths.h"

#include "../graphics/GraphicsEngine.h"
#include "../utility/Error.h"

Shader::Shader(GraphicsEngine* aGraphicsEngine) { myGraphicsEngine = aGraphicsEngine; }
Shader::~Shader() { myGraphicsEngine = nullptr; }

bool Shader::InternalInit(
	const std::string&,
	const D3D11_BUFFER_DESC* aBufferDesc,
	const D3D11_SUBRESOURCE_DATA* aSubresourceData
)
{
	if (aBufferDesc == nullptr) { return true; }

	HRESULT result = myGraphicsEngine->GetDevice()->CreateBuffer(aBufferDesc, aSubresourceData, &myConstantBuffer);
	if (FAILED(result))
	{
		PrintW("[Shader.cpp] Failed to create buffer for shader");
		return false;
	}

	return true;
}


bool Shader::PrepareRender(
	const BufferSlots& aSlot,
	void* aBufferData,
	const size_t& aSize
) const
{
	if (!myConstantBuffer) { return true; }

	auto* context = myGraphicsEngine->GetContext();

	D3D11_MAPPED_SUBRESOURCE mappedVResource;
	auto result = context->Map(myConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVResource);
	if (FAILED(result))
	{
		PrintE("Could not map BoneBuffer [AnimatedModelShader.cpp]");
		return false;
	}
	char* dataVPtr = (char*)mappedVResource.pData;

	memcpy(dataVPtr, aBufferData, aSize);

	context->Unmap(myConstantBuffer.Get(), 0);
	context->VSSetConstantBuffers((UINT)aSlot, 1, myConstantBuffer.GetAddressOf());

	return true;
}