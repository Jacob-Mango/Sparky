#include "sp/sp.h"
#include "Entity.h"

#define SIZEOF_ARRAY( a ) (sizeof( a ) / sizeof( a[ 0 ] )) 

namespace sp { namespace entity {

	using namespace component;

	Entity::Entity()
	{
		m_ShouldUpdate = false;
	}

	Entity::Entity(graphics::Sprite* sprite, maths::mat4* transform)
	{
		AddComponent(new SpriteComponent(sprite));
		AddComponent(new TransformComponent(transform));
		m_ShouldUpdate = false;
	}

	Entity::Entity(graphics::Mesh* mesh, maths::mat4* transform)
	{
		AddComponent(new TransformComponent(transform));
		
		AddComponent(new MeshComponent(mesh));
		AddComponent(new AnimationComponent());

		m_ShouldUpdate = false;
	}

	void Entity::AddComponent(component::Component* component)
	{
		component->SetEntity(this);
		m_Components.push_back(component);
	}

} }