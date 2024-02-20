#pragma once
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;

struct DxBufferDesc
{
	D3D11_BUFFER_DESC* desc = nullptr;
	unsigned int stride = 0;
	unsigned int offset = 0;
};

class DxBuffer
{
public:
	DxBuffer();
	~DxBuffer();

	bool Init(
		const D3D11_BUFFER_DESC& aDesc,
		const unsigned int aStride,
		const unsigned int aOffset = 0
	);

	bool Init(const DxBufferDesc& aDesc);
	bool Map(void*& aMappableData) const;
	void UnMap(void*& aMappableData) const;
	bool HasBuffer() const { return myBuffer != nullptr; }

	ComPtr<ID3D11Buffer> myBuffer = nullptr;
	unsigned int myStride = 0;
	unsigned int myOffset = 0;
private:
	//D3D11_BUFFER_DESC desc;
	//D3D11_SUBRESOURCE_DATA data;
};
