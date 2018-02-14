#pragma once

#include <Sparky.h>

class Test3D : public sp::graphics::Layer3D
{
private:
	sp::graphics::Camera* m_FPSCamera;

	std::vector<sp::scene::Object*> m_Spheres;
	sp::graphics::Light* m_Light;
	float m_Rotation;
	bool m_SetUniforms[2];
	std::vector<sp::graphics::PBRMaterial*> m_Materials;
public:
	Test3D();
	~Test3D();

	void OnInit(sp::graphics::Renderer3D* renderer, sp::scene::Scene* scene) override;

	void OnTick() override;
	void OnUpdate(const sp::Timestep& ts) override;
	void OnEvent(sp::events::Event& event) override;

	void OnRender(sp::graphics::Renderer3D& renderer);
};