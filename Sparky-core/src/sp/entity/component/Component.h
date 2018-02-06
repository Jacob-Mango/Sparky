#pragma once

#include "sp/Common.h"
#include "sp/Types.h"
#include "sp/String.h"

#include "sp/utils/Timestep.h"

namespace sp { namespace entity {
	class Entity;
} }

namespace sp { namespace world {
	class World;
} }

namespace sp { namespace entity { namespace component {

	struct SP_API ComponentType
	{
		String name;
	};

	class SP_API Component
	{
	protected:
		Entity* m_Entity;
		world::World* m_World;
	public:
		void SetEntity(Entity* entity) { m_Entity = entity; }
		void SetWorld(world::World* world) { m_World = world; }

		virtual Entity* GetEntity() { return m_Entity; }
		virtual world::World* GetWorld() { return m_World; }
		virtual ComponentType* GetType() const { return nullptr; }

		virtual void OnInit() {}
		virtual void OnUpdate(const sp::Timestep& ts) {}
	};

} } }
