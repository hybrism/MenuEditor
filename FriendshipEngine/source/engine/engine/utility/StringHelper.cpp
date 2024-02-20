#include "pch.h"
#include "StringHelper.h"
#include <locale>
#include <codecvt>

#pragma warning(disable : 4996)

std::wstring StringHelper::s2ws(const std::string& aString)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(aString);
}

std::string StringHelper::ws2s(const std::wstring& aWString)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(aWString);
}

std::string StringHelper::GetFileExtension(const std::string& aString)
{
	size_t lastindex = aString.find_last_of(".");
	return aString.substr(lastindex + 1, aString.length());
}

std::string StringHelper::GetFileName(std::string aString)
{
	aString = aString.substr(aString.find_last_of("/") + 1);
	aString = aString.substr(0, aString.find_last_of("."));
	return aString;
}
