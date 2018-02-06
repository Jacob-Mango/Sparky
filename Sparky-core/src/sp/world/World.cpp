#include "sp/sp.h"
#include "World.h"

#include "sp/graphics/Renderer3D.h"

#include "sp/graphics/camera/FPSCamera.h"

#include "sp/debug/DebugRenderer.h"

#include "terrain\HeightMap.h"

namespace sp {
	namespace world {

		using namespace entity;
		using namespace component;

		using namespace maths;

		World::World()
			: m_Camera(spnew graphics::FPSCamera(maths::mat4::Perspective(65.0f, 16.0f / 9.0f, 0.1f, 1000.0f)))
		{
			Init();
		}

		World::World(graphics::Camera* camera)
			: m_Camera(camera)
		{
			Init();
		}

		World::~World()
		{
			for (uint i = 0; i < m_Entities.size(); i++)
				spdel m_Entities[i];

			m_Entities.clear();
			spdel m_Camera;
		}

		void World::Init()
		{
		}

		void World::Add(Entity* entity)
		{
			if (entity->GetComponent<TransformComponent>() == nullptr)
				entity->AddComponent(spnew TransformComponent(maths::mat4::Identity()));

			for (Component* components : entity->GetComponents()) {
				components->SetEntity(entity);
				components->SetWorld(this);
				components->OnInit();
			}

			entity->OnInit();

			m_Entities.push_back(entity);
		}

		void World::PushLightSetup(graphics::LightSetup* lightSetup)
		{
			m_LightSetupStack.push_back(lightSetup);
		}

		graphics::LightSetup* World::PopLightSetup()
		{
			graphics::LightSetup* result = m_LightSetupStack.back();
			m_LightSetupStack.pop_back();
			return result;
		}

		void World::SetCamera(graphics::Camera* camera)
		{
			m_Camera = camera;
			m_Camera->OnFocus();
		}

		void World::OnUpdate(const Timestep& ts)
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

		void World::OnRender(graphics::Renderer3D& renderer)
		{
			debug::DebugRenderer::SetCamera(m_Camera);

			renderer.Begin();
			renderer.BeginScene(m_Camera);
			for (uint i = 0; i < m_LightSetupStack.size(); i++)
				renderer.SubmitLightSetup(*m_LightSetupStack[i]);

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

		void World::CreateHeightMap()
		{
			int numMaps = 8;
			for (int x = 0; x < numMaps; x++)
				for (int z = 0; z < numMaps; z++)
					Add(new HeightMap(vec3(x, 0, z)));
		}
	}
}