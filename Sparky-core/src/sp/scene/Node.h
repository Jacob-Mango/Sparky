#pragma once

#include "sp/sp.h"
#include "sp/Common.h"

#include "sp/utils/Timestep.h"

#include "component/Components.h"

namespace sp {
	namespace scene {

		class SP_API Node
		{
		protected:
			std::vector<Node> m_Children;
			Node* m_Parent;

			maths::mat4 m_Transform;
		public:
			Node(const maths::mat4& transform = maths::mat4::Identity());

			void Init();
			void Update(const sp::Timestep& ts);

			virtual void OnInit() {}
			virtual void OnUpdate(const sp::Timestep& ts) {}

			inline Node* GetParent() { return m_Parent; }

			inline maths::mat4 GetTransform() { return m_Transform; }
			inline void SetTransform(maths::mat4 transform) { m_Transform = transform; }
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
