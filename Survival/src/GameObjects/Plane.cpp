#include "Plane.h"


using namespace sp;
using namespace graphics;
using namespace maths;

using namespace events;
using namespace entity;
using namespace component;

using namespace API;

using namespace world;

Plane::Plane(const sp::maths::vec3& position, sp::graphics::MaterialInstance* mat)
{
	AddComponent(spnew MeshComponent(MeshFactory::CreatePlane(128, vec3(0, 1, 0), mat)));

	maths::mat4 pos = maths::mat4::Identity();
	pos.SetPosition(position);
	TransformComponent* pc = spnew TransformComponent(pos);
	AddComponent(pc);

	AddComponent(spnew AnimationComponent());

	m_ShouldUpdate = true;
}


void Plane::OnInit()
{
}

void Plane::OnUpdate(const sp::Timestep& ts)
{
}