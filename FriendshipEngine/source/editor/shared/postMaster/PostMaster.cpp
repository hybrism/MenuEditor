#include "PostMaster.h"
#include "Observer.h"

FE::PostMaster* FE::PostMaster::myInstance = nullptr;

FE::PostMaster* FE::PostMaster::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new PostMaster();
	}

	return myInstance;
}


void FE::PostMaster::Subscribe(Observer* aObserver, const eMessageType aEventType)
{
	myObservers[aEventType].push_back(aObserver);
}

void FE::PostMaster::SendMessage(const Message& aMessage)
{
	for (int i = 0; i < myObservers[aMessage.myEventType].size(); i++)
	{
		myObservers[aMessage.myEventType][i]->RecieveMessage(aMessage);
	}
}