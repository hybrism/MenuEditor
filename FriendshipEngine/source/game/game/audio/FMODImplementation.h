#pragma once
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_studio.hpp>
#include <FMOD/fmod_errors.h>

#define MAX_AMOUNT_CHANNELS 512


class FMODImplementation
{
public:
    FMODImplementation();
    ~FMODImplementation();

    void Update();
    
    FMOD::Studio::System* myStudioSystem;
    FMOD::System* myCoreSystem;

    int myNextChannelId = 0;

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

    BankMap myBanks;
    EventMap myEvents;
    SoundMap mySounds;
    ChannelMap myChannels;



private:

    //AudioManager&
};