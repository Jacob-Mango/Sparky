#pragma once

#include "sp/Common.h"
#include "sp/app/Window.h"

namespace sp {
	namespace graphics {
		namespace API {

			enum class SP_API RenderAPI
			{
				NONE, OPENGL, DIRECT3D
			};

			enum class SP_API RendererType {
				DEFERRED, FORWARD
			};

			class SP_API Context
			{
			protected:
				static Context* s_Context;
				static RenderAPI s_RenderAPI;
				static RendererType s_RendererType;
			public:
				static void Create(WindowProperties* properties, void* deviceContext);

				static RenderAPI GetRenderAPI() { return s_RenderAPI; }
				static void SetRenderAPI(RenderAPI api) { s_RenderAPI = api; }

				static RendererType GetRendererType() { return s_RendererType; }

				static String GetRendererString() {
					switch (s_RendererType) {
					case RendererType::FORWARD: return "FORWARD";
					case RendererType::DEFERRED: return "DEFERRED";
					default: return "Unknown";
					}
				}

				static void SetRendererType(RendererType type) { s_RendererType = type; }
			};

		}
	}
}