#include "pch.h"

#include "ScriptCommon.h"

#include "utility/ScriptJson.h"
#include "ScriptStringRegistry.h"

void ScriptLinkData::LoadFromJson(ScriptLinkDataType type, const ScriptJson& json)
{
	if (!json.json.is_null())
	{
		switch (type)
		{
		case ScriptLinkDataType::Bool:
			data = json.json.get<bool>();
			break;
		case ScriptLinkDataType::Int:
			data = json.json.get<int>();
			break;
		case ScriptLinkDataType::Float:
			data = json.json.get<float>();
			break;
		case ScriptLinkDataType::String:
		{
			std::string string = json.json.get<std::string>();
			data = ScriptStringRegistry::RegisterOrGetStringId(string);
			break;
		}
		case ScriptLinkDataType::Entity:
		{
			data = json.json.get<Entity>();
			break;
		}
		case ScriptLinkDataType::Vector3f:
		{
			Vector3f vector = { json.json[0].get<float>(), json.json[1].get<float>(), json.json[2].get<float>() };
			data = vector;
			break;
		}
		}
	}
}

void ScriptLinkData::WriteToJson(ScriptLinkDataType type, ScriptJson& json) const
{
	if (!std::holds_alternative<std::monostate>(data))
	{
		switch (type)
		{
		case ScriptLinkDataType::Bool:
			json.json = std::get<bool>(data);
			break;
		case ScriptLinkDataType::Int:
			json.json = std::get<int>(data);
			break;
		case ScriptLinkDataType::Float:
			json.json = std::get<float>(data);
			break;
		case ScriptLinkDataType::String:
		{
			ScriptStringId stringId = std::get<ScriptStringId>(data);
			json.json = ScriptStringRegistry::GetStringFromStringId(stringId).data();
			break;
		}
		case ScriptLinkDataType::Entity:
		{
			json.json = std::get<Entity>(data);
			break;
		}
		case ScriptLinkDataType::Vector3f:
		{
			Vector3f vector = std::get<Vector3f>(data);
			json.json = { vector.x, vector.y, vector.z };
			break;
		}
		}
	}
}
