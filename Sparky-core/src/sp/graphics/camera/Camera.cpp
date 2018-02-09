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

		//Quaternion rot = Quaternion::RotationY(Yaw) * Quaternion::RotationX(Pitch);

		//vec3 forward = Quaternion::Rotate(m_Rotation, -vec3::ZAxis());;
		//vec3 position = vec3(m_Position);

		//m_ViewMatrix = mat4::LookAt(position, position + forward, vec3::Up());


		mat4 rotation = mat4::Rotate(m_Rotation.Conjugate());
		mat4 translation = mat4::Translate(-m_Position);

		m_ViewMatrix = rotation * translation;
	}
} }