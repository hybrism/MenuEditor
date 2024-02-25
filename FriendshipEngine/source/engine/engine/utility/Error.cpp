#include "pch.h"
#include "Error.h"

#include "../Defines.h"

#if USE_FE_CONSOLE
#include "../editor/shared/postMaster/PostMaster.h"
#endif

#include <iostream>
#include <Windows.h>

namespace Error
{
	void PrintIntro()
	{
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console, 13);
		//std::cout << R"(######################################################################################)" << std::endl;
		//std::cout << R"(#                                                                                    #)" << std::endl;
		//std::cout << R"(#                            ,.--------._                                            #)" << std::endl;
		//std::cout << R"(#                           /            ''.                                         #)" << std::endl;
		//std::cout << R"(#                         ,'                \     |"\                /\          /\  #)" << std::endl;
		//std::cout << R"(#                /"|     /                   \    |__"              ( \\        // ) #)" << std::endl;
		//std::cout << R"(#               "_"|    /           z#####z   \  //                  \ \\      // /  #)" << std::endl;
		//std::cout << R"(#                 \\  #####        ##------".  \//                    \_\\||||//_/   #)" << std::endl;
		//std::cout << R"(#                  \\/-----\     /          ".  \                      \/ _  _ \     #)" << std::endl;
		//std::cout << R"(#                   \|      \   |   ,,--..       \                    \/|(O)(O)|     #)" << std::endl;
		//std::cout << R"(#                   | ,.--._ \  (  | ##   \)      \                  \/ |      |     #)" << std::endl;
		//std::cout << R"(#                   |(  ##  )/   \ `-....-//       |///////////////_\/  \      /     #)" << std::endl;
		//std::cout << R"(#                     '--'."      \                \              //     |____|      #)" << std::endl;
		//std::cout << R"(#                  /'    /         ) --.            \            ||     /      \     #)" << std::endl;
		//std::cout << R"(#               ,..|     \.________/    `-..         \   \       \|     \ 0  0 /     #)" << std::endl;
		//std::cout << R"(#            _,##/ |   ,/   /   \           \         \   \       U    / \_//_/      #)" << std::endl;
		//std::cout << R"(#          :###.-  |  ,/   /     \        /' ""\      .\        (     /              #)" << std::endl;
		//std::cout << R"(#         /####|   |   (.___________,---',/    |       |\=._____|  |_/               #)" << std::endl;
		//std::cout << R"(#        /#####|   |     \__|__|__|__|_,/             |####\    |  ||                #)" << std::endl;
		//std::cout << R"(#       /######\   \      \__________/                /#####|   \  ||                #)" << std::endl;
		//std::cout << R"(#      /|#######`. `\                                /#######\   | ||                #)" << std::endl;
		//std::cout << R"(#     /++\#########\  \                      _,'    _/#########\ | ||                #)" << std::endl;
		//std::cout << R"(#    /++++|#########|  \      .---..       ,/      ,'##########.\|_||                #)" << std::endl;
		//std::cout << R"(#   //++++|#########\.  \.              ,-/      ,'########,+++++\\_\\               #)" << std::endl;
		//std::cout << R"(#  /++++++|##########\.   '._        _,/       ,'######,''++++++++\                  #)" << std::endl;
		//std::cout << R"(# |+++++++|###########|       -----."        _'#######' +++++++++++\                 #)" << std::endl;
		//std::cout << R"(# |+++++++|############\.     \\     //      /#######/++++ S@yaN +++\                #)" << std::endl;
		//std::cout << R"(######################################################################################)" << std::endl;
		//std::cout << R"(#      ________________________\\___//______________________________________         #)" << std::endl;
		//std::cout << R"(#     / ____________________________________________________________________)        #)" << std::endl;
		//std::cout << R"(#    / /              _                                             _                #)" << std::endl;
		//std::cout << R"(#   / /__  __           | |                                           | |               #)" << std::endl;
		//std::cout << R"(#  /  __/ / /__         | | _           ____           ____           | |  _            #)" << std::endl;
		//std::cout << R"(# /  /   /  ___|       | || \         / ___)         / _  )          | | / )           #)" << std::endl;
		//std::cout << R"(#/__/   /__/          | | | |        | |           (  __ /          | |< (            #)" << std::endl;
		//std::cout << R"(#                    |_| |_|        |_|            \_____)         |_| \_)           #)" << std::endl;
		//std::cout << R"(#                                                                           19.08.02 #)" << std::endl;
		//std::cout << R"(######################################################################################)" << std::endl;
		std::cout << "                                    ***** FRIENDSHIP ENGINE *****" << std::endl;
		SetConsoleTextAttribute(console, TEXT_DEFAULT);
	}

	void DebugPrintString(const std::string& aString)
	{

		std::cout << aString << std::endl;

#if USE_FE_CONSOLE
		FE::Message message;
		message.myEventType = FE::eMessageType::PrintToConsole;
		message.myMessage = std::string(aString);
		FE::PostMaster::GetInstance()->SendMessage(message);
#endif
	}

	void DebugPrintString(const std::string& aFile, const std::string& aString)
	{
		std::cout << "[" << aFile << "] " << aString << std::endl;

#if USE_FE_CONSOLE
		FE::Message message;
		message.myEventType = FE::eMessageType::PrintToConsole;
		message.myMessage = std::string("[" + aFile + "] " + aString);
		FE::PostMaster::GetInstance()->SendMessage(message);
#endif
	}

	void DebugPrintWarning(const std::string& aString)
	{
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console, YELLOW);
		std::cout << "[WARNING] " << aString << " :S" << std::endl;
		SetConsoleTextAttribute(console, TEXT_DEFAULT);

#if USE_FE_CONSOLE
		FE::Message message;
		message.myEventType = FE::eMessageType::PrintToConsole;
		message.myMessage = std::string("[WARNING] " + aString);
		FE::PostMaster::GetInstance()->SendMessage(message);
#endif
	}

	void DebugPrintError(const std::string& aString)
	{
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console, RED);
		std::cout << "[ERROR] " << aString << " :(" << std::endl;
		SetConsoleTextAttribute(console, TEXT_DEFAULT);

#if USE_FE_CONSOLE
		FE::Message message;
		message.myEventType = FE::eMessageType::PrintToConsole;
		message.myMessage = std::string("[ERROR] " + aString);
		FE::PostMaster::GetInstance()->SendMessage(message);
#endif
	}

	void DebugPrintInfo(const std::string& aString)
	{
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console, PURPLE);
		std::cout << "[INFO] " << aString << std::endl;
		SetConsoleTextAttribute(console, TEXT_DEFAULT);

#if USE_FE_CONSOLE
		FE::Message message;
		message.myEventType = FE::eMessageType::PrintToConsole;
		message.myMessage = std::string("[INFO] " + aString);
		FE::PostMaster::GetInstance()->SendMessage(message);
#endif

	}

	void DebugPrintPosition(const std::string& aObjectName, const Vector3f& aPos)
	{
		std::cout << "[" << aObjectName << "]\t Position x: " << aPos.x << " y: " << aPos.y << " z: " << aPos.z << std::endl;

#if USE_FE_CONSOLE
		std::string text = "[" + aObjectName + "]\t Position x: " + std::to_string(aPos.x) + " y: " + std::to_string(aPos.y) + " z: " + std::to_string(aPos.z);
		FE::Message message;
		message.myEventType = FE::eMessageType::PrintToConsole;
		message.myMessage = std::string(text);
		FE::PostMaster::GetInstance()->SendMessage(message);
#endif
	}

	void DebugPrintRotation(const std::string& aObjectName, const Vector3f& aRot)
	{
		std::cout << "[" << aObjectName << "]\t Rotation x: " << aRot.x << " y: " << aRot.y << " z: " << aRot.z << std::endl;
	}

	void DebugPrintScale(const std::string& aObjectName, const Vector3f& aSca)
	{
		std::cout << "[" << aObjectName << "]\t Scale x: " << aSca.x << " y: " << aSca.y << " z: " << aSca.z << std::endl;
	}

	void DebugPrintVector3(const std::string& aObjectName, const Vector3f& aVec)
	{
		std::cout << "[" << aObjectName << "]\t x: " << aVec.x << " y: " << aVec.y << " z: " << aVec.z << std::endl;
	}

	void DebugPrintTransform(const std::string& aObjectName, const Transform& aTransform)
	{
		DebugPrintPosition(aObjectName, aTransform.GetPosition());
		DebugPrintRotation(aObjectName, aTransform.GetEulerRotation());
		DebugPrintScale(aObjectName, aTransform.GetScale());
	}

	void DebugPrintPathError(const std::string& aPath)
	{
		DebugPrintWarning("Could not find asset (" + aPath + ")");
	}

}