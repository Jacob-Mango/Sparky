#include "Test3D.h"

#include "Terrain/TerrainNode.h"

using namespace sp;
using namespace graphics;
using namespace maths;

using namespace events;
using namespace scene;
using namespace component;

using namespace API;

#define DEBUG_MENU(name, var, min, max) debug::DebugMenu::Add("Test3D/" ## name, var, min, max)

Test3D::Test3D()
	: Layer3D(spnew Scene())
{
	m_FPSCamera = spnew FPSCamera(maths::mat4::Perspective(65.0f, 16.0f / 9.0f, 0.1f, 1000.0f));

	m_Rotation = 0.0f;
	m_SetUniforms[0] = true;
	m_SetUniforms[1] = true;

	mat4 result = mat4::Identity() * mat4::Identity();
}

Test3D::~Test3D()
{
}

void Test3D::OnInit(Renderer3D* renderer, Scene* scene)
{
	// Enable this to use FPS camera
	scene->SetCamera(m_FPSCamera);

	Timer timer;

	ShaderManager::Add(Shader::CreateFromFile(DEFAULT_SHADER_NAME, String("/shaders/Default/Default.shader")));
	ShaderManager::Add(Shader::CreateFromFile("Animated", String("/shaders/Animated/Animated.shader")));

	PBRMaterial* castIron = spnew PBRMaterial("CastIron");
	PBRMaterial* absRed = spnew PBRMaterial("ABSRed");
	PBRMaterial* custom = spnew PBRMaterial("Custom");

	ShaderManager::Add(Shader::CreateFromFile("Terrain", String("/shaders/Terrain/Terrain.shader")));
	MaterialManager::Add(spnew PBRMaterial("Terrain", "Terrain"));

	Model* sphereModel = spnew Model("/models/Sphere/Sphere.spm");

	float spaceZ = 20.0f;
	float spaceX = 20.0f;
	float z = 0;

	// Plastics
	for (int x = -6; x < 5; x++)
	{
		float xx = (x + 1) * spaceX;

		float roughness = (x + 6) / 10.0f;
		vec3 spec(0.04f);
		vec4 diffuse(vec3(1.0f, 0.0f, 0.0f), 1.0f);

		PBRMaterialInstance* m = spnew PBRMaterialInstance(absRed);
		m->SetMetallic(vec3(roughness, 0.04f, 0.0f));

		Mesh* mesh = spnew Mesh(sphereModel->GetMesh());
		mesh->SetMaterial(m);
		m->SetMetallic(vec3(roughness, 0.04f, 0.0f));

		maths::mat4 transform = maths::mat4(1.0f);
		transform.SetPosition(vec3(xx, 10.0f, -spaceZ + z));
		transform *= mat4::Scale(vec3(3, 3, 3));
		Object* sphere = spnew Object(mesh, transform);

		m_Spheres.push_back(sphere);
		m_Scene->Add(sphere);
	}

	// Metals
	for (int x = -6; x < 5; x++)
	{
		float xx = (x + 1) * spaceX;

		float roughness = (x + 6) / 10.0f;
		vec4 diffuse(0.0f, 0.0f, 0.0f, 1.0f);

		PBRMaterialInstance* m = spnew PBRMaterialInstance(castIron);
		m->SetMetallic(vec3(roughness, 1.0f, 0.0f));

		Mesh* mesh = spnew Mesh(sphereModel->GetMesh());
		mesh->SetMaterial(m);
		m->SetMetallic(vec3(roughness, 1.0f, 0.0f));

		maths::mat4 transform = maths::mat4(1.0f);
		transform.SetPosition(vec3(xx, 10.0f, spaceZ + z));
		transform *= mat4::Scale(vec3(3, 3, 3));
		Object* sphere = spnew Object(mesh, transform);

		m_Spheres.push_back(sphere);
		m_Scene->Add(sphere);
	}

	float size = 64;
	float density = 1;
	int amount = 10;
	Mesh* mesh = MeshFactory::CreatePlane(size, vec3(0, 1, 0), spnew PBRMaterialInstance(custom));
	//for (int x = -amount; x < amount; x++) {
	//	for (int z = -amount; z < amount; z++) {
	//		m_Scene->Add(spnew Object(mesh, maths::mat4::Translate(vec3(x * size, 0, z * size))));
	//	}
	//}

	LightSetup* lights = spnew LightSetup();
	m_Light = spnew Light(vec3(0, 20, 0), 1.0f, vec4(0.8f, 0.8f, 0.8f, 1.0f));
	lights->Add(m_Light);
	m_Scene->PushLightSetup(lights);

	terrain::TerrainNode* terrain = spnew terrain::TerrainNode(m_FPSCamera);
	m_Scene->Add(terrain);

	ShaderManager::Add(Shader::CreateFromFile("HDR_PEP", String("/shaders/PostFX/HDR/HDR.shader")));
	m_PostEffects->Push(spnew postfx::HDRPostEffect());

	//ShaderManager::Add(Shader::CreateFromFile("FXAA_PEP", String("/shaders/PostFX/FXAA/FXAA.shader")));
	//m_PostEffects->Push(spnew postfx::FXAAPostEffect());

	SP_INFO("Init took ", timer.ElapsedMillis(), " ms");

}

void Test3D::OnTick()
{
}

void Test3D::OnUpdate(const Timestep& ts)
{
}

void Test3D::OnRender(Renderer3D& renderer)
{
	Layer3D::OnRender(renderer);
}

bool debugMenu = false;

void Test3D::OnEvent(Event& event)
{
	if (event.GetType() == Event::Type::KEY_PRESSED)
	{
		KeyPressedEvent* kpe = (KeyPressedEvent*)&event;
		if (kpe->GetRepeat() == 0)
		{
			switch (kpe->GetKeyCode())
			{
			case SP_KEY_R:
				ShaderManager::ReloadAll();
				break;
			}
		}
	}
	if (event.GetType() == Event::Type::KEY_RELEASED)
	{
		KeyPressedEvent* kpe = (KeyPressedEvent*)&event;
		if (kpe->GetRepeat() == 0)
		{
			switch (kpe->GetKeyCode())
			{
			case SP_KEY_T:
				debugMenu = !debugMenu;
				debug::DebugMenu::SetVisible(debugMenu);
				if (debugMenu) {
					Input::GetInputManager()->SetMouseGrabbed(false);
					Input::GetInputManager()->SetMouseCursor(1);
				}
				else {
					Input::GetInputManager()->SetMouseGrabbed(true);
					Input::GetInputManager()->SetMouseCursor(SP_NO_CURSOR);
				}
				break;
			}
		}
	}
	Layer::OnEvent(event);
}
