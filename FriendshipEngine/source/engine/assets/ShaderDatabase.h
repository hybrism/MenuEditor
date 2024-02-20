#pragma once
#include "ShaderTypeEnum.h"

class PixelShader;
class VertexShader;

class ShaderDatabase
{
public:
	inline static void CreateInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new ShaderDatabase();
			myInstance->LoadShaders();
		}
	}

	inline static void DestroyInstance()
	{
		if (myInstance != nullptr)
		{
			delete myInstance;
			myInstance = nullptr;
		}
	}

	static const PixelShader* GetPixelShader(const PsType& aType);
	static const VertexShader* GetVertexShader(const VsType& aType);
private:
	ShaderDatabase();
	~ShaderDatabase();

	void LoadShaders();
	void LoadPBRShaders();
	void LoadFullscreenShaders();
	void LoadSpriteShaders();
	void LoadCommonShaders();
	void LoadVFXShaders();

	PixelShader* myPixelShaders;
	VertexShader* myVertexShaders;
	static ShaderDatabase* myInstance;
};
