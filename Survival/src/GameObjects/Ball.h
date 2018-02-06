#pragma once

#include <Sparky.h>

class Ball : public sp::entity::Entity
{
public:
	Ball(const sp::maths::vec3& position, sp::graphics::MaterialInstance* mat);

	void OnInit() override;
	void OnUpdate(const sp::Timestep& ts) override;
};

