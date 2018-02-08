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

		Quaternion orientation = Quaternion::RotationY(-Yaw) * Quaternion::RotationX(-Pitch);

		vec3 forward = Quaternion::Rotate(orientation, -vec3::ZAxis());;

		forward.x = -sin(toRadians(-Yaw));
		forward.y = 0; // +cos(toRadians(Pitch + 180));
		forward.z = +cos(toRadians(-Yaw));

		vec3 position = vec3(m_Position);

		m_ViewMatrix = mat4::LookAt(position, position + forward, vec3::Up());

		SP_INFO(m_ViewMatrix.ToString());

	}
} }