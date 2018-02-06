#include "Application.h"

namespace sp { namespace cx {

	Application::Application(System::String^ name, WindowProperties properties) {
		m_Instance = new sp::Application(StringToStdString(name), *(sp::WindowProperties*)&properties);
	}

	Application::Application(System::String^ name, WindowProperties properties, sp::graphics::API::RenderAPI api) {
		m_Instance = new sp::Application(StringToStdString(name), *(sp::WindowProperties*)&properties, api);
	}
} }