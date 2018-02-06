#include "sp/sp.h"
#include "OrthographicCamera.h"
#include "sp/app/Application.h"

namespace sp { namespace graphics {
 
	OrthographicCamera::OrthographicCamera(const maths::mat4& projectionMatrix)
		: Camera(projectionMatrix)
	{

	}

	OrthographicCamera::~OrthographicCamera()
	{

	}

	void OrthographicCamera::OnFocus()
	{

	}

	void OrthographicCamera::OnUpdate(const Timestep& ts)
	{
		float width = Application::GetApplication().GetWindowWidth();
		float height = Application::GetApplication().GetWindowHeight();

		float aspectX = (float)width / (float)height;
		float aspectY = (float)height / (float)width;
		if (aspectX < 1.0f) aspectX = 1.0f;
		if (aspectY < 1.0f) aspectY = 1.0f;

		aspectX *= 9.0f;
		aspectY *= 9.0f;

		SetProjectionMatrix(maths::mat4::Orthographic(-aspectX, aspectX, aspectY, -aspectY, -1.0f, 1.0f));
	}

} }