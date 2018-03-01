#pragma once

#include "sp/Common.h"
#include "sp/String.h"

#include "vec3.h"
#include "vec4.h"
#include "maths_func.h"

namespace sp {
	namespace maths {

		struct Quaternion;

		struct SP_API mat3
		{
			union
			{
				// [row + col * 4]
				float elements[3 * 3];
				vec3 rows[3];
			};

			mat3();
			mat3(float diagonal);
			mat3(float* elements);
			mat3(const vec3& row0, const vec3& row1, const vec3& row2);

			static mat3 Identity();

			mat3& Multiply(const mat3& other);
			friend SP_API mat3 operator*(mat3 left, const mat3& right);
			mat3& operator*=(const mat3& other);

			vec3 Multiply(const vec3& other) const;
			friend SP_API vec3 operator*(const mat3& left, const vec3& right);

			mat3& Invert();

			vec3 GetColumn(int index) const;
			void SetColumn(uint index, const vec3& column);

			//static mat3 Orthographic(float left, float right, float bottom, float top, float near, float far);
			//static mat3 Perspective(float fov, float aspectRatio, float near, float far);
			//static mat3 LookAt(const vec3& camera, const vec3& object, const vec3& up);

			static mat3 Translate(const vec3& translation);
			static mat3 Rotate(float angle, const vec3& axis);
			static mat3 Rotate(const Quaternion& quat);
			static mat3 Scale(const vec3& scale);
			static mat3 Invert(const mat3& matrix);

			static mat3 Transpose(const mat3& matrix);

			String ToString() const;
		};

	}
}