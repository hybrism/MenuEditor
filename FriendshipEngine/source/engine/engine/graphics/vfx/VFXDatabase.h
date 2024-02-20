#pragma once
#include <assets/ShaderTypeEnum.h>
#include "VisualEffect.h"
#include <vector>

class VFXDatabase
{
public:
	inline static VFXDatabase* GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new VFXDatabase();
		}
		return myInstance;
	}

	inline static void DestroyInstance()
	{
		if (myInstance != nullptr)
		{
			delete myInstance;
			myInstance = nullptr;
		}
	}

	vfxid_t CreateEffect(VisualEffect aEffect)
	{
		myEffects.push_back(aEffect);
		return myEffects.size() - 1;
	}

	const VisualEffect& GetEffect(const vfxid_t& aEffectId) const
	{
		return myEffects[aEffectId];
	}

private:
	std::vector<VisualEffect> myEffects;

	static VFXDatabase* myInstance;
};
