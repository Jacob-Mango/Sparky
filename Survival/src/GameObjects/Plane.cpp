#include "Plane.h"


using namespace sp;
using namespace graphics;
using namespace maths;

using namespace events;
using namespace entity;
using namespace component;

using namespace API;

using namespace graphics;

Plane::Plane(const sp::maths::vec3& position, sp::graphics::MaterialInstance* mat)
{
	AddComponent(spnew MeshComponent(MeshFactory::CreatePlane2(128, 8, mat)));

	maths::mat4* transform = spnew maths::mat4(1.0f);
	transform->SetPosition(position);
	AddComponent(spnew TransformComponent(transform));
	AddComponent(spnew AnimationComponent());

	m_ShouldUpdate = true;
}


void Plane::OnInit()
{
}

void Plane::OnUpdate(const sp::Timestep& ts)
{
}