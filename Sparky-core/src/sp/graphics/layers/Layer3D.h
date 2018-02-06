#pragma once

#include "sp/Common.h"

#include "Layer.h"
#include "sp/world/World.h"

#include "../ForwardRenderer.h"
#include "../DeferredRenderer.h"

namespace sp { namespace graphics {

	class SP_API Layer3D : public Layer
	{
	protected:
		world::World* m_Scene;
		Renderer3D* m_Renderer;
	public:
		Layer3D(world::World* scene, Renderer3D* renderer = spnew DeferredRenderer());
		~Layer3D();

		virtual void Init();
		virtual void OnInit(Renderer3D* renderer, world::World* scene);

		inline world::World* GetScene() const { return m_Scene; }

		void OnUpdateInternal(const Timestep& ts) override;
		void OnRender() override;
		virtual void OnRender(Renderer3D& renderer);
	protected:
		virtual bool OnResize(uint width, uint height) override;
	};

} }
