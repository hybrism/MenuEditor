#pragma once
class SceneManager;
struct SceneUpdateContext;

class EventSystem : public System<EventSystem>
{
public:
	EventSystem(World* aWorld);
	//~HitboxSystem() override;
	void SetSceneManager(SceneManager* aManager) { mySceneManager = aManager; }
	// Inherited via System
	virtual void Update(SceneUpdateContext& dt) override;

private:
	SceneManager* mySceneManager;
};