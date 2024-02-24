#include "pch.h"
#include "AudioManager.h"
AudioManager* AudioManager::myInstance = nullptr;


void AudioManager::Create()
{
	assert(myInstance == nullptr && "AudioManager already created!");
	myInstance = new AudioManager();
	//myPlayerFmodPosition = new FMOD_3D_ATTRIBUTES();
	//myPlayerFmodPosition->position = { 0,0,0 };
	//myPlayerFmodPosition->forward = { 1,0,0 };
	//myPlayerFmodPosition->up = { 0,1,0 };l
	//myPlayerFmodPosition->velocity = { 0,0,0 };
}

void AudioManager::Init()
{
	myFMOD = new FMODImplementation();
	std::string masterFilePath = RELATIVE_FMOD_ASSET_PATH;
	masterFilePath += "Master.bank";	
	std::string musicFilePath = RELATIVE_FMOD_ASSET_PATH;
	musicFilePath += "Music.bank";
	//LoadBank("D:/Spelprojekt/SP7/FriendshipEngine/content/audio/Desktop/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
	LoadBank(masterFilePath, FMOD_STUDIO_LOAD_BANK_NORMAL);
	LoadBank(musicFilePath, FMOD_STUDIO_LOAD_BANK_NORMAL);

	std::string jumpFilePath = RELATIVE_AUDIO_ASSET_PATH;
	jumpFilePath += "Jump.wav";

	LoadSound(jumpFilePath, false, false, false);
	//PlaySound(freeBirdFilePath, {0,50,0}, 1.f);
}

void AudioManager::Destroy()
{
	assert(myInstance != nullptr && "No AudioManager exist!");
	delete myInstance;
	myInstance = nullptr;
}

AudioManager::AudioManager()
{
	myFMOD = nullptr;

}

AudioManager::~AudioManager()
{
	delete myFMOD;
}

void AudioManager::Update()
{
	myFMOD->Update();
}

void AudioManager::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
	auto tFoundIt = myFMOD->myBanks.find(strBankName);
	if (tFoundIt != myFMOD->myBanks.end())
		return;
	FMOD::Studio::Bank* bank;
	ErrorCheck(myFMOD->myStudioSystem->loadBankFile(strBankName.c_str(), flags, &bank));
	if (bank)
	{
		myFMOD->myBanks[strBankName] = bank;
	}
}

void AudioManager::LoadEvent(const std::string& strEventName)
{
	auto tFoundit = myFMOD->myEvents.find(strEventName);
	if (tFoundit != myFMOD->myEvents.end())
		return;
	FMOD::Studio::EventDescription* eventDescription = NULL;
	ErrorCheck(myFMOD->myStudioSystem->getEvent(strEventName.c_str(), &eventDescription));
	if (eventDescription)
	{
		FMOD::Studio::EventInstance* pEventInstance = NULL;
		ErrorCheck(eventDescription->createInstance(&pEventInstance));
		if (pEventInstance)
		{
			myFMOD->myEvents[strEventName] = pEventInstance;
		}
	}
}

void AudioManager::LoadSound(const std::string& strSoundName, bool is3D, bool isLooping, bool isStream)
{
	auto loadSound = myFMOD->mySounds.find(strSoundName);
	if (loadSound != myFMOD->mySounds.end())
		return;

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= is3D ? FMOD_3D : FMOD_2D;
	eMode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* sound = nullptr;

	ErrorCheck(myFMOD->myCoreSystem->createSound(strSoundName.c_str(), eMode, nullptr, &sound));
	if (sound != nullptr)
		myFMOD->mySounds[strSoundName] = sound;

}

void AudioManager::UnLoadSound(const std::string& strSoundName)
{
	auto sound = myFMOD->mySounds.find(strSoundName);
	if (sound == myFMOD->mySounds.end())
		return;

	ErrorCheck(sound->second->release());
	myFMOD->mySounds.erase(sound);

}

void AudioManager::Set3dListenerAndOrientation(const Vector3f aPos, float aVolume)
{
	aPos;
	aVolume;
	auto tFoundIt = myFMOD->myChannels.find(myFMOD->myNextChannelId);
	if (tFoundIt == myFMOD->myChannels.end())
		return;

	FMOD_VECTOR position = Vector3ToFmod(aPos);
	ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

int AudioManager::PlaySound(const std::string& strSoundName, const Vector3f& aPosition, float fVolumedB)
{
	int channelID = myFMOD->myNextChannelId++;
	auto tFoundIt = myFMOD->mySounds.find(strSoundName);
	if (tFoundIt == myFMOD->mySounds.end())
	{
		LoadSound(strSoundName);
		tFoundIt = myFMOD->mySounds.find(strSoundName);
		if (tFoundIt == myFMOD->mySounds.end())
		{
			return channelID;
		}
	}
	FMOD::Channel* channel = nullptr;
	ErrorCheck(myFMOD->myCoreSystem->playSound(tFoundIt->second, nullptr, true, &channel));
	if (channel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D)
		{
			FMOD_VECTOR soundPosition = Vector3ToFmod(aPosition);
			ErrorCheck(channel->set3DAttributes(&soundPosition, nullptr));
		}
		ErrorCheck(channel->setVolume(fVolumedB));
		ErrorCheck(channel->setPaused(false));
		myFMOD->myChannels[channelID] = channel;
	}
	return channelID;
}

void AudioManager::PlayEvent(const std::string& strEventName)
{
	auto tFoundit = myFMOD->myEvents.find(strEventName);
	if (tFoundit == myFMOD->myEvents.end())
	{
		LoadEvent(strEventName);
		tFoundit = myFMOD->myEvents.find(strEventName);
		if (tFoundit == myFMOD->myEvents.end())
			return;
	}
	tFoundit->second->start();
}

void AudioManager::StopEvent(const std::string& strEventName, bool stopImmediate)
{
	auto tFoundIt = myFMOD->myEvents.find(strEventName);
	if (tFoundIt == myFMOD->myEvents.end())
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = stopImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
	ErrorCheck(tFoundIt->second->stop(eMode));
}

void AudioManager::StopChannel(int aChannelID)
{
	auto tFoundIt = myFMOD->myChannels.find(aChannelID);
	if (tFoundIt == myFMOD->myChannels.end())
		return;

	ErrorCheck(tFoundIt->second->stop());
}

void AudioManager::GetEventParameter(const std::string& /*strEventName*/, const std::string& /*strParameterName*/, float* /*parameter*/)
{
	//auto tFoundIt = myFMOD->myEvents.find(strEventName);
	//if (tFoundIt == myFMOD->myEvents.end())
	//	return;

	//FMOD::Studio::EventInstance* pParameter = NULL;
	//AudioManager::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), &pParameter));
	//AudioManager::ErrorCheck(pParameter->getParameterValue(parameter));
}
void AudioManager::SetEventParameter(const std::string& /*strEventName*/, const std::string& /*strParameterName*/, float /*fValue*/)
{
	//auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	//if (tFoundIt == sgpImplementation->mEvents.end())
	//	return;

	//FMOD::Studio::ParameterInstance* pParameter = NULL;
	//CAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
	//CAudioEngine::ErrorCheck(pParameter->setValue(fValue));
}

void AudioManager::StopAllChannels()
{
	for (auto currentChannel = myFMOD->myChannels.begin(), lastChannel = myFMOD->myChannels.end(); currentChannel != lastChannel; currentChannel++)
	{
		bool isPlaying = false;
		currentChannel->second->isPlaying(&isPlaying);
		if (isPlaying)
			ErrorCheck(currentChannel->second->stop());
	}
}




void AudioManager::SetChannel3dPosition(int aChannelID, const Vector3f& aPosition)
{
	auto tFoundIt = myFMOD->myChannels.find(aChannelID);
	if (tFoundIt == myFMOD->myChannels.end())
		return;

	FMOD_VECTOR position = Vector3ToFmod(aPosition);
	ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void AudioManager::SetChannelVolume(int aChannelID, float aVolume)
{
	auto tFoundIt = myFMOD->myChannels.find(aChannelID);
	if (tFoundIt == myFMOD->myChannels.end())
		return;
	ErrorCheck(tFoundIt->second->setVolume(aVolume));
}

bool AudioManager::IsChannelPlaying(int aChannelID) const
{
	auto tFoundIt = myFMOD->myChannels.find(aChannelID);
	if (tFoundIt == myFMOD->myChannels.end())
		return false;
	bool isPlaying = false;
	ErrorCheck(tFoundIt->second->isPlaying(&isPlaying));
	return isPlaying;
}

bool AudioManager::IsEventPlaying(const std::string& strEventName) const
{
	auto tFoundIt = myFMOD->myEvents.find(strEventName);
	if (tFoundIt == myFMOD->myEvents.end())
		return false;

	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		return true;
	}
	return false;
}

void AudioManager::SetPlayerListenPosition(const Transform& aPos)
{
	
	FMOD_VECTOR position = { 0,0,0 };
	position = Vector3ToFmod(aPos.GetPosition());
	FMOD_VECTOR forward = Vector3ToFmod(aPos.GetForward() * -1.f);
	FMOD_VECTOR up = Vector3ToFmod(aPos.GetUp() * -1.f);
	FMOD_VECTOR velocity = {0,0,-1 };
	myFMOD->myCoreSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up);
}

FMOD_VECTOR AudioManager::Vector3ToFmod(const Vector3f& aPosition)
{
	FMOD_VECTOR fVec;
	fVec.x = aPosition.x;
	fVec.y = aPosition.y;
	fVec.z = aPosition.z;
	return fVec;
}

Vector3f AudioManager::FmodToVector3(const FMOD_VECTOR& fVec)
{
	Vector3f vector3f;
	vector3f.x = fVec.x;
	vector3f.y = fVec.y;
	vector3f.z = fVec.z;
	return vector3f;
}
