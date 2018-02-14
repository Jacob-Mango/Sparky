#include "sp/sp.h"
#include "CameraComponent.h"

#include "Components.h"

#include "../Object.h"

#include "sp\app\Application.h"

namespace sp { namespace scene { namespace component {

	CameraComponent::CameraComponent(graphics::Camera* camera)
		: camera(camera)
	{
	}

	void CameraComponent::OnUpdate(const sp::Timestep& ts)
	{
		float aspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();
		camera->SetProjectionMatrix(maths::mat4::Perspective(90.0f, aspectRatio, 0.05f, 1000.0f));
		camera->SetViewMatrix();

		SetPosition(maths::vec3(0.0f, 10.0f, 0.0f).Add(m_Parent->GetTransform().GetPosition()));
	}

	void CameraComponent::SetRotation(maths::Quaternion& rot)
	{
		camera->SetRotation(rot);
	}
	
	void CameraComponent::SetPosition(maths::vec3& pos)
	{
		camera->SetPosition(pos);
	}

} } }