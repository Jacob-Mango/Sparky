#include "Test3D.h"

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

vec3 g_CubeTransform(-10, 10, 0);
vec3 g_DaggerTransform(0, 20, 0);
vec4 g_SphereColor(0.0f, 0.0f, 0.0f, 1.0f);
vec3 g_SphereSpecularColor(1.0f, 1.0f, 0.6f);

float g_DaggerGloss = 0.5f;

Shader* shadowPassShader;
FrameBufferDepth* g_DepthBuffer;
TextureDepth* g_ShadowMap;

void Test3D::OnInit(Renderer3D* renderer, Scene* scene)
{
	// Enable this to use FPS camera
	scene->SetCamera(m_FPSCamera);

	Timer timer;
	String environmentFiles[11] =
	{
		"/materials/cubemap/CubeMap0.tga",
		"/materials/cubemap/CubeMap1.tga",
		"/materials/cubemap/CubeMap2.tga",
		"/materials/cubemap/CubeMap3.tga",
		"/materials/cubemap/CubeMap4.tga",
		"/materials/cubemap/CubeMap5.tga",
		"/materials/cubemap/CubeMap6.tga",
		"/materials/cubemap/CubeMap7.tga",
		"/materials/cubemap/CubeMap8.tga",
		"/materials/cubemap/CubeMap9.tga",
		"/materials/cubemap/CubeMap10.tga"
	};

	TextureCube* environment = TextureCube::CreateFromVCross(environmentFiles, 11);

	Shader* pbrShader = Shader::CreateFromFile("Default", String("/shaders/Default/Default.shader"));
	ShaderManager::Add(pbrShader);

	PBRMaterial* castIron = spnew PBRMaterial("CastIron", pbrShader);
	castIron->SetEnviromentMap(environment);

	PBRMaterial* absRed = spnew PBRMaterial("ABSRed", pbrShader);
	absRed->SetEnviromentMap(environment);

	PBRMaterial* custom = spnew PBRMaterial("Custom", pbrShader);
	custom->SetEnviromentMap(environment);

	Model* sphereModel = spnew Model("/models/Sphere/Sphere.spm");

	float space = 20.0f;
	float spaceX = 20.0f;
	float z = -80;

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

		maths::mat4 transform = maths::mat4(1.0f);
		transform.SetPosition(vec3(xx, 10.0f, -space + z));
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
		m->SetMetallic(vec3(roughness, 0.1f, 0.0f));

		Mesh* mesh = spnew Mesh(sphereModel->GetMesh());
		mesh->SetMaterial(m);


		maths::mat4 transform = maths::mat4(1.0f);
		transform.SetPosition(vec3(xx, 10.0f, space + z));
		transform *= mat4::Scale(vec3(3, 3, 3));
		Object* sphere = spnew Object(mesh, transform);

		m_Spheres.push_back(sphere);
		m_Scene->Add(sphere);
	}

	float size = 64;
	float density = 1;
	int amount = 10;
	Mesh* mesh = MeshFactory::CreatePlane(size, vec3(0, 1, 0), spnew PBRMaterialInstance(custom));
	for (int x = -amount; x < amount; x++) {
		for (int z = -amount; z < amount; z++) {
			m_Scene->Add(spnew Object(mesh, maths::mat4::Translate(vec3(x * size, 0, z * size))));
		}
	}


	LightSetup* lights = spnew LightSetup();
	m_Light = spnew Light(vec3(0, 20, 0), 1.0f, vec4(0.8f, 0.8f, 0.8f, 1.0f));
	lights->Add(m_Light);
	m_Scene->PushLightSetup(lights);

	Shader* test_PEP = Shader::CreateFromFile("Test_PEP", String("/shaders/PostFX/Test/Test.shader"));
	ShaderManager::Add(test_PEP);
	m_PostEffects->Push(spnew PostEffectsPass(test_PEP));

	Shader* HDR_PEP = Shader::CreateFromFile("HDR_PEP", String("/shaders/PostFX/HDR/HDR.shader"));
	ShaderManager::Add(HDR_PEP);
	m_PostEffects->Push(spnew PostEffectsPass(HDR_PEP));

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
