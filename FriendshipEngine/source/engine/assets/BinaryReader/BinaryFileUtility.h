#pragma once

struct Skeleton;

class BinaryFileUtility
{
public:
	static std::string GetModelFilePathFromName(const std::string& aName);
	static std::string GetAnimationFilePathFromName(const std::string& aName);

	static void WriteStringToFile(std::ofstream& aFile, const std::string& aString);
	static void WriteBoneToFile(std::ofstream& aFile, Bone& aBone);

	static void ReadStringFromFile(std::ifstream& aFile, std::string& outString);
	static void ReadBoneFromFile(std::ifstream& file, Skeleton* aSkeleton, Bone& outBone);
};
