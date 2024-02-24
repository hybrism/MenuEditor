#pragma once
#include "../math/Vector.h"
#include "../math/Transform.h"

#define TEXT_DEFAULT 7
#define RED 4
#define YELLOW 6
#define PURPLE 5

#define WELCOME_TO_FRIENDSHIPENGINE Error::PrintIntro();
#define Print(msg) Error::DebugPrintString(msg);
#define PrintF(filename, msg) Error::DebugPrintString(filename, msg);
#define PrintW(msg) Error::DebugPrintWarning(msg);
#define PrintE(msg) Error::DebugPrintError(msg);
#define PrintI(msg) Error::DebugPrintInfo(msg);
#define PrintPosition(name, pos) Error::DebugPrintPosition(name, pos);
#define PrintRotation(name, pos) Error::DebugPrintRotation(name, pos);
#define PrintScale(name, pos) Error::DebugPrintScale(name, pos);
#define PrintVec3(name, pos) Error::DebugPrintVector3(name, pos);
#define PrintTransform(name, transform) Error::DebugPrintTransform(name, transform);
#define PrintPathError(path) Error::DebugPrintPathError(path);

namespace Error
{
	void PrintIntro();
	void DebugPrintString(const std::string& aString);
	void DebugPrintString(const std::string& aFile, const std::string& aString);
	void DebugPrintWarning(const std::string& aString);
	void DebugPrintError(const std::string& aString);
	void DebugPrintInfo(const std::string& aString);
	void DebugPrintPosition(const std::string& aObjectName, const Vector3f& aPos);
	void DebugPrintRotation(const std::string& aObjectName, const Vector3f& aRot);
	void DebugPrintScale(const std::string& aObjectName, const Vector3f& aSca);
	void DebugPrintVector3(const std::string& aObjectName, const Vector3f& aVec);
	void DebugPrintTransform(const std::string& aObjectName, const Transform& aTransform);
	void DebugPrintPathError(const std::string& aPath);
}