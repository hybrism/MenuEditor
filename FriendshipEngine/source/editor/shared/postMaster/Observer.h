#pragma once

namespace FE
{
	struct Message;
	class Observer
	{
	public:
		virtual void RecieveMessage(const Message& aMessage) = 0;
	};
}