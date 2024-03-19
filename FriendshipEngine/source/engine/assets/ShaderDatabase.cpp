#include "pch.h"

#include "ShaderDatabase.h"

#include <engine/graphics/RenderDefines.h>
#include <engine/graphics/GraphicsData.h>
#include <engine/graphics/animation/Pose.h>
#include <engine/shaders/PixelShader.h>
#include <engine/shaders/VertexShader.h>

ShaderDatabase* ShaderDatabase::myInstance = nullptr;

const VertexShader* ShaderDatabase::GetVertexShader(const VsType& aType) { return &myInstance->myVertexShaders[static_cast<size_t>(aType)]; }
const PixelShader* ShaderDatabase::GetPixelShader(const PsType& aType) { return &myInstance->myPixelShaders[static_cast<size_t>(aType)]; }

ShaderDatabase::ShaderDatabase()
{
	myPixelShaders = new PixelShader[static_cast<size_t>(PsType::Count)];
	myVertexShaders = new VertexShader[static_cast<size_t>(VsType::Count)];
}

ShaderDatabase::~ShaderDatabase()
{
	delete[] myPixelShaders;
	delete[] myVertexShaders;
}

void ShaderDatabase::LoadShaders()
{
	LoadPBRShaders();
	LoadFullscreenShaders();
	LoadSpriteShaders();
	LoadCommonShaders();
	LoadVFXShaders();
}

void ShaderDatabase::LoadPBRShaders()
{
#pragma region VS

	VsType type = VsType::Count;
	// Default PBR Vertex Shader
	{
		type = VsType::DefaultPBR;
		auto& current = myVertexShaders[static_cast<size_t>(type)];
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		};

		current.Init("pbr_default", type, layout, sizeof(layout));
	}

	// Animated PBR Vertex Shader
	{
		type = VsType::SkinnedPBR;
		auto& current = myVertexShaders[static_cast<size_t>(type)];

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BONES",		0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "WEIGHTS",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		};


		D3D11_BUFFER_DESC matrixVertexBufferDesc;
		matrixVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixVertexBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX) * MAX_ANIMATION_BONES;
		matrixVertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixVertexBufferDesc.MiscFlags = 0;
		matrixVertexBufferDesc.StructureByteStride = 0;

		current.Init("pbr_skinned", type, layout, sizeof(layout), nullptr, &matrixVertexBufferDesc, NULL);
	}


	// Default PBR Vertex Instanced Shader
	{
		type = VsType::DefaultPBRInstanced;
		auto& current = myVertexShaders[static_cast<size_t>(type)];
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	0,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	16,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	32,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	48,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "ENTITY",		0, DXGI_FORMAT_R32G32_UINT,			1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		};

		current.Init("pbr_default_instanced", type, layout, sizeof(layout));
	}

	// Default PBR Vertex Instanced Shader
	{
		type = VsType::VertexPaintedPBRInstanced;
		auto& current = myVertexShaders[static_cast<size_t>(type)];
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	0,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	16,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	32,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	48,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "ENTITY",		0, DXGI_FORMAT_R32G32_UINT,			1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		};

		current.Init("pbr_vertex_painted_instanced", type, layout, sizeof(layout));
	}
#pragma endregion //!VS
#pragma region PS
	myPixelShaders[static_cast<size_t>(PsType::DefaultPBR)].Init("pbr_default", PsType::DefaultPBR);
	myPixelShaders[static_cast<size_t>(PsType::SilhouettePBR)].Init("pbr_silhouette", PsType::SilhouettePBR);
	myPixelShaders[static_cast<size_t>(PsType::DeferredPBR)].Init("pbr_deferred", PsType::DeferredPBR);
	myPixelShaders[static_cast<size_t>(PsType::GBuffer)].Init("pbr_g_buffer", PsType::GBuffer);
	myPixelShaders[static_cast<size_t>(PsType::LightBounds)].Init("pbr_bounding_lights", PsType::LightBounds);
	myPixelShaders[static_cast<size_t>(PsType::DirectionalLight)].Init("pbr_directional_light", PsType::DirectionalLight);
	myPixelShaders[static_cast<size_t>(PsType::DebugLine)].Init("debug_line", PsType::DebugLine);
	myPixelShaders[static_cast<size_t>(PsType::DebugLightSphere)].Init("debug_point_ligth_sphere", PsType::DebugLightSphere);
	myPixelShaders[static_cast<size_t>(PsType::SkyBox)].Init("skybox", PsType::SkyBox);
#pragma endregion //!PS
}

void ShaderDatabase::LoadFullscreenShaders()
{
#pragma region VS
	VsType type = VsType::Count;
	// fullscreen
	{
		type = VsType::Fullscreen;
		auto& current = myVertexShaders[static_cast<size_t>(type)];

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "SV_VertexID", 0, DXGI_FORMAT_R8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		current.Init("fullscreen", type, layout, sizeof(layout));
	}
#pragma endregion //!VS
#pragma region PS
	// fullscreen
	{
		myPixelShaders[static_cast<size_t>(PsType::Fullscreen)].Init("fullscreen", PsType::Fullscreen);
	}
	// Post Process
	{
		{
			D3D11_BUFFER_DESC bufferDescription = { 0 };
			bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDescription.ByteWidth = sizeof(PostProcessBufferData);

			myPixelShaders[static_cast<size_t>(PsType::PostProcess)].Init("post_process", PsType::PostProcess, &bufferDescription);
		}

		myPixelShaders[static_cast<size_t>(PsType::Vignette)].Init("post_process_vignette", PsType::Vignette);
		myPixelShaders[static_cast<size_t>(PsType::ToneMap)].Init("post_process_tone_map", PsType::ToneMap);
	}
#pragma endregion //!PS
}

void ShaderDatabase::LoadSpriteShaders()
{
#pragma region VS
	VsType type = VsType::Count;
	// instanced_sprite_VS
	{
		type = VsType::InstancedSprite;
		auto& current = myVertexShaders[static_cast<size_t>(type)];

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 }, //POSITION
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 }, //VERTEXINDEX
		{ "TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //TRANSFORM 1-4
		{ "TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "TEXCOORD",	4, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //!TRANSFORM
		{ "TEXCOORD",	5, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //COLOR
		{ "TEXCOORD",	6, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //UV
		{ "TEXCOORD",	7, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //UVRECT
		{ "TEXCOORD",	8, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		};

		current.Init("instanced_sprite", type, layout, sizeof(layout));
	}

	// instanced_3D_sprite_VS
	{
		type = VsType::Instanced3DSprite;
		auto& current = myVertexShaders[static_cast<size_t>(type)];

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 }, //POSITION
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 }, //VERTEXINDEX
		{ "TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //TRANSFORM 1-4
		{ "TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "TEXCOORD",	4, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //!TRANSFORM
		{ "TEXCOORD",	5, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //COLOR
		{ "TEXCOORD",	6, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //UV
		{ "TEXCOORD",	7, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 }, //UVRECT
		{ "TEXCOORD",	8, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		};

		current.Init("instanced_3D_sprite", type, layout, sizeof(layout));
	}
#pragma endregion //!VS
#pragma region PS
	myPixelShaders[static_cast<size_t>(PsType::Sprite)].Init("sprite", PsType::Sprite);
#pragma endregion //!PS
}

void ShaderDatabase::LoadCommonShaders()
{
#pragma region VS

#pragma endregion //!VS
#pragma region PS
	myPixelShaders[static_cast<size_t>(PsType::MissingTextureLegacy)].Init("missing_texture", PsType::MissingTextureLegacy);
	myPixelShaders[static_cast<size_t>(PsType::MissingTextureDeferred)].Init("missing_texture_deferred", PsType::MissingTextureDeferred);
#pragma endregion //!PS
}

void ShaderDatabase::LoadVFXShaders()
{
#pragma region VS
	VsType type = VsType::Count;
	// Default PBR Vertex Shader
	{
		//type = VsType::DefaultVFX;
		//auto& current = myVertexShaders[static_cast<size_t>(type)];
		//D3D11_INPUT_ELEMENT_DESC layout[] =
		//{
		//{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	0,								D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "TEXCOORD",	1, DXGI_FORMAT_R32_FLOAT,			0,	8,								D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//};

		//current.Init("vfx_default", type, layout, sizeof(layout));


		type = VsType::DefaultVFX;
		//auto& current = myVertexShaders[static_cast<size_t>(type)];
		//D3D11_INPUT_ELEMENT_DESC layout[] =
		//{
		//{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	0,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	16,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	32,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	48,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//{ "ENTITY",		0, DXGI_FORMAT_R32G32_UINT,			1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//};
		//{
		//{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		//{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	0,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	16,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	32,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	48,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		//};

		auto& current = myVertexShaders[static_cast<size_t>(type)];
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	0,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	16,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	32,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1,	48,								D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		{ "TIME",		0, DXGI_FORMAT_R32_FLOAT,			1,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		};

		current.Init("vfx_default", type, layout, sizeof(layout));
	}
#pragma endregion //!VS
#pragma region PS
	myPixelShaders[static_cast<size_t>(PsType::TestVFX)].Init("test_vfx", PsType::TestVFX);
#pragma endregion //!PS
}
