#pragma once

#include <Sparky.h>

class Plane : public sp::entity::Entity
{
public:
	Plane(const sp::maths::vec3& position, sp::graphics::MaterialInstance* mat);

	void OnInit() override;
	void OnUpdate(const sp::Timestep& ts) override;
};

