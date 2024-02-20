#pragma once
#include <game/scripting/ScriptCommon.h>

constexpr uint8_t ScriptLinkColors[(size_t)ScriptLinkDataType::Count][3] =
{
	{245, 0, 0},		//Unknown
	{185, 185, 185},	//Flow
	{149, 0, 0},		//Bool
	{31, 227, 175},		//Int
	{56, 213, 0},		//Float
	{255, 0, 212}		//String
};

constexpr uint8_t ScriptLinkHoverColors[(size_t)ScriptLinkDataType::Count][3] =
{
	{255, 50, 50},		//Unknown
	{255, 255, 255},	//Flow
	{73, 42, 58},		//Bool
	{32, 92, 97},		//Int
	{37, 87, 48},		//Float
	{87, 33, 102}		//String

};

constexpr uint8_t ScriptLinkSelectedColors[(size_t)ScriptLinkDataType::Count][3] =
{
	{255, 50, 50},		//Unknown
	{255, 255, 255},	//Flow
	{73, 42, 58},		//Bool
	{32, 92, 97},		//Int
	{37, 87, 48},		//Float
	{87, 33, 102}		//String
};
