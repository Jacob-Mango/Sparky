#pragma once

#include <math.h>
#include "maths_func.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#include "AABB.h"
#include "Rectangle.h"

#include "tvec2.h"

#include "mat3.h"
#include "mat4.h"

#include "Quaternion.h"

namespace sp { namespace maths {
	
	typedef tvec2<int32> ivec2;
	typedef tvec2<uint> uvec2;

} }

#ifdef DOUBLE_PRECISION
typedef double decimal;
#else
typedef float decimal;
#endif 

#define M_PI 3.14159265359