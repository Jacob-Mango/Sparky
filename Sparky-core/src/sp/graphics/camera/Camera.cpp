#include "sp/sp.h"
#include "Camera.h"

#include "sp/debug/DebugMenu.h"

namespace sp { namespace graphics {

	Camera::Camera(const maths::mat4& projectionMatrix)
		: m_ProjectionMatrix(projectionMatrix)
	{
		m_ViewMatrix = maths::mat4::Identity();
	}

	void Camera::SetRotation(maths::Quaternion& rotation)
	{
		m_Rotation = rotation;
	}

	void Camera::SetFirstPersonViewMatrix() {
		SetViewMatrix();
	}

	void Camera::SetViewMatrix() {
		using namespace maths;
			
		vec3 forward = Quaternion::Rotate(m_Rotation, -vec3::ZAxis());;
		m_ViewMatrix = mat4::LookAt(m_Position, m_Position + forward, vec3::Up());

		SP_INFO(m_ViewMatrix.ToString());

	}
} }