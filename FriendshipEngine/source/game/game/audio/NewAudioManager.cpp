#include "pch.h"
#include "NewAudioManager.h"

void NewAudioManager::Init()
{

  myFMOD = new FMODImplementation();


	std::string filePath = RELATIVE_AUDIO_ASSET_PATH;
	
	LoadSound(filePath + "Honk1.ogg" , eSounds::Honk, false, false, false);
	LoadSound(filePath + "Land.mp3" , eSounds::Land, false, false, false);
	LoadSound(filePath + "Jump.ogg" , eSounds::Jump, false, false, false);
	LoadSound(filePath + "Step.wav" , eSounds::Run, false, false, false);
	LoadSound(filePath + "slide.wav" , eSounds::Slide, false, false, false);
	LoadSound(filePath + "ArrowLand.wav" , eSounds::ArrowLand, true, false, false);
	LoadSound(filePath + "ArrowShoot.wav" , eSounds::ArrowShoot, true, false, false);
	LoadSound(filePath + "MenuMusic.mp3" , eSounds::MenuMusic, false, true, false);

	LoadSound(filePath + "GameMusic0.wav" , eSounds::GameMusic0, false, true, false);
	LoadSound(filePath + "GameMusic1.wav" , eSounds::GameMusic1, false, true, false);
	LoadSound(filePath + "GameMusic2.ogg" , eSounds::GameMusic2, false, true, false);
	LoadSound(filePath + "GameMusic3.wav" , eSounds::GameMusic3, false, true, false);

	LoadSound(filePath + "MenuNavigation.mp3" , eSounds::MenyNavigation, false, false, false);
	LoadSound(filePath + "WallRun.wav" , eSounds::WallRun, false, true, false);


	myFMOD->myNextChannelId = (int)eDedicatedChannels::Count;

	//PlayMusic(eSounds::GameMusic, 0.07f);
}

void NewAudioManager::Create()
{
		//assert(myInstance == nullptr && "AudioManager already created!");
		//myInstance = new NewAudioManager(); 
}

void NewAudioManager::Destroy()
{
}

void NewAudioManager::Update()
{
}

//void NewAudioManager::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
//{
//}
//
//void NewAudioManager::LoadEvent(const FMOD_GUID& aID)
//{
//	
//}

void NewAudioManager::LoadSound(const std::string& strSoundName, eSounds aSoundEnum, bool is3D, bool isLooping, bool isStream)
{
	
	auto loadSound = myFMOD->mySounds.find(strSoundName);
	if (loadSound != myFMOD->mySounds.end())
		return;

	mySound.insert(std::pair(aSoundEnum, strSoundName));

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= is3D ? FMOD_3D : FMOD_2D;
	eMode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* sound = nullptr;

	myFMOD->myCoreSystem->createSound(strSoundName.c_str(), eMode, nullptr, &sound);
	if (sound != nullptr)
		myFMOD->mySounds[strSoundName] = sound;
}

void NewAudioManager::UnLoadSound(const std::string& strSoundName)
{
	strSoundName;
}

void NewAudioManager::PlayLoadedSong()
{
	StopMusic();
	PlayMusic(myLoadedSound, myConstantMusicVolumeValue);
}

void NewAudioManager::LoadLevelSong(const std::string& strLevelName)
{
	if (strLevelName == "lvl_0.json")
	{
		myLoadedSound = eSounds::MenuMusic;
	}
	else if (strLevelName == "lvl_1.json")
	{
		myLoadedSound = eSounds::GameMusic1;
	}
	else if (strLevelName == "lvl_2.json")
	{
		myLoadedSound = eSounds::GameMusic2;
	}
	else if (strLevelName == "lvl_3.json")
	{
		myLoadedSound = eSounds::GameMusic3;
	}
}


int NewAudioManager::PlaySound(const eSounds aSoundEnum, float fVolumedB)
{
	int channelID = myFMOD->myNextChannelId++;
	auto tFoundIt = myFMOD->mySounds.find(mySound.at(aSoundEnum));
	if (tFoundIt == myFMOD->mySounds.end())
	{
		return 0;
	}
	//{
	//	LoadSound(strSoundName);
	//	tFoundIt = myFMOD->mySounds.find(strSoundName);
	//	if (tFoundIt == myFMOD->mySounds.end())
	//	{
	//		return channelID;
	//	}
	//}
	FMOD::Channel* channel = nullptr;
	myFMOD->myCoreSystem->playSound(tFoundIt->second, nullptr, true, &channel);
	if (channel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D)
		{
		//	FMOD_VECTOR soundPosition = Vector3ToFmod(vPosition);
			//ErrorCheck(channel->set3DAttributes(&soundPosition, nullptr));
		}
		//ErrorCheck(channel->setVolume(fVolumedB));
		//ErrorCheck(channel->setPaused(false));

		channel->setVolume(fVolumedB * mySfxMultiplier);
		channel->setPaused(false);
		myFMOD->myChannels[channelID] = channel;
	}
	return channelID;
}

int NewAudioManager::PlaySound(const eSounds aSoundEnum, float fVolumedB, float fPitch)
{
	int channelID = myFMOD->myNextChannelId++;
	auto tFoundIt = myFMOD->mySounds.find(mySound.at(aSoundEnum));
	if (tFoundIt == myFMOD->mySounds.end())
	{
		return 0;
	}
	
	FMOD::Channel* channel = nullptr;
	myFMOD->myCoreSystem->playSound(tFoundIt->second, nullptr, true, &channel);
	if (channel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D)
		{

		}


		channel->setVolume(fVolumedB * mySfxMultiplier);
		channel->setPaused(false);
		channel->setPitch(fPitch);
		myFMOD->myChannels[channelID] = channel;
	}
	return channelID;
}

int NewAudioManager::PlaySound(const eSounds aSoundEnum, float fVolumedB, Vector3f aPos)
{
	int channelID = myFMOD->myNextChannelId++;
	auto tFoundIt = myFMOD->mySounds.find(mySound.at(aSoundEnum));
	if (tFoundIt == myFMOD->mySounds.end())
	{
		return 0;
	}

	FMOD::Channel* channel = nullptr;
	myFMOD->myCoreSystem->playSound(tFoundIt->second, nullptr, true, &channel);

	if (channel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);

		if (currMode & FMOD_3D)
		{
			FMOD_VECTOR fVec;
			fVec.x = aPos.x;
			fVec.y = aPos.y;
			fVec.z = aPos.z;
			channel->set3DMinMaxDistance(1.f, 10000.f);
			channel->set3DAttributes(&fVec, nullptr);
		}

		channel->setVolume(fVolumedB * mySfxMultiplier);
		channel->setPaused(false);
		myFMOD->myChannels[channelID] = channel;
	}
	return channelID;
}

int NewAudioManager::PlayMusic(const eSounds aSoundEnum, float fVolumedB)
{
	myConstantMusicVolumeValue = fVolumedB;
	int channelID = (int)eDedicatedChannels::Music;
	auto tFoundIt = myFMOD->mySounds.find(mySound.at(aSoundEnum));
	if (tFoundIt == myFMOD->mySounds.end())
	{
		return 0;
	}

	FMOD::Channel* channel = nullptr;
	myFMOD->myCoreSystem->playSound(tFoundIt->second, nullptr, true, &channel);
	if (channel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D)
		{
			//	FMOD_VECTOR soundPosition = Vector3ToFmod(vPosition);
				//ErrorCheck(channel->set3DAttributes(&soundPosition, nullptr));
		}
		//ErrorCheck(channel->setVolume(fVolumedB));
		//ErrorCheck(channel->setPaused(false));

		channel->setVolume(fVolumedB * myMusicMultiplier);
		channel->setPaused(false);
		myFMOD->myChannels[channelID] = channel;
	}
	return channelID;
}

int NewAudioManager::PlayLoopingSound(const eDedicatedChannels aChannelEnum, const eSounds aSoundEnum, float fVolumedB, float aPitch)
{
	int channelID = (int)aChannelEnum;
	auto tFoundIt = myFMOD->mySounds.find(mySound.at(aSoundEnum));
	if (tFoundIt == myFMOD->mySounds.end())
	{
		return 0;
	}

	FMOD::Channel* channel = nullptr;
	myFMOD->myCoreSystem->playSound(tFoundIt->second, nullptr, true, &channel);
	if (channel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D)
		{
			//	FMOD_VECTOR soundPosition = Vector3ToFmod(vPosition);
				//ErrorCheck(channel->set3DAttributes(&soundPosition, nullptr));
		}
		//ErrorCheck(channel->setVolume(fVolumedB));
		//ErrorCheck(channel->setPaused(false));

		channel->setVolume(fVolumedB * mySfxMultiplier);
		channel->setPaused(false);
		channel->setPitch(aPitch);

		myFMOD->myChannels[channelID] = channel;
	}
	return channelID;
}


void NewAudioManager::StopLoopingSound(const eDedicatedChannels aChannelEnum)
{
	int channelID = (int)aChannelEnum;
	myFMOD->myChannels[channelID]->setPaused(true);
}

void NewAudioManager::StopMusic()
{
	int channelID = (int)eDedicatedChannels::Music;
	myFMOD->myChannels[channelID]->stop();
}

void NewAudioManager::StopSound(int channelID)
{
	FMOD::Channel* channel = nullptr;
	myFMOD->myCoreSystem->getChannel(channelID, &channel);
	channel->setPaused(true);
}

void NewAudioManager::SetPlayerListenPosition(const Transform& aPos)
{
	FMOD_VECTOR position = { 0,0,0 };
	position.x = aPos.GetPosition().x;
	position.y = aPos.GetPosition().y;
	position.z = aPos.GetPosition().z;

	FMOD_VECTOR forward = Vector3ToFmod(aPos.GetForward() * -1.f);
	FMOD_VECTOR up = Vector3ToFmod(aPos.GetUp() * -1.f);
	FMOD_VECTOR velocity = { 0,0,-1 };
	myFMOD->myCoreSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up);
}

void NewAudioManager::SetSfxVolume(const float aVolume)
{
	mySfxMultiplier = aVolume;
}

void NewAudioManager::SetMusicVolume(const float aVolume)
{
	myMusicMultiplier = aVolume;
	myFMOD->myChannels[(int)eDedicatedChannels::Music]->setVolume(myConstantMusicVolumeValue * myMusicMultiplier);
}

FMOD_VECTOR NewAudioManager::Vector3ToFmod(const Vector3f& aPosition)
{
	FMOD_VECTOR fVec;
	fVec.x = aPosition.x;
	fVec.y = aPosition.y;
	fVec.z = aPosition.z;
	return fVec;
}


//void NewAudioManager::SetChannelVolume(int aChannelID, float aVolume)
//{
//}
//
//bool NewAudioManager::IsChannelPlaying(int nChannelId) const
//{
//    return false;
//}
//
//void NewAudioManager::SetPlayerListenPosition(const Transform& aPos)
//{
//}
//
//FMOD_VECTOR NewAudioManager::Vector3ToFmod(const Vector3f& aPosition)
//{
//    return FMOD_VECTOR();
//}
//
//Vector3f NewAudioManager::FmodToVector3(const FMOD_VECTOR& aPosition)
//{
//    return Vector3f();
//}
