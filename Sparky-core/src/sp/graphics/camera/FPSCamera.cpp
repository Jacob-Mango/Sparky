#include "sp/sp.h"
#include "FPSCamera.h"

#include "sp/app/Input.h"
#include "sp/utils/Log.h"

#include "sp/app/Application.h"
#include "sp/debug/DebugMenu.h"

namespace sp {
	namespace graphics {

		using namespace maths;

		float Yaw;
		float Pitch;

		bool ShouldHaveFocus = false;

		FPSCamera::FPSCamera(const maths::mat4& projectionMatrix)
			: Camera(projectionMatrix), m_MouseSensitivity(1.0f), m_Speed(50.0f), m_SprintSpeed(500.0f), m_MouseWasGrabbed(false)
		{
			debug::DebugMenu::Add("Camera/FPS Camera Speed", &m_Speed);
			debug::DebugMenu::Add("Camera/FPS Camera Sprint Speed", &m_SprintSpeed);
		}

		FPSCamera::~FPSCamera()
		{
			debug::DebugMenu::Remove("Camera/FPS Camera Speed");
			debug::DebugMenu::Remove("Camera/FPS Camera Sprint Speed");
		}

		void FPSCamera::OnFocus()
		{
			if (ShouldHaveFocus)
				Input::GetInputManager()->SetMouseCursor(SP_NO_CURSOR);
		}

		void FPSCamera::OnUpdate(const Timestep& ts)
		{
			vec2 windowSize = Application::GetApplication().GetWindowSize();
			vec2 windowCenter = vec2((float)(int32)(windowSize.x / 2.0f), (float)(int32)(windowSize.y / 2.0f));

			if (Input::IsMouseButtonPressed(SP_MOUSE_RIGHT))
			{
				ShouldHaveFocus = true;
				if (!Input::GetInputManager()->IsMouseGrabbed() && !debug::DebugMenu::IsVisible() && ShouldHaveFocus)
				{
					Input::GetInputManager()->SetMouseGrabbed(true);
					Input::GetInputManager()->SetMouseCursor(SP_NO_CURSOR);
					Input::GetInputManager()->SetMousePosition(windowCenter);
				}
			}
			else if (Input::IsKeyPressed(SP_KEY_ESCAPE))
			{
				Input::GetInputManager()->SetMouseGrabbed(false);
				Input::GetInputManager()->SetMouseCursor(1);
			}

			if (Input::GetInputManager()->IsMouseGrabbed() && ShouldHaveFocus)
			{
				vec2 mouse = Input::GetInputManager()->GetMousePosition();
				mouse.x -= windowCenter.x;
				mouse.y -= windowCenter.y;

				Yaw += mouse.x * m_MouseSensitivity * ts.GetSeconds();
				Pitch += mouse.y * m_MouseSensitivity * ts.GetSeconds();
				m_Rotation = GetOrientation();

				Input::GetInputManager()->SetMousePosition(windowCenter);

				Quaternion rotationYaw = Quaternion::RotationY(-Yaw);

				vec3 forward = GetForwardDirection(rotationYaw);
				vec3 right = GetRightDirection(rotationYaw);
				vec3 up = vec3::YAxis();
				float speed = (Input::IsKeyPressed(SP_KEY_SHIFT) ? m_SprintSpeed : m_Speed) * ts.GetSeconds();

				if (Input::IsKeyPressed(SP_KEY_W))
					m_Position += forward * speed;
				else if (Input::IsKeyPressed(SP_KEY_S))
					m_Position -= forward * speed;

				if (Input::IsKeyPressed(SP_KEY_A))
					m_Position -= right * speed;
				else if (Input::IsKeyPressed(SP_KEY_D))
					m_Position += right * speed;

				if (Input::IsKeyPressed(SP_KEY_SPACE))
					m_Position += up * speed;
				else if (Input::IsKeyPressed(SP_KEY_CONTROL))
					m_Position -= up * speed;

				SetViewMatrix();
			}

		}

		Quaternion FPSCamera::GetOrientation() const
		{
			return Quaternion::RotationY(-Yaw) * Quaternion::RotationX(-Pitch);
		}

		vec3 FPSCamera::GetForwardDirection(const Quaternion& orientation) const
		{
			return Quaternion::Rotate(orientation, -vec3::ZAxis());
		}

		vec3 FPSCamera::GetUpDirection(const Quaternion& orientation) const
		{
			return Quaternion::Rotate(orientation, vec3::YAxis());
		}

		vec3 FPSCamera::GetRightDirection(const Quaternion& orientation) const
		{
			return Quaternion::Rotate(orientation, vec3::XAxis());
		}

	}
}