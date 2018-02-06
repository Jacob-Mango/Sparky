#include "sp/sp.h"
#include "TransformComponent.h"

namespace sp { namespace entity { namespace component {

	TransformComponent::TransformComponent(maths::mat4* transform)
		: transform(transform)
	{
	}

	TransformComponent::TransformComponent(maths::mat4& transform)
		: transform(&transform)
	{
	}

} } }