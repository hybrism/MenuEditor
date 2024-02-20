#pragma once
#include "Shader.h"
#include "../graphics/DxBuffer.h"

using Microsoft::WRL::ComPtr;

class GraphicsEngine;

class VertexShader : public Shader
{
public:
    VertexShader();
    ~VertexShader() override;
    bool Init(
        const std::string& aName,
        const VsType aType,
        D3D11_INPUT_ELEMENT_DESC aLayout[],
        const size_t& aSize,
        const DxBufferDesc* aInstanceBufferDesc = nullptr,
        const D3D11_BUFFER_DESC* aConstantBufferDesc = nullptr,
        const D3D11_SUBRESOURCE_DATA* aSubresourceData = nullptr
    );
    bool PrepareRender(
        const BufferSlots& aSlot = BufferSlots::Frame,
        void* aBufferData = nullptr,
        const size_t& aSize = 0
    ) const override;

    bool SetInputLayout(D3D11_INPUT_ELEMENT_DESC aLayout[], const size_t& aSize, const std::string& aVsData);

    ID3D11VertexShader* GetShader() const { return myShader.Get(); }
    VsType GetType() const { return myType; }

    const DxBuffer& GetInstanceBuffer() const { return myInstanceBuffer; }
private:
    VsType myType = VsType::Count;
    ComPtr<ID3D11InputLayout> myInputLayout = nullptr;
    ComPtr<ID3D11VertexShader> myShader = nullptr;
    DxBuffer myInstanceBuffer;
};
