#pragma once

#include "Camera.h"

namespace sp { namespace graphics {

	class SP_API FPSCamera : public Camera
	{
	private:
		float m_MouseSensitivity;
		float m_Speed, m_SprintSpeed;
		bool m_MouseWasGrabbed;
	public:
		FPSCamera(const maths::mat4& projectionMatrix);
		~FPSCamera();
		void OnFocus() override;
		void OnUpdate(const Timestep& ts) override;
	private:
		maths::Quaternion GetOrientation() const;
		maths::vec3 GetForwardDirection(const maths::Quaternion& orientation) const;
		maths::vec3 FPSCamera::GetUpDirection(const maths::Quaternion& orientation) const;
		maths::vec3 FPSCamera::GetRightDirection(const maths::Quaternion& orientation) const;
	};

} }
