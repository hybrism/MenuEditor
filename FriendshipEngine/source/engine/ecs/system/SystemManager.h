#pragma once

#include <unordered_map>
#include <typeinfo>
#include <cassert>

#include "../entity/Entity.h"
#include "../component/Component.h"
#include "../system/System.h"
#include "../engine/utility/Error.h"
class World;

#define SHOULD_PRINT_SYSTEM_EXECUTION_ORDER 0

class SystemManager
{
public:
	SystemManager(World* aWorld)
	{
		mySystemSignatures = {};
		mySystems = {};
		myWorld = aWorld;
	}

	~SystemManager()
	{
		for (auto& system : mySystems)
		{
			delete system.second;
		}
		mySystems.clear();
		myWorld = nullptr;
	}

	void Reset()
	{
		for (auto& system : mySystems)
		{
			system.second->myEntities.clear();
		}
	}

	void Update(const float& dt)
	{
#if SHOULD_PRINT_SYSTEM_EXECUTION_ORDER
		Print("\n--------------------------------------------------------\n");
#endif
		for (auto& system : mySystems)
		{
#if SHOULD_PRINT_SYSTEM_EXECUTION_ORDER
			Print(system.first);
#endif
			system.second->Update(dt);
		}
	}

	void Render()
	{
		for (auto& system : mySystems)
		{
			system.second->Render();
		}
	}

	void Init()
	{
		for (auto& system : mySystems)
		{
			system.second->Init();
		}
	}

	template<typename T, typename... Param>
	T* RegisterSystem(Param... args)
	{
#ifdef _DEBUG
		const char* hash = typeid(T).name();
#else
		size_t hash = typeid(T).hash_code();
#endif

		assert(mySystems.find(hash) == mySystems.end() && "Registering system more than once.");

		T* system = new T(myWorld, args...);
		mySystems.insert({ hash, system });
		return system;
	}

	// WARNING DON'T USE THIS!!!!
	template<typename T>
	T* GetSystem()
	{
#ifdef _DEBUG
		const char* hash = typeid(T).name();
#else
		size_t hash = typeid(T).hash_code();
#endif

		assert(mySystems.find(hash) != mySystems.end() && "System is not registered.");

		return static_cast<T*>(mySystems.at(hash));
	}


	template<typename T>
	void SetSignature(const ComponentSignature& aSignature)
	{
#ifdef _DEBUG
		const char* hash = typeid(T).name();
#else
		size_t hash = typeid(T).hash_code();
#endif
		// if you will need to change the signature in runtime for some reason, please discuss it with the team first and then add another check for it
		assert(mySystemSignatures.find(hash) == mySystemSignatures.end() && "Setting signature at _Val1 that already exists.");

		mySystemSignatures.insert({ hash, aSignature });
	}

	void OnEntityDestroyed(const Entity& aEntity)
	{
		for (auto const& pair : mySystems)
		{
			System* system = pair.second;
			system->myEntities.erase(aEntity);
		}
	}

	void OnEntitySignatureChanged(const Entity& aEntity, const ComponentSignature& aSignature)
	{
		for (auto const& pair : mySystems)
		{
			auto const& type = pair.first;
			System* system = pair.second;
			ComponentSignature& systemSignature = mySystemSignatures.at(type);

			if ((aSignature & systemSignature) == systemSignature)
			{
				system->myEntities.insert(aEntity);
				continue;
			}

			system->myEntities.erase(aEntity);
		}
	}
private:
	World* myWorld;

	std::unordered_map<
#ifdef _DEBUG
		const char*
#else
		size_t
#endif
		, ComponentSignature> mySystemSignatures; // used for filtering entities
	std::unordered_map<
#ifdef _DEBUG
		const char*
#else
		size_t
#endif
		, System*> mySystems;
};
