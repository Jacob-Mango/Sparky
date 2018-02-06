#pragma once

#include <vector>

#include <sp/Types.h>
#include <sp/entity/Entity.h>

#include "sp/graphics/camera/Camera.h"
#include "sp/graphics/LightSetup.h"


namespace sp { namespace graphics {

	class Renderer3D;

} namespace world {


	class SP_API World
	{
	private:
		std::vector<entity::Entity*> m_Entities;
		graphics::Camera* m_Camera;
		std::vector<graphics::LightSetup*> m_LightSetupStack;
	public:
		World();
		World(graphics::Camera* camera);
		~World();

		void Add(entity::Entity* entity);
		void PushLightSetup(graphics::LightSetup* lightSetup);
		graphics::LightSetup* PopLightSetup();
		void SetCamera(graphics::Camera* camera);

		void Init();

		void OnUpdate(const Timestep& ts);
		void OnRender(graphics::Renderer3D& renderer);

		inline graphics::Camera* GetCamera() const { return m_Camera; }
		inline const std::vector<entity::Entity*>& GetEntities() const { return m_Entities; }

		void CreateHeightMap();
	};

} }
