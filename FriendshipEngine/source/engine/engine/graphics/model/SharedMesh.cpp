#include "pch.h"
#include "SharedMesh.h"

#include "../../../assets/AssetDatabase.h"
#include "../Texture.h"

void SharedMesh::SetTextures(const TextureCollection& aTextures)
{
	myTextures = aTextures;
}

void SharedMesh::SetMaterialName(std::string aMaterialPath)
{
	myMaterialName = aMaterialPath;
}

void SharedMesh::BindTextures() const
{
	if (myTextures.albedoTexture.texture == nullptr) { return; }

	myTextures.albedoTexture.texture->Bind((UINT)ShaderTextureSlot::Albedo);
	myTextures.normalTexture.texture->Bind((UINT)ShaderTextureSlot::Normal);
	myTextures.emissiveTexture.texture->Bind((UINT)ShaderTextureSlot::Emissive);
	myTextures.materialTexture.texture->Bind((UINT)ShaderTextureSlot::Material);
}
