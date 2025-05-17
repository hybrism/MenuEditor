#pragma once

#include "FMODImplementation.h"
#include <engine/math/Transform.h>
//#include <fmod/fmod_studio_guids.hpp>
#include <string>
#include <engine\utility\Error.h>

enum class eDedicatedChannels
{
	Music,
	WallRun,
	Honk,
	Count,
};
enum class eSounds
{
	Test,
Jump,
Run,
Slide,
ArrowLand,
ArrowShoot,
MenuMusic,
GameMusic0,
GameMusic1,
GameMusic2,
GameMusic3,
MenyNavigation,
WallRun,
Honk,
Land
};

class NewAudioManager
{
public:
	//static int ErrorCheck(FMOD_RESULT result)
	//{
	//	if (result != FMOD_OK)
	//	{
	//		Print("FMOD ERROR");
	//		//std::cout << "FMOD ERROR " << result << endl;
	//		return 0;
	//	}
	//	//cout << "FMOD all good" << endl;
	//	return 1;
	//}
	static NewAudioManager* GetInstance()
	{
		static NewAudioManager* myInstance;

		if (myInstance == nullptr)
		{
			myInstance = new NewAudioManager();
		}

		return myInstance;
	};

	void Init();
	void Create();
	void Destroy();

	void Update();

	//	void LoadBank(const std::string & strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
		//void LoadEvent(const FMOD_GUID& aID);
	void LoadSound(const std::string& strSoundName, eSounds aSoundEnum, bool is3D = true, bool isLooping = false, bool isStream = false);
	void UnLoadSound(const std::string& strSoundName);

	void PlayLoadedSong();
	void LoadLevelSong(const std::string& strLevelName);

	int PlaySound(const eSounds aSoundEnum, float fVolumedB);
	int PlaySound(const eSounds aSoundEnum, float fVolumedB, float fPitch);
	int PlaySound(const eSounds aSoundEnum, float fVolumedB, Vector3f aPos);
	int PlayMusic(const eSounds aSoundEnum, float fVolumedB);
	int PlayLoopingSound(const eDedicatedChannels aChannelEnum, const eSounds aSoundEnum, float fVolumedB, float aPitch = 1.f);
	void StopLoopingSound(const eDedicatedChannels aChannelEnum);

	void StopMusic();

	void StopSound(int channelID);
	void SetPlayerListenPosition(const Transform& aPos);

	//0 - 1
	void SetSfxVolume(const float aVolume);
	void SetMusicVolume(const float aVolume);

	float GetSfxVolume() const { return mySfxMultiplier; }
	float GetMusicVolume() const { return myMusicMultiplier; }

	FMOD_VECTOR Vector3ToFmod(const Vector3f& aPosition);
	//void SetChannelVolume(int aChannelID, float aVolume);
	//bool IsChannelPlaying(int nChannelId) const;

	//void SetPlayerListenPosition(const Transform& aPos);
	//FMOD_VECTOR Vector3ToFmod(const Vector3f& aPosition);
	//Vector3f FmodToVector3(const FMOD_VECTOR& aPosition);

	~NewAudioManager() {}

private:
	NewAudioManager() {}

	//FMOD_3D_ATTRIBUTES* myPlayerFmodPosition;

	FMODImplementation* myFMOD = nullptr;

	float mySfxMultiplier = 1.0f;
	float myMusicMultiplier = 1.0f;


	float myConstantMusicVolumeValue = 0.3f;
	std::map<eSounds, std::string> mySound;
	eSounds myLoadedSound = eSounds::ArrowShoot;
};