#pragma once

#include "sp/sp.h"
#include "sp/Common.h"

#include "sp/utils/Timestep.h"

#include "component/Components.h"

#include "Node.h"

namespace sp {
	namespace scene {

		class SP_API Object : public Node
		{
		protected:
			std::unordered_map<component::ComponentType*, component::Component*> m_Components;
		public:
			Object(const maths::mat4& transform = maths::mat4::Identity());

			Object(graphics::Sprite* sprite, const maths::mat4& transform = maths::mat4::Identity());
			Object(graphics::Mesh* mesh, const maths::mat4& transform = maths::mat4::Identity());

			void AddComponent(component::Component* component);

			template <typename T>
			const T* GetComponent() const
			{
				return GetComponentInternal<T>();
			}

			template <typename T>
			T* GetComponent()
			{
				return (T*)GetComponentInternal<T>();
			}

			std::unordered_map<component::ComponentType*, component::Component*> GetComponents() {
				return m_Components;
			}
		private:
			template <typename T>
			const T* GetComponentInternal() const
			{
				component::ComponentType* type = T::GetStaticType();
				auto it = m_Components.find(type);
				if (it == m_Components.end())
					return nullptr;
				return (T*)it->second; 
			}
		};
	}
}
