#pragma once

#include "sp/Types.h"
#include "sp/maths/maths.h"

namespace sp {
	namespace graphics {
		union SP_API Light
		{
			unsigned char bytes[sizeof(float) * 8];
			struct
			{
				maths::vec4 color;
				maths::vec3 position;
				maths::vec3 direction;
				maths::vec3 vector;
				float intensity;
			};
			
			Light() {
				color = maths::vec4();
				position = maths::vec3(0);
				direction = maths::vec3(0);
				vector = maths::vec3(0);
				intensity = 0;
			}

			Light(const maths::vec3& position, float intensity = 1.0f, const maths::vec4& color = maths::vec4(1.0f));
		};

	}
}