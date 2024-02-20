#pragma once

class AnimationPlayer;

class AnimatorSystem : public System
{
public:
	AnimatorSystem(World* aWorld);
	~AnimatorSystem() override;

	void Init() override;
	void Update(const float&) override;
private:
	AnimationPlayer* myAnimationPlayer;
};
