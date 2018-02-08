#include "sp/sp.h"
#include "Scene.h"

#include "sp/graphics/Renderer3D.h"

#include "sp/graphics/camera/FPSCamera.h"

#include "sp/debug/DebugRenderer.h"

namespace sp {
	namespace graphics {

		using namespace entity;
		using namespace component;

		using namespace maths;

		Scene::Scene()
			: m_Camera(spnew graphics::FPSCamera(maths::mat4::Perspective(65.0f, 16.0f / 9.0f, 0.1f, 1000.0f)))
		{
			Init();
		}

		Scene::Scene(graphics::Camera* camera)
			: m_Camera(camera)
		{
			Init();
		}

		Scene::~Scene()
		{
			for (uint i = 0; i < m_Entities.size(); i++)
				spdel m_Entities[i];

			m_Entities.clear();
			spdel m_Camera;
		}

		void Scene::Init()
		{
		}

		void Scene::Add(Entity* entity)
		{
			if (entity->GetComponent<TransformComponent>() == nullptr)
				entity->AddComponent(spnew TransformComponent(maths::mat4::Identity()));

			for (Component* components : entity->GetComponents()) {
				components->SetEntity(entity);
				components->SetScene(this);
				components->OnInit();
			}

			entity->OnInit();

			m_Entities.push_back(entity);
		}

		void Scene::PushLightSetup(graphics::LightSetup* lightSetup)
		{
			m_LightSetupStack.push_back(lightSetup);
		}

		graphics::LightSetup* Scene::PopLightSetup()
		{
			graphics::LightSetup* result = m_LightSetupStack.back();
			m_LightSetupStack.pop_back();
			return result;
		}

		void Scene::SetCamera(graphics::Camera* camera)
		{
			m_Camera = camera;
			m_Camera->OnFocus();
		}

		void Scene::OnUpdate(const Timestep& ts)
		{
			CameraComponent* cc = nullptr;

			for (Entity* entity : m_Entities)
			{
				if (entity->ShouldUpdate())
					entity->OnUpdate(ts);

				for (Component* component : entity->GetComponents())
					if (StringEquals(component->GetType()->name, CameraComponent::GetStaticType()->name))
						cc = (CameraComponent*)component;
					else
						component->OnUpdate(ts);
			}

			if (cc != nullptr) cc->OnUpdate(ts);

			m_Camera->OnUpdate(ts);
		}

		void Scene::OnRender(graphics::Renderer3D& renderer)
		{
			debug::DebugRenderer::SetCamera(m_Camera);

			renderer.Begin();
			renderer.BeginScene(m_Camera);
			renderer.SubmitLightSetup(*m_LightSetupStack[0]);

			mat4* iden = spnew mat4[NUMBONES];
			for (int i = 0; i < NUMBONES; i++) iden[i] = mat4::Identity();

			for (Entity* entity : m_Entities)
			{
				MeshComponent* mesh = entity->GetComponent<MeshComponent>();
				if (mesh)
				{
					TransformComponent* tc = entity->GetComponent<TransformComponent>();
					SP_ASSERT(tc, "Mesh does not have transform!");

					AnimationComponent* ac = entity->GetComponent<AnimationComponent>();

					graphics::Bone* rootBone = mesh->mesh->m_RootBone;
					std::vector<maths::mat4> jointMatrix;

					if (ac) jointMatrix.insert(jointMatrix.begin(), ac->joints, ac->joints + sizeof(ac->joints) / sizeof(ac->joints[0]));
					else jointMatrix.insert(jointMatrix.begin(), iden, iden + sizeof(iden) / sizeof(iden[0]));

					renderer.SubmitMesh(mesh->mesh, *tc->transform, jointMatrix.data(), rootBone);
				}
			}
			renderer.EndScene();
			renderer.End();
		}
	}
}