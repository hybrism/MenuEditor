#pragma once

class RenderingSystem : public System
{
public:
	RenderingSystem(World* aWorld);
	~RenderingSystem() override;

	void Update(const float&) override;
	void Render() override;
private:

};
