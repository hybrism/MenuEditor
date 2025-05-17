#include "pch.h"
#include "VertexShader.h"
#include <d3d11.h>
#include "../Paths.h"

#include "../graphics/GraphicsEngine.h"
#include "../utility/Error.h"

VertexShader::VertexShader()
	: Shader(GraphicsEngine::GetInstance()), myInstanceBuffer()
{}

VertexShader::~VertexShader() {}

bool VertexShader::Init(
	const std::string& aName,
	const VsType aType,
	D3D11_INPUT_ELEMENT_DESC aLayout[],
	const size_t& aSize,
	const DxBufferDesc* aInstanceBufferDesc,
	const D3D11_BUFFER_DESC* aConstantBufferDesc,
	const D3D11_SUBRESOURCE_DATA* aConstantBufferSubresourceData
)
{
	HRESULT result;
	std::string vsData;
	auto& device = myGraphicsEngine->DX().GetDevice();

	std::ifstream vsFile;
	vsFile.open(RELATIVE_SHADER_PATH + aName + "_VS.cso", std::ios::binary);
	vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myShader);
	if (FAILED(result))
	{
		PrintE("[Shader.cpp] Could not create VertexShader!");
		return false;
	}
	vsFile.close();

	myType = aType;

	if (aLayout == nullptr) { return true; }

	if (!SetInputLayout(aLayout, aSize, vsData)) { return false; }

	if (!InternalInit(aName, aConstantBufferDesc, aConstantBufferSubresourceData)) { return false; }

	if (aInstanceBufferDesc == nullptr) { return true; }

	return myInstanceBuffer.Init(*aInstanceBufferDesc);
}

bool VertexShader::SetInputLayout(D3D11_INPUT_ELEMENT_DESC aLayout[], const size_t& aSize, const std::string& aVsData)
{
	if (myInputLayout)
	{
		myInputLayout.Reset();
	}

	D3D11_INPUT_ELEMENT_DESC* layout = aLayout;
	unsigned int numElements = static_cast<unsigned int>(aSize / sizeof(D3D11_INPUT_ELEMENT_DESC));
	HRESULT result = S_OK;

	// TODO: Make this prettier
	if (!aLayout)
	{
		// DEFAULT INPUT LAYOUT
		D3D11_INPUT_ELEMENT_DESC l[] =
		{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		};
		numElements = static_cast<unsigned int>(aSize / sizeof(D3D11_INPUT_ELEMENT_DESC));

		result = myGraphicsEngine->DX().GetDevice()->CreateInputLayout(l, numElements, aVsData.data(), aVsData.size(), myInputLayout.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			PrintE("[Shader.cpp] Could not create InputLayout!");
			return false;
		}
		return true;
	}

	result = myGraphicsEngine->DX().GetDevice()->CreateInputLayout(layout, numElements, aVsData.data(), aVsData.size(), myInputLayout.ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		PrintE("[Shader.cpp] Could not create InputLayout!");
		return false;
	}

	return true;
}


bool VertexShader::PrepareRender(
	const BufferSlots& aSlot,
	void* aBufferData,
	const size_t& aSize
) const
{
	if (!myShader)
	{
		PrintE("[VertexShader.cpp] PrepareRender failed!");
		return false;
	}

	auto* context = myGraphicsEngine->DX().GetContext();
	context->VSSetShader(myShader.Get(), NULL, 0);
	context->IASetInputLayout(myInputLayout.Get());

	if (!Shader::PrepareRender(aSlot, aBufferData, aSize)) { return false; }

	if (myConstantBuffer != nullptr)
	{
		context->VSSetConstantBuffers((UINT)aSlot, 1, myConstantBuffer.GetAddressOf());
	}

	return true;
}
