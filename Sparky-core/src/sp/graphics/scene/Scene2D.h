#pragma once

#include "sp/sp.h"
#include "sp/graphics/camera/OrthographicCamera.h"
#include "sp/graphics/Renderer2D.h"
#include "sp/entity/Entity.h"

namespace sp { namespace graphics {
 
	class SP_API Scene2D
	{
	private:
		graphics::OrthographicCamera* m_Camera;
		graphics::Renderer2D* m_Renderer;

		std::vector<entity::Entity*> m_Entities;
	public:
		Scene2D();
		Scene2D(const maths::mat4& projectionMatrix);
		~Scene2D();

		void Add(entity::Entity* entity);

		virtual void OnUpdate(const Timestep& ts);
		virtual void OnRender(graphics::Renderer2D& renderer);

		void OnRender();

		inline graphics::Renderer2D* GetRenderer() { return m_Renderer; }
		inline graphics::OrthographicCamera* GetCamera() { return m_Camera; }
	};

} }