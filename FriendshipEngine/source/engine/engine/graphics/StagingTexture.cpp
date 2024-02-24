#include "pch.h"
#include "StagingTexture.h"
#include <d3d11.h>
#include "GraphicsEngine.h"

#ifdef _DEBUG
void StagingTexture::CopyToStaging() const
{
	GraphicsEngine::GetInstance()->GetContext()->CopyResource(myStagingTexture, myTexture);
}

bool StagingTexture::Map(D3D11_MAPPED_SUBRESOURCE& outData) const
{
	auto* context = GraphicsEngine::GetInstance()->GetContext();

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
	GraphicsEngine::GetInstance()->GetContext()->Unmap(myStagingTexture, 0);
}
#endif

void StagingTexture::CreateStagingTexture()
{
#ifdef _DEBUG
	auto& device = GraphicsEngine::GetInstance()->GetDevice();//GlobalStuff::GetInstance().GetGraphicsEngine()->GetDevice();

	D3D11_TEXTURE2D_DESC stagingDesc{};
	myTexture->GetDesc(&stagingDesc);

	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingDesc.Usage = D3D11_USAGE_STAGING;

	HRESULT result = device->CreateTexture2D(&stagingDesc, nullptr, &myStagingTexture);
	assert(SUCCEEDED(result));
#else
	__noop;
#endif
}
