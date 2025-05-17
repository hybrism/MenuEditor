#include "pch.h"
#include "StringHelper.h"
#include <locale>
#include <codecvt>
#include <algorithm>
#include <cctype>
#include <string>

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

std::string StringHelper::GetNameFromPath(std::string aPath)
{
	size_t n = aPath.rfind("/");
	if (n != std::string::npos)
	{
		aPath = aPath.substr(n + 1);
	}

	n = aPath.rfind("\\");
	if (n != std::string::npos)
	{
		aPath = aPath.substr(n + 1);
	}

	n = aPath.rfind(".");
	if (n != std::string::npos)
	{
		aPath = aPath.substr(0, n);
	}

	return aPath;
}

std::string StringHelper::GetMaterialNameFromPath(std::string aPath)
{
	std::string name = GetNameFromPath(aPath);
	
	name = name.substr(name.find_first_of("_") + 1);
	name = name.substr(0, name.rfind("_"));
	
	return name;
}

std::string StringHelper::ToLower(std::string aValue)
{
	std::transform(aValue.begin(), aValue.end(), aValue.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return aValue;
}
