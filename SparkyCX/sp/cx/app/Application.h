#pragma once

#include <sp\app\Application.h>

#include "../SparkyCX.h"

namespace sp { namespace cx {

	public ref class Application : public ManagedClass<sp::Application>
	{
	internal:
		Application(sp::Application* instance);
	public:
		Application(System::String^ name, WindowProperties properties);
		Application(System::String^ name, WindowProperties properties, sp::graphics::API::RenderAPI api);

		System::String^ GetBuildConfiguration();
		System::String^ GetPlatform();
	};

} }