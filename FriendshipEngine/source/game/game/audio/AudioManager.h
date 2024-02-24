#pragma once

#include "FMODImplementation.h"
#include <engine/math/Transform.h>



class AudioManager
{
public:



	static int AudioManager::ErrorCheck(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
		{
			Print("FMOD ERROR");
			//std::cout << "FMOD ERROR " << result << endl;
			return 0;
		}
		 //cout << "FMOD all good" << endl;
		return 1;
	}

	static AudioManager* GetInstance() 
	{ 
		return myInstance;
	};

	void Init();
	void Create();
	void Destroy();

	void Update();

	void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEvent(const std::string& strEventName);
	void LoadSound(const std::string& strSoundName, bool is3D = true, bool isLooping = false, bool isStream = false);
	void UnLoadSound(const std::string& strSoundName);

	void Set3dListenerAndOrientation(const Vector3f aPos = Vector3f{ 0,0,0 }, float aVolume = 0);
	int PlaySound(const std::string& strSoundName, const Vector3f& vPosition, float fVolumedB);
	void PlayEvent(const std::string& strEventName);
	void StopEvent(const std::string& strEventName, bool stopImmediate);
	void StopChannel(int channelID);
	void GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* parameter);
	void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
	void StopAllChannels();
	void SetChannel3dPosition(int aChannelID, const Vector3f& aPosition);
	void SetChannelVolume(int aChannelID, float aVolume);
	bool IsChannelPlaying(int nChannelId) const;
	bool IsEventPlaying(const std::string& strEventName) const;

	void SetPlayerListenPosition(const Transform& aPos);
	FMOD_VECTOR Vector3ToFmod(const Vector3f& aPosition);
	Vector3f FmodToVector3(const FMOD_VECTOR& aPosition);

private:


private:

	//FMOD_3D_ATTRIBUTES* myPlayerFmodPosition;

	FMODImplementation* myFMOD;
	AudioManager();
	~AudioManager();
	static AudioManager* myInstance;


};