#include "sp/sp.h"
#include "LightSetup.h"

namespace sp {
	namespace graphics {

		Light::Light(const maths::vec3& position, float intensity, const maths::vec4& color)
			: intensity(intensity), color(color), position(position)
		{
			direction = maths::vec3(0);
			vector = maths::vec3(0);
		}

	}
}