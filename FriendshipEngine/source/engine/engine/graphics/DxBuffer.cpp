#include "pch.h"
#include "DxBuffer.h"
#include "GraphicsEngine.h"
#include <utility/Error.h>

DxBuffer::DxBuffer() = default;
DxBuffer::~DxBuffer() = default;

bool DxBuffer::Init(
	const D3D11_BUFFER_DESC& aDesc,
	const unsigned int aStride,
	const unsigned int aOffset
)
{
	HRESULT result;
	result = GraphicsEngine::GetInstance()->GetDevice()->CreateBuffer(&aDesc, nullptr, myBuffer.ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		PrintE("[DxBuffer.cpp] Buffer error");
		return false;
	}

	myStride = aStride;
	myOffset = aOffset;

	return true;
}

bool DxBuffer::Init(const DxBufferDesc& aDesc)
{
	return Init(*aDesc.desc, aDesc.stride, aDesc.offset);
}

bool DxBuffer::Map(void*& aMappableData) const
{
	D3D11_MAPPED_SUBRESOURCE mappedObjectResource;
	HRESULT result = GraphicsEngine::GetInstance()->GetContext()->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObjectResource);
	if (FAILED(result))
	{
		PrintE("[DxBuffer.cpp] Error in rendering! Function: SpriteBatchScope::Map()");
		return false;
	}

	aMappableData = mappedObjectResource.pData;
	return true;
}

void DxBuffer::UnMap(void*& aMappableData) const
{
	GraphicsEngine::GetInstance()->GetContext()->Unmap(myBuffer.Get(), 0);
	aMappableData = nullptr;
}
