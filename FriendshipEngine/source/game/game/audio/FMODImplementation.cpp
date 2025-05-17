#include "pch.h"
#include "FMODImplementation.h"
#include "AudioManager.h"

FMODImplementation::FMODImplementation()
{
	// TODO TA BORT CREATE HÄR UWU
	myStudioSystem = nullptr;
	AudioManager::GetInstance()->Create();
	AudioManager::GetInstance()->ErrorCheck(FMOD::Studio::System::create(&myStudioSystem));
	AudioManager::GetInstance()->ErrorCheck(myStudioSystem->initialize(MAX_AMOUNT_CHANNELS, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr));
	
	myCoreSystem = nullptr;
	AudioManager::GetInstance()->ErrorCheck(myStudioSystem->getCoreSystem(&myCoreSystem));
}

FMODImplementation::~FMODImplementation()
{
	AudioManager::GetInstance()->ErrorCheck(myStudioSystem->unloadAll());
	AudioManager::GetInstance()->ErrorCheck(myStudioSystem->release());
}

void FMODImplementation::Update()
{
	std::vector<ChannelMap::iterator> channelsToDelete;
	for (auto currentChannel = myChannels.begin(), lastChannel = myChannels.end() ;currentChannel != lastChannel; currentChannel++)
	{
		bool isPlaying = false;
		currentChannel->second->isPlaying(&isPlaying);
		if (!isPlaying)
			channelsToDelete.push_back(currentChannel);
	}
	
	for (FMODImplementation::ChannelMap::iterator& it : channelsToDelete)
	{
		channelsToDelete.erase(channelsToDelete.begin());
		it;
	}
	myStudioSystem->update();
	//AudioManager::ErrorCheck(myStudioSystem->update());
}
