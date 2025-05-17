#pragma once
#include <any>
#include <map>
#include <vector>

#ifdef SendMessage
#undef SendMessage
#endif

namespace FE
{
	class Observer;
	enum class eMessageType
	{
		MeshDropped,
		DdsDropped,
		TtfDropped,
		PrintToConsole,
		PushEntityToInspector,
		NewLevelLoaded,
		NewMenuLoaded,
		UpdateMenuEditorColliders,
		Count
	};

	struct Message
	{
		eMessageType myEventType;
		std::any myMessage;
	};

	class PostMaster
	{
	public:
		PostMaster(PostMaster& aOther) = delete;
		void operator=(const PostMaster& aOther) = delete;
		~PostMaster();

		static PostMaster* GetInstance();

		void Subscribe(Observer* aObserver, const eMessageType aEventType);
		void SendMessage(const Message& aMessage);

	private:
		PostMaster() = default;

		std::map<eMessageType, std::vector<Observer*>> myObservers;

		static PostMaster* myInstance;
	};
};