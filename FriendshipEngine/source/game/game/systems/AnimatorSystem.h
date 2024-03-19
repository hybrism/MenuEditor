#pragma once

class AnimationPlayer;
struct SceneUpdateContext;

class AnimatorSystem : public System<AnimatorSystem>
{
public:
	AnimatorSystem(World* aWorld);
	~AnimatorSystem() override;

	void Init() override;
	void Update(const SceneUpdateContext&) override;
private:
	AnimationPlayer* myAnimationPlayer;
};
