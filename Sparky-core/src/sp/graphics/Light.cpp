#include "sp/sp.h"
#include "LightSetup.h"

namespace sp { namespace graphics {

	Light::Light(const maths::vec3& position, float intensity, const maths::vec4& color)
		: direction(maths::vec3(0,0,0)), intensity(intensity), color(color), p0(0.0f), p1(0.0f), position(position), lightVector(maths::vec3(0.0f))
	{
	}

} }