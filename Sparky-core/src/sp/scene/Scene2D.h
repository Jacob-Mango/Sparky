#pragma once

#include "sp/sp.h"
#include "sp/graphics/camera/OrthographicCamera.h"
#include "sp/graphics/Renderer2D.h"
#include "sp/scene/Object.h"

namespace sp { namespace scene {
 
	class SP_API Scene2D
	{
	private:
		graphics::OrthographicCamera* m_Camera;
		graphics::Renderer2D* m_Renderer;

		std::vector<scene::Object*> m_Objects;
	public:
		Scene2D();
		Scene2D(const maths::mat4& projectionMatrix);
		~Scene2D();

		void Add(scene::Object* object);

		virtual void OnUpdate(const Timestep& ts);
		virtual void OnRender(graphics::Renderer2D& renderer);

		void OnRender();

		inline graphics::Renderer2D* GetRenderer() { return m_Renderer; }
		inline graphics::OrthographicCamera* GetCamera() { return m_Camera; }
	};

} }