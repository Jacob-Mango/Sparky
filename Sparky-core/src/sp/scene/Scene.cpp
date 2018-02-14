#include "sp/sp.h"
#include "Scene.h"

#include "sp/graphics/Renderer3D.h"

#include "sp/graphics/camera/FPSCamera.h"

#include "sp/debug/DebugRenderer.h"

namespace sp {
	namespace scene {

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

			m_Objects.clear();
			spdel m_Camera;
		}

		void Scene::Init()
		{
		}

		void Scene::Add(Object* object)
		{
			object->Init();
			m_Objects.push_back(object);
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

			for (Object* object : m_Objects)
			{
				object->Update(ts);

				for (auto comp : object->GetComponents()) {
					Component* component = comp.second;
					if (StringEquals(component->GetType()->name, CameraComponent::GetStaticType()->name))
						cc = (CameraComponent*)component;
					else
						component->OnUpdate(ts);
				}
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

			mat4* temp = new mat4[NUMBONES];
			for (int i = 0; i < NUMBONES; i++)
				temp[i] = mat4::Identity();

			for (Object* object : m_Objects)
			{
				MeshComponent* mesh = object->GetComponent<MeshComponent>();
				if (mesh)
				{
					AnimationComponent* ac = object->GetComponent<AnimationComponent>();

					graphics::Bone* rootBone = mesh->mesh->m_RootBone;
					std::vector<maths::mat4> jointMatrix;

					if (ac) jointMatrix.insert(jointMatrix.begin(), ac->joints, ac->joints + sizeof(ac->joints) / sizeof(ac->joints[0]));
					else jointMatrix.insert(jointMatrix.begin(), temp, temp + sizeof(temp) / sizeof(temp[0]));

					renderer.SubmitMesh(mesh->mesh, object->GetTransform(), jointMatrix.data(), rootBone);
				}
			}
			renderer.EndScene();
			renderer.End();
		}
	}
}