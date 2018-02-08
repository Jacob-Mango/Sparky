#pragma once

#include <Sparky.h>

class RenderLayer : public sp::graphics::Layer3D
{
public:
	RenderLayer();
	~RenderLayer();

	void OnInit(sp::graphics::Renderer3D* renderer, sp::graphics::Scene* scene) override;

	void OnTick() override;
	void OnUpdate(const sp::Timestep& ts) override;
	void OnEvent(sp::events::Event& event) override;

	void OnRender(sp::graphics::Renderer3D& renderer);
};

