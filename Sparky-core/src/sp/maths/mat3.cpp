#include "sp/sp.h"
#include "mat3.h"

#include <sstream>

#include "Quaternion.h"

namespace sp {
	namespace maths {

		mat3::mat3()
		{
			memset(elements, 0, 3 * 3 * sizeof(float));
		}

		mat3::mat3(float diagonal)
		{
			memset(elements, 0, 3 * 3 * sizeof(float));
			elements[0 + 0 * 3] = diagonal;
			elements[1 + 1 * 3] = diagonal;
			elements[2 + 2 * 3] = diagonal;
		}

		mat3::mat3(float* elements)
		{
			memcpy(this->elements, elements, 3 * 3 * sizeof(float));
		}

		mat3::mat3(const vec3& row0, const vec3& row1, const vec3& row2)
		{
			rows[0] = row0;
			rows[1] = row1;
			rows[2] = row2;
		}

		mat3 mat3::Identity()
		{
			return mat3(1.0f);
		}

		mat3& mat3::Multiply(const mat3& other)
		{
			float data[12];
			for (int32 row = 0; row < 3; row++)
			{
				for (int32 col = 0; col < 3; col++)
				{
					float sum = 0.0f;
					for (int32 e = 0; e < 3; e++)
					{
						sum += elements[e + row * 3] * other.elements[col + e * 3];
					}
					data[col + row * 3] = sum;
				}
			}
			memcpy(elements, data, 3 * 3 * sizeof(float));
			return *this;
		}

		vec3 mat3::Multiply(const vec3& other) const
		{
			return other.Multiply(*this);
		}

		mat3 operator*(mat3 left, const mat3& right)
		{
			return left.Multiply(right);
		}

		mat3& mat3::operator*=(const mat3& other)
		{
			return Multiply(other);
		}

		vec3 operator*(const mat3& left, const vec3& right)
		{
			return left.Multiply(right);
		}

		mat3& mat3::Invert()
		{
			float temp[12];

			temp[0] = elements[5] * elements[10] * elements[15] -
				elements[5] * elements[11] * elements[14] -
				elements[9] * elements[6] * elements[15] +
				elements[9] * elements[7] * elements[14] +
				elements[13] * elements[6] * elements[11] -
				elements[13] * elements[7] * elements[10];

			temp[4] = -elements[4] * elements[10] * elements[15] +
				elements[4] * elements[11] * elements[14] +
				elements[8] * elements[6] * elements[15] -
				elements[8] * elements[7] * elements[14] -
				elements[12] * elements[6] * elements[11] +
				elements[12] * elements[7] * elements[10];

			temp[8] = elements[4] * elements[9] * elements[15] -
				elements[4] * elements[11] * elements[13] -
				elements[8] * elements[5] * elements[15] +
				elements[8] * elements[7] * elements[13] +
				elements[12] * elements[5] * elements[11] -
				elements[12] * elements[7] * elements[9];

			temp[12] = -elements[4] * elements[9] * elements[14] +
				elements[4] * elements[10] * elements[13] +
				elements[8] * elements[5] * elements[14] -
				elements[8] * elements[6] * elements[13] -
				elements[12] * elements[5] * elements[10] +
				elements[12] * elements[6] * elements[9];

			temp[1] = -elements[1] * elements[10] * elements[15] +
				elements[1] * elements[11] * elements[14] +
				elements[9] * elements[2] * elements[15] -
				elements[9] * elements[3] * elements[14] -
				elements[13] * elements[2] * elements[11] +
				elements[13] * elements[3] * elements[10];

			temp[5] = elements[0] * elements[10] * elements[15] -
				elements[0] * elements[11] * elements[14] -
				elements[8] * elements[2] * elements[15] +
				elements[8] * elements[3] * elements[14] +
				elements[12] * elements[2] * elements[11] -
				elements[12] * elements[3] * elements[10];

			temp[9] = -elements[0] * elements[9] * elements[15] +
				elements[0] * elements[11] * elements[13] +
				elements[8] * elements[1] * elements[15] -
				elements[8] * elements[3] * elements[13] -
				elements[12] * elements[1] * elements[11] +
				elements[12] * elements[3] * elements[9];

			temp[13] = elements[0] * elements[9] * elements[14] -
				elements[0] * elements[10] * elements[13] -
				elements[8] * elements[1] * elements[14] +
				elements[8] * elements[2] * elements[13] +
				elements[12] * elements[1] * elements[10] -
				elements[12] * elements[2] * elements[9];

			temp[2] = elements[1] * elements[6] * elements[15] -
				elements[1] * elements[7] * elements[14] -
				elements[5] * elements[2] * elements[15] +
				elements[5] * elements[3] * elements[14] +
				elements[13] * elements[2] * elements[7] -
				elements[13] * elements[3] * elements[6];

			temp[6] = -elements[0] * elements[6] * elements[15] +
				elements[0] * elements[7] * elements[14] +
				elements[4] * elements[2] * elements[15] -
				elements[4] * elements[3] * elements[14] -
				elements[12] * elements[2] * elements[7] +
				elements[12] * elements[3] * elements[6];

			temp[10] = elements[0] * elements[5] * elements[15] -
				elements[0] * elements[7] * elements[13] -
				elements[4] * elements[1] * elements[15] +
				elements[4] * elements[3] * elements[13] +
				elements[12] * elements[1] * elements[7] -
				elements[12] * elements[3] * elements[5];

			temp[14] = -elements[0] * elements[5] * elements[14] +
				elements[0] * elements[6] * elements[13] +
				elements[4] * elements[1] * elements[14] -
				elements[4] * elements[2] * elements[13] -
				elements[12] * elements[1] * elements[6] +
				elements[12] * elements[2] * elements[5];

			temp[3] = -elements[1] * elements[6] * elements[11] +
				elements[1] * elements[7] * elements[10] +
				elements[5] * elements[2] * elements[11] -
				elements[5] * elements[3] * elements[10] -
				elements[9] * elements[2] * elements[7] +
				elements[9] * elements[3] * elements[6];

			temp[7] = elements[0] * elements[6] * elements[11] -
				elements[0] * elements[7] * elements[10] -
				elements[4] * elements[2] * elements[11] +
				elements[4] * elements[3] * elements[10] +
				elements[8] * elements[2] * elements[7] -
				elements[8] * elements[3] * elements[6];

			temp[11] = -elements[0] * elements[5] * elements[11] +
				elements[0] * elements[7] * elements[9] +
				elements[4] * elements[1] * elements[11] -
				elements[4] * elements[3] * elements[9] -
				elements[8] * elements[1] * elements[7] +
				elements[8] * elements[3] * elements[5];

			temp[15] = elements[0] * elements[5] * elements[10] -
				elements[0] * elements[6] * elements[9] -
				elements[4] * elements[1] * elements[10] +
				elements[4] * elements[2] * elements[9] +
				elements[8] * elements[1] * elements[6] -
				elements[8] * elements[2] * elements[5];

			float determinant = elements[0] * temp[0] + elements[1] * temp[4] + elements[2] * temp[8] + elements[3] * temp[12];
			determinant = 1.0f / determinant;

			for (int32 i = 0; i < 3 * 3; i++)
				elements[i] = temp[i] * determinant;

			return *this;
		}

		vec3 mat3::GetColumn(int index) const
		{
			return vec3(elements[index + 0 * 3], elements[index + 1 * 3], elements[index + 2 * 3]);
		}

		void mat3::SetColumn(uint index, const vec3& column)
		{
			elements[index + 0 * 3] = column.x;
			elements[index + 1 * 3] = column.y;
			elements[index + 2 * 3] = column.z;
		}

		mat3 mat3::Translate(const vec3& translation)
		{
			mat3 result = mat3::Identity();
			//result.SetPosition(translation);
			return result;
		}

		mat3 mat3::Rotate(float angle, const vec3& axis)
		{
			mat3 result(1.0f);

			float r = toRadians(angle);
			float c = cos(r);
			float s = sin(r);
			float omc = 1.0f - c;

			float x = axis.x;
			float y = axis.y;
			float z = axis.z;

			result.elements[0 + 0 * 3] = x * x * omc + c;
			result.elements[0 + 1 * 3] = y * x * omc + z * s;
			result.elements[0 + 2 * 3] = x * z * omc - y * s;

			result.elements[1 + 0 * 3] = x * y * omc - z * s;
			result.elements[1 + 1 * 3] = y * y * omc + c;
			result.elements[1 + 2 * 3] = y * z * omc + x * s;

			result.elements[2 + 0 * 3] = x * z * omc + y * s;
			result.elements[2 + 1 * 3] = y * z * omc - x * s;
			result.elements[2 + 2 * 3] = z * z * omc + c;

			return result;
		}

		mat3 mat3::Rotate(const Quaternion& quat)
		{
			mat3 result = Identity();

			float qx, qy, qz, qw, qx2, qy2, qz2, qxqx2, qyqy2, qzqz2, qxqy2, qyqz2, qzqw2, qxqz2, qyqw2, qxqw2;
			qx = quat.x;
			qy = quat.y;
			qz = quat.z;
			qw = quat.w;
			qx2 = (qx + qx);
			qy2 = (qy + qy);
			qz2 = (qz + qz);
			qxqx2 = (qx * qx2);
			qxqy2 = (qx * qy2);
			qxqz2 = (qx * qz2);
			qxqw2 = (qw * qx2);
			qyqy2 = (qy * qy2);
			qyqz2 = (qy * qz2);
			qyqw2 = (qw * qy2);
			qzqz2 = (qz * qz2);
			qzqw2 = (qw * qz2);

			result.rows[0] = vec4(((1.0f - qyqy2) - qzqz2), (qxqy2 - qzqw2), (qxqz2 + qyqw2), 0.0f);
			result.rows[1] = vec4((qxqy2 + qzqw2), ((1.0f - qxqx2) - qzqz2), (qyqz2 - qxqw2), 0.0f);
			result.rows[2] = vec4((qxqz2 - qyqw2), (qyqz2 + qxqw2), ((1.0f - qxqx2) - qyqy2), 0.0f);
			return result;
		}

		mat3 mat3::Scale(const vec3& scale)
		{
			mat3 result(1.0f);

			result.elements[0 + 0 * 3] = scale.x;
			result.elements[1 + 1 * 3] = scale.y;
			result.elements[2 + 2 * 3] = scale.z;

			return result;
		}

		mat3 mat3::Invert(const mat3& matrix)
		{
			mat3 result = matrix;
			return result.Invert();
		}

		mat3 mat3::Transpose(const mat3& matrix)
		{
			return mat3(
				vec3(matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x),
				vec3(matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y),
				vec3(matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z)
			);
		}

		String mat3::ToString() const
		{
			std::stringstream result;
			result << "mat3: (" << rows[0].x << ", " << rows[1].x << ", " << rows[2].x << "), ";
			result << "(" << rows[0].y << ", " << rows[1].y << ", " << rows[2].y << "), ";
			result << "(" << rows[0].z << ", " << rows[1].z << ", " << rows[2].z <<  ")";
			return result.str();
		}

	}
}