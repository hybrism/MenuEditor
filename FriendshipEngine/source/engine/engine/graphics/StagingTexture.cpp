#include "pch.h"
#include "StagingTexture.h"
#include <d3d11.h>
#include "GraphicsEngine.h"

void StagingTexture::CopyToStaging() const
{
	auto* context = GraphicsEngine::GetInstance()->DX().GetContext();
#ifdef _EDITOR
	context->CopyResource(myStagingTexture, myTexture);
#endif
	context->CopyResource(myIntermediateTexture, myTexture);
}

#ifdef _EDITOR
bool StagingTexture::Map(D3D11_MAPPED_SUBRESOURCE& outData) const
{
	auto* context = GraphicsEngine::GetInstance()->DX().GetContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = context->Map(myStagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	outData = mappedResource;
	return true;
}

void StagingTexture::Unmap() const
{
	GraphicsEngine::GetInstance()->DX().GetContext()->Unmap(myStagingTexture, 0);
}
#endif

void StagingTexture::CreateStagingTexture(D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc)
{
	HRESULT result = S_OK;

	auto& device = GraphicsEngine::GetInstance()->DX().GetDevice();//GlobalStuff::GetInstance().GetGraphicsEngine()->GetDevice();
#ifdef _EDITOR
	{
		D3D11_TEXTURE2D_DESC stagingDesc{};
		myTexture->GetDesc(&stagingDesc);

		stagingDesc.BindFlags = 0;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.Usage = D3D11_USAGE_STAGING;

		result = device->CreateTexture2D(&stagingDesc, nullptr, &myStagingTexture);
		assert(SUCCEEDED(result));
	}
#endif

	{
		D3D11_TEXTURE2D_DESC stagingDesc{};
		myTexture->GetDesc(&stagingDesc);

		stagingDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

		result = device->CreateTexture2D(&stagingDesc, nullptr, &myIntermediateTexture);
		assert(SUCCEEDED(result));
	}

	ID3D11ShaderResourceView* tempSRV;
	result = device->CreateShaderResourceView(myIntermediateTexture, aSRVDesc, &tempSRV);
	assert(SUCCEEDED(result));
	myIntermediateSRV = tempSRV;
	tempSRV->Release();
}
