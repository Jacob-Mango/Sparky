#pragma once

#include "sp/sp.h"
#include "sp/Common.h"

#include "sp/utils/Timestep.h"

#include "component/Components.h"

namespace sp {
	namespace entity {

		class SP_API Entity
		{
		protected:
			std::vector<component::Component*> m_Components;

			bool m_ShouldUpdate;
		public:
			Entity();
			Entity(graphics::Sprite* sprite, maths::mat4* transform = &maths::mat4::Identity());
			Entity(graphics::Mesh* mesh, maths::mat4* transform = &maths::mat4::Identity());

			void AddComponent(component::Component* component);

			template <typename T>
			const T* GetComponent() const
			{
				return GetComponentInternal<T>();
			}

			std::vector<component::Component*> GetComponents()
			{
				return m_Components;
			}

			template <typename T>
			T* GetComponent()
			{
				return (T*)GetComponentInternal<T>();
			}

			bool ShouldUpdate()
			{
				return m_ShouldUpdate;
			}

			virtual void OnInit() {}
			virtual void OnUpdate(const sp::Timestep& ts) {}
		private:
			template <typename T>
			const T* GetComponentInternal() const
			{
				component::ComponentType* type = T::GetStaticType();
				if (m_Components.size() == 0) return nullptr;
				for (auto x : m_Components)
				{
					if (x->GetType() == type)
						return (const T*)x;
				}
				return nullptr;
			}
		};

	}
}
