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
	AddComponent(spnew MeshComponent(MeshFactory::CreateCube(1.0f, mat)));
	
	maths::mat4 pos = maths::mat4::Identity();
	pos.SetPosition(position);
	TransformComponent* pc = spnew TransformComponent(pos);
	AddComponent(pc);

	AddComponent(spnew AnimationComponent());

	m_ShouldUpdate = true;
}


void Ball::OnInit()
{
}

float time = 0;

void Ball::OnUpdate(const sp::Timestep& ts)
{
	maths::mat4* transform = GetComponent<TransformComponent>()->transform;
	transform->Translate(maths::vec3(0, maths::sin(time * ts.GetSeconds()), 0));
}