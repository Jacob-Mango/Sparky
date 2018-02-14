#include "sp/sp.h"
#include "Node.h"

namespace sp {
	namespace scene {

		Node::Node(const maths::mat4& transform) : m_Transform(transform)
		{
		}

		void Node::Init()
		{
			OnInit();
			for (Node child : m_Children)
				child.Init();
		}

		void Node::Update(const sp::Timestep& ts)
		{
			OnUpdate(ts);
			for (Node child : m_Children)
				child.Update(ts);
		}

	}
}