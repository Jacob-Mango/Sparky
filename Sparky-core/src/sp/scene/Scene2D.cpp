#pragma once

#include "sp/sp.h"
#include "Scene2D.h"

#include "sp/app/Application.h"
#include "sp/debug/DebugRenderer.h"

namespace sp { namespace scene {

	using namespace graphics;
	using namespace maths;

	using namespace component;

	Scene2D::Scene2D()
	{
		float width = Application::GetApplication().GetWindowWidth();
		float height = Application::GetApplication().GetWindowHeight();
		float aspect = width / height;

		m_Camera = spnew OrthographicCamera(mat4::Orthographic(-10.0f * aspect, 10.0f * aspect, -10.0f, 10.0f, -1.0f, 1.0f));
		m_Renderer = spnew Renderer2D(width, height);
		m_Renderer->SetCamera(m_Camera);
	}

	Scene2D::Scene2D(const mat4& projectionMatrix)
		: m_Camera(spnew OrthographicCamera(projectionMatrix))
	{
		float width = Application::GetApplication().GetWindowWidth();
		float height = Application::GetApplication().GetWindowHeight();

		m_Renderer = spnew Renderer2D(width, height);
		m_Renderer->SetCamera(m_Camera);
	}

	Scene2D::~Scene2D()
	{
		spdel m_Camera;
	}

	void Scene2D::Add(Object* object)
	{
		m_Objects.push_back(object);
	}

	void Scene2D::OnUpdate(const Timestep& ts)
	{
		// for (uint i = 0; i < m_Entities.size(); i++)
		// 	m_Entities[i]

		m_Camera->OnUpdate(ts);
	}

	void Scene2D::OnRender(Renderer2D& renderer)
	{

	}

	void Scene2D::OnRender()
	{
		debug::DebugRenderer::SetCamera(m_Camera);

		m_Renderer->Begin();
		for (uint i = 0; i < m_Objects.size(); i++)
		{
			Object* object = m_Objects[i];
			SpriteComponent* sComp = object->GetComponent<SpriteComponent>();
			if (sComp)
			{
				sComp->sprite->Submit(m_Renderer);
			}

		}
		OnRender(*m_Renderer);

		m_Renderer->End();
		m_Renderer->Present();
	}

} }