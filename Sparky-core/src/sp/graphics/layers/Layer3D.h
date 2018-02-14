#pragma once

#include "sp/Common.h"

#include "Layer.h"

#include "sp/scene/Scene.h"

#include "sp/graphics/ForwardRenderer.h"
#include "sp/graphics/DeferredRenderer.h"

namespace sp { namespace graphics {

	class SP_API Layer3D : public Layer
	{
	protected:
		scene::Scene* m_Scene;
		Renderer3D* m_Renderer;
	public:
		Layer3D(scene::Scene* scene);
		~Layer3D();

		virtual void Init();
		virtual void OnInit(Renderer3D* renderer, scene::Scene* scene);

		inline scene::Scene* GetScene() const { return m_Scene; }

		void OnUpdateInternal(const Timestep& ts) override;
		void OnRender() override;
		virtual void OnRender(Renderer3D& renderer);
	protected:
		virtual bool OnResize(uint width, uint height) override;
	};

} }
