#pragma once

#include <vector>

#include <sp/Types.h>
#include <sp/scene/Object.h>

#include "sp/graphics/camera/Camera.h"
#include "sp/graphics/LightSetup.h"


namespace sp {
	namespace graphics {
		class Renderer3D;
	}

	namespace scene {

		class SP_API Scene
		{
		private:
			std::vector<Object*> m_Objects;
			graphics::Camera* m_Camera;
			std::vector<graphics::LightSetup*> m_LightSetupStack;
		public:
			Scene();
			Scene(graphics::Camera* camera);
			~Scene();

			void Add(Object* entity);
			void PushLightSetup(graphics::LightSetup* lightSetup);
			graphics::LightSetup* PopLightSetup();
			void SetCamera(graphics::Camera* camera);

			void Init();

			void OnUpdate(const Timestep& ts);
			void OnRender(graphics::Renderer3D& renderer);

			inline graphics::Camera* GetCamera() const { return m_Camera; }
			inline const std::vector<Object*>& GetEntities() const { return m_Objects; }
		};

	}
}
