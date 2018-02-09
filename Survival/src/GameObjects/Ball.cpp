#include "Ball.h"

using namespace sp;
using namespace graphics;

using namespace events;
using namespace entity;
using namespace component;

using namespace API;

using namespace graphics;

Ball::Ball(const sp::maths::vec3& position, sp::graphics::MaterialInstance* mat)
{
	AddComponent(spnew MeshComponent(MeshFactory::CreateCube(4.0f, mat)));
	
	maths::mat4* transform = spnew maths::mat4(1.0f);
	transform->SetPosition(position);
	AddComponent(spnew TransformComponent(transform));

	AddComponent(spnew AnimationComponent());

	m_ShouldUpdate = true;
}

void Ball::OnInit()
{
}

void Ball::OnUpdate(const sp::Timestep& ts)
{
	// GetComponent<TransformComponent>()->transform->Translate();
}