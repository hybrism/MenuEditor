#include "pch.h"
#include "PixelShader.h"
#include "../Paths.h"

#include "../graphics/GraphicsEngine.h"
#include "../utility/Error.h"

PixelShader::PixelShader()
	: Shader(GraphicsEngine::GetInstance())
{}

PixelShader::~PixelShader() {}

bool PixelShader::Init(
	const std::string& aName,
	const PsType aType,
	const D3D11_BUFFER_DESC* aBufferDesc,
	const D3D11_SUBRESOURCE_DATA* aSubresourceData
)
{
	HRESULT result;
	std::string vsData;

	std::ifstream psFile;
	psFile.open(RELATIVE_SHADER_PATH + aName + "_PS.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	result = myGraphicsEngine->DX().GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &myShader);
	if (FAILED(result))
	{
		PrintE("[Shader.cpp] Could not create PixelShader!");
		return false;
	}
	psFile.close();

	myType = aType;

	return InternalInit(aName, aBufferDesc, aSubresourceData);
}

bool PixelShader::PrepareRender(
	const BufferSlots& aSlot,
	void* aBufferData,
	const size_t& aSize
) const
{
	if (!myShader)
	{
		PrintE("[PixelShader.cpp] PrepareRender failed!");
		return false;
	}

	auto* context = myGraphicsEngine->DX().GetContext();
	context->PSSetShader(myShader.Get(), NULL, 0);

	if (!Shader::PrepareRender(aSlot, aBufferData, aSize)) { return false; }

	if (myConstantBuffer != nullptr)
	{
		context->PSSetConstantBuffers((UINT)aSlot, 1, myConstantBuffer.GetAddressOf());
	}

	return true;
}
