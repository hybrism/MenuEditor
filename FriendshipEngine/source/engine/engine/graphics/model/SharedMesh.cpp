#include "pch.h"
#include "SharedMesh.h"

#include <assets/AssetDatabase.h>
#include <assets/TextureDatabase.h>
#include <assets/ShaderDatabase.h>
#include "../Texture.h"

void SharedMesh::SetTextures(const TextureCollection& aTextures)
{
	myTextures = aTextures;
	SetVertexTextureId(myTextures.vertexPaintIndex);
}

void SharedMesh::SetVertexTextureId(const int aTextureId)
{
	if (aTextureId >= 0)
	{
		myVertexShader = ShaderDatabase::GetVertexShader(VsType::VertexPaintedPBRInstanced);
	}
	else // TODO: consider if previous scene included vertex painted meshes can influence result
	{
		myVertexShader = ShaderDatabase::GetVertexShader(VsType::DefaultPBRInstanced);
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
		}

		id = collection.materials.g.id;
		if (id >= 0)
		{
			auto& tex = AssetDatabase::GetTextures(id);
			tex.albedoTexture.texture->Bind(PixelShaderTextureSlot::VertexAlbedoG);
		}

		id = collection.materials.b.id;
		if (id >= 0)
		{
			auto& tex = AssetDatabase::GetTextures(id);
			tex.albedoTexture.texture->Bind(PixelShaderTextureSlot::VertexAlbedoB);
		}

		id = collection.materials.a.id;
		if (id >= 0)
		{
			auto& tex = AssetDatabase::GetTextures(id);
			tex.albedoTexture.texture->Bind(PixelShaderTextureSlot::VertexAlbedoA);
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
