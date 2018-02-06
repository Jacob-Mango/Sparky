#pragma once

#include "sp/sp.h"
#include "World2D.h"

#include "sp/app/Application.h"
#include "sp/debug/DebugRenderer.h"

namespace sp { namespace world {

	using namespace graphics;
	using namespace maths;

	using namespace entity;
	using namespace component;

	World2D::World2D()
	{
		float width = Application::GetApplication().GetWindowWidth();
		float height = Application::GetApplication().GetWindowHeight();
		float aspect = width / height;

		m_Camera = spnew OrthographicCamera(mat4::Orthographic(-10.0f * aspect, 10.0f * aspect, -10.0f, 10.0f, -1.0f, 1.0f));
		m_Renderer = spnew Renderer2D(width, height);
		m_Renderer->SetCamera(m_Camera);
	}

	World2D::World2D(const mat4& projectionMatrix)
		: m_Camera(spnew OrthographicCamera(projectionMatrix))
	{
		float width = Application::GetApplication().GetWindowWidth();
		float height = Application::GetApplication().GetWindowHeight();

		m_Renderer = spnew Renderer2D(width, height);
		m_Renderer->SetCamera(m_Camera);
	}

	World2D::~World2D()
	{
		spdel m_Camera;
	}

	void World2D::Add(Entity* entity)
	{
		m_Entities.push_back(entity);
	}

	void World2D::OnUpdate(const Timestep& ts)
	{
		// for (uint i = 0; i < m_Entities.size(); i++)
		// 	m_Entities[i]

		m_Camera->OnUpdate(ts);
	}

	void World2D::OnRender(Renderer2D& renderer)
	{

	}

	void World2D::OnRender()
	{
		debug::DebugRenderer::SetCamera(m_Camera);

		m_Renderer->Begin();
		for (uint i = 0; i < m_Entities.size(); i++)
		{
			Entity* entity = m_Entities[i];
			SpriteComponent* sprite = entity->GetComponent<SpriteComponent>();
			if (sprite)
			{
				TransformComponent* tc = entity->GetComponent<TransformComponent>();
				SP_ASSERT(tc, "Sprite does not have transform!"); // Sprites MUST have transforms
				sprite->sprite->Submit(m_Renderer);
			}

		}
		OnRender(*m_Renderer);

		m_Renderer->End();
		m_Renderer->Present();
	}

} }