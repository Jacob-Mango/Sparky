#pragma once

#include "sp/Common.h"
#include "sp/Types.h"
#include "sp/String.h"

#include "sp/utils/Timestep.h"

namespace sp { namespace entity {
	class Entity;
} }

namespace sp { namespace graphics {
	class Scene;
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
		graphics::Scene* m_Scene;
	public:
		void SetEntity(Entity* entity) { m_Entity = entity; }
		void SetScene(graphics::Scene* scene) { m_Scene = scene; }

		virtual Entity* GetEntity() { return m_Entity; }
		virtual graphics::Scene* GetWorld() { return m_Scene; }
		virtual ComponentType* GetType() const { return nullptr; }

		virtual void OnInit() {}
		virtual void OnUpdate(const sp::Timestep& ts) {}
	};

} } }
