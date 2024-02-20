#pragma once
#include "Shader.h"
using Microsoft::WRL::ComPtr;

class GraphicsEngine;

class PixelShader : public Shader
{
public:
    PixelShader();
    ~PixelShader() override;
    bool Init(
        const std::string& aName,
        const PsType aType,
        const D3D11_BUFFER_DESC* aBufferDesc = nullptr,
        const D3D11_SUBRESOURCE_DATA* aSubresourceData = nullptr
    );
    bool PrepareRender(
        const BufferSlots& aSlot = BufferSlots::Frame,
        void* aBufferData = nullptr,
        const size_t& aSize = 0
    ) const override;

    PsType GetType() const { return myType; }
    ID3D11PixelShader* GetShader() const { return myShader.Get(); }
private:
    PsType myType;
    ComPtr<ID3D11PixelShader> myShader;
};
