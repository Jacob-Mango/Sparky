#include "sp/sp.h"
#include "Layer3D.h"


namespace sp { namespace graphics {

	Layer3D::Layer3D(world::World* scene, Renderer3D* renderer)
		: m_Scene(scene), m_Renderer(renderer)
	{
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

	void Layer3D::OnInit(Renderer3D* renderer, world::World* scene)
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