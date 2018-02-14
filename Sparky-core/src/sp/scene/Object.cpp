#include "sp/sp.h"
#include "Object.h"

#define SIZEOF_ARRAY( a ) (sizeof( a ) / sizeof( a[ 0 ] )) 

namespace sp {
	namespace scene {

		using namespace component;

		Object::Object(const maths::mat4& transform) : Node(transform)
		{
		}

		Object::Object(graphics::Sprite* sprite, const maths::mat4& transform) : Node(transform)
		{
			AddComponent(spnew SpriteComponent(sprite));
		}

		Object::Object(graphics::Mesh* mesh, const maths::mat4& transform) : Node(transform)
		{
			AddComponent(spnew MeshComponent(mesh));
		}

		void Object::AddComponent(component::Component* component)
		{
			SP_ASSERT(component->GetType());
			m_Components[component->GetType()] = component;
			m_Components[component->GetType()]->SetParent(this);
		}

	}
}