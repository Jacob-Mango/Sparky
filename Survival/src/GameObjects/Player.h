#pragma once

#include <Sparky.h>

namespace sp { namespace maths {

	struct vec2;
	struct vec3;
	struct vec4;
	struct mat4;

} }

class btRigidBody;

__declspec(align(16)) class btVector3;

class Player : public sp::entity::Entity
{
private:
	sp::graphics::Scene* m_Scene;

	float m_MouseSensitivity;
	bool m_MouseWasGrabbed;

	float m_Speed, m_SprintSpeed;
public:
	Player(sp::graphics::Scene* scene, sp::graphics::API::Shader* shader);
	~Player();

	void OnInit() override;
	void OnUpdate(const sp::Timestep& ts) override;
private:
	void CheckInputs(const sp::Timestep& ts);
};

