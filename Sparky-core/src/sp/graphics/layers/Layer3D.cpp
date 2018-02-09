#include "sp/sp.h"
#include "Layer3D.h"
#include "../shaders/Shader.h"

#include "../API/Context.h"

namespace sp { namespace graphics {

	Layer3D::Layer3D(graphics::Scene* scene)
		: m_Scene(scene)
	{
		switch (graphics::API::Context::GetRendererType()) {
		default:
		case graphics::API::RendererType::FORWARD:
			m_Renderer = new ForwardRenderer();
			break;
		case graphics::API::RendererType::DEFERRED:
			m_Renderer = new DeferredRenderer();
			break;
		}
	}

	Layer3D::~Layer3D()
	{
		delete m_Scene;
		delete m_Renderer;
	}

	void Layer3D::Init()
	{
		OnInit(m_Renderer, m_Scene);
		m_Renderer->Init();
	}

	void Layer3D::OnInit(Renderer3D* renderer, graphics::Scene* scene)
	{
	}

	bool Layer3D::OnResize(uint width, uint height)
	{
		m_Renderer->SetScreenBufferSize(width, height);
		return false;
	}

	void Layer3D::OnUpdateInternal(const Timestep& ts)
	{
		m_Scene->OnUpdate(ts);
		OnUpdate(ts);
	}

	void Layer3D::OnRender()
	{
		OnRender(*m_Renderer);
	}

	void Layer3D::OnRender(Renderer3D& renderer)
	{
		m_Scene->OnRender(renderer);
		renderer.Present();
	}

} }