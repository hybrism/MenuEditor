#pragma once

class SharedMesh;
using Microsoft::WRL::ComPtr;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();
	void Init();
	void Update();
	void Render();


private:
	ComPtr<ID3D11ShaderResourceView> myCubemap;
	SharedMesh* mySkyBoxMesh;
	unsigned int myNumMips = 0;
};