#pragma once
#include <d3d11_1.h>
#include <string>
#include <wrl/client.h>
#include "../graphics/RenderDefines.h"
#include <assets/ShaderTypeEnum.h>

using Microsoft::WRL::ComPtr;

class GraphicsEngine;

class Shader
{
public:
    Shader(GraphicsEngine* aGraphicsEngine);
    virtual ~Shader();
    virtual bool PrepareRender(
        const BufferSlots& aSlot = BufferSlots::Frame,
        void* aBufferData = nullptr,
        const size_t& aSize = 0
    ) const;
protected:
    bool InternalInit(
        const std::string& aName,
        const D3D11_BUFFER_DESC* aBufferDesc = nullptr,
        const D3D11_SUBRESOURCE_DATA* aSubresourceData = nullptr
    );

    ComPtr<ID3D11Buffer> myConstantBuffer = NULL; // TODO: Change to vector if more than one buffer is needed in any shader;
    GraphicsEngine* myGraphicsEngine;
};
