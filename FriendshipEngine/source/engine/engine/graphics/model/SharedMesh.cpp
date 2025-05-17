#include "pch.h"
#include "SharedMesh.h"

#include <assets/AssetDatabase.h>
#include <assets/TextureDatabase.h>
#include <assets/ShaderDatabase.h>
#include <engine/shaders/VertexShader.h>
#include "../Texture.h"

void SharedMesh::SetTextures(const TextureCollection& aTextures)
{
	myTextures = std::move(aTextures);
	SetVertexTextureId(myTextures.vertexPaintIndex);
}

// TODO: REMOVE ALL SHADER INFO FROM SHARED MESH
void SharedMesh::SetVertexTextureId(const int aTextureId)
{
	// THIS IS AN EXTREMELY TEMPORARY HACK THAT SHOULD NOT EXIST
	// WE REALLY NEED TO REMOVE THE SHADER DEPENDENCIES FROM THE MESH ASAP
	if (myVertexShader != nullptr && myVertexShader->GetType() != VsType::SkinnedPBR)
	{
		if (aTextureId >= 0)
		{
			myVertexShader = ShaderDatabase::GetVertexShader(VsType::VertexPaintedPBRInstanced);
		}
		else // TODO: consider if previous scene included vertex painted meshes can influence result
		{
			myVertexShader = ShaderDatabase::GetVertexShader(VsType::DefaultPBRInstanced);
		}
	}

	myTextures.vertexPaintIndex = aTextureId;
}

void SharedMesh::SetMaterialName(std::string aMaterialName)
{
	myMaterialName = aMaterialName;
}

void SharedMesh::BindTextures() const
{
	if (myTextures.vertexPaintIndex >= 0)
	{
		const VertexTextureCollection& collection = AssetDatabase::GetTextureDatabase().GetVertexTextures(myTextures.vertexPaintIndex);
		collection.materials.vertex.texture->Bind((UINT)VertexShaderTextureSlot::VertexColor, false);

		int id = collection.materials.r.id;
		
		if (id >= 0)
		{
			auto& tex = AssetDatabase::GetTextures(id);
			tex.albedoTexture.texture->Bind(PixelShaderTextureSlot::VertexAlbedoR);
			tex.normalTexture.texture->Bind(PixelShaderTextureSlot::VertexNormalR);
			tex.materialTexture.texture->Bind(PixelShaderTextureSlot::VertexMaterialR);
			tex.emissiveTexture.texture->Bind(PixelShaderTextureSlot::VertexEmissiveR);
		}

		id = collection.materials.g.id;
		if (id >= 0)
		{
			auto& tex = AssetDatabase::GetTextures(id);
			tex.albedoTexture.texture->Bind(PixelShaderTextureSlot::VertexAlbedoG);
			tex.normalTexture.texture->Bind(PixelShaderTextureSlot::VertexNormalG);
			tex.materialTexture.texture->Bind(PixelShaderTextureSlot::VertexMaterialG);
			tex.emissiveTexture.texture->Bind(PixelShaderTextureSlot::VertexEmissiveG);
		}

		id = collection.materials.b.id;
		if (id >= 0)
		{
			auto& tex = AssetDatabase::GetTextures(id);
			tex.albedoTexture.texture->Bind(PixelShaderTextureSlot::VertexAlbedoB);
			tex.normalTexture.texture->Bind(PixelShaderTextureSlot::VertexNormalB);
			tex.materialTexture.texture->Bind(PixelShaderTextureSlot::VertexMaterialB);
			tex.emissiveTexture.texture->Bind(PixelShaderTextureSlot::VertexEmissiveB);
		}

		id = collection.materials.a.id;
		if (id >= 0)
		{
			auto& tex = AssetDatabase::GetTextures(id);
			tex.albedoTexture.texture->Bind(PixelShaderTextureSlot::VertexAlbedoA);
			tex.normalTexture.texture->Bind(PixelShaderTextureSlot::VertexNormalA);
			tex.materialTexture.texture->Bind(PixelShaderTextureSlot::VertexMaterialA);
			tex.emissiveTexture.texture->Bind(PixelShaderTextureSlot::VertexEmissiveA);
		}
		//myTextures.textureData.texture->Bind((UINT)VertexShaderTextureSlot::VertexColor, false);
		//myTextures.vertex.texture->Bind((UINT)VertexShaderTextureSlot::VertexColor, false);
	}

	if (myTextures.albedoTexture.texture == nullptr) { return; }

	myTextures.albedoTexture.texture->Bind((UINT)PixelShaderTextureSlot::Albedo);
	myTextures.normalTexture.texture->Bind((UINT)PixelShaderTextureSlot::Normal);
	myTextures.emissiveTexture.texture->Bind((UINT)PixelShaderTextureSlot::Emissive);
	myTextures.materialTexture.texture->Bind((UINT)PixelShaderTextureSlot::Material);
}
