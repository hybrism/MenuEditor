#pragma once

class RenderingSystem : public System<RenderingSystem>
{
public:
	RenderingSystem(World* aWorld);
	~RenderingSystem() override;

	void Init() override;
	void Update(const SceneUpdateContext&) override;
	void Render() override;
private:
	std::vector<std::vector<unsigned int>> myMeshOrderCounter;
};
