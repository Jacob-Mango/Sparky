#pragma once

#include "sp/Common.h"
#include "sp/Types.h"
#include "sp/String.h"

#include "sp/utils/Timestep.h"

namespace sp { namespace scene {
	class Object;
} }

namespace sp { namespace scene { namespace component {

	struct SP_API ComponentType
	{
		String name;
	};

	class SP_API Component
	{
	protected:
		scene::Object* m_Parent;
	public:
		virtual ComponentType* GetType() const { return nullptr; }

		virtual void OnInit() {}
		virtual void OnUpdate(const sp::Timestep& ts) {}

		inline scene::Object* GetParent() { return m_Parent; }
		inline void SetParent(scene::Object* parent) { m_Parent = parent; }
	};

} } }
