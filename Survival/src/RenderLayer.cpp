#include "RenderLayer.h"

#include "GameObjects\Player.h"
#include "GameObjects\Plane.h"
#include "GameObjects\Ball.h"

using namespace sp;
using namespace graphics;
using namespace maths;

using namespace events;
using namespace entity;
using namespace component;

using namespace API;

using namespace graphics;

RenderLayer::RenderLayer() : Layer3D(spnew Scene())
{
	m_Window->SetVsync(VSYNC_DISABLED);
}

RenderLayer::~RenderLayer()
{
}

MaterialInstance* test;
MaterialInstance* bricks;
MaterialInstance* water;
Light* light;

void RenderLayer::OnInit(Renderer3D* renderer, Scene* scene)
{
	Shader* defaultShader = Shader::CreateFromFile("Default", String("/shaders/Default/Default.shader"));
	Shader* waterShader = Shader::CreateFromFile("Water", String("/shaders/Water/Water.shader"));

	ShaderManager::Add(defaultShader);
	ShaderManager::Add(waterShader);

	LightSetup* lights = spnew LightSetup();

	light = spnew Light(vec3(0.0f, 10.0f, 0.0f), 1.0f, vec4(0.1, 0.1, 0.1, 0.1));
	lights->Add(light);

	scene->PushLightSetup(lights);

	// Player* player = spnew Player(scene, mainShader);
	// scene->Add(player);

	scene->SetCamera(spnew FPSCamera(mat4::Perspective(90.0f, 16.0f / 9.0f, 0.1f, 1000.0f)));

	MaterialManager::Add(spnew PBRMaterial("Bricks_V2", defaultShader));
	MaterialManager::Add(spnew PBRMaterial("Water", waterShader));

	test = spnew MaterialInstance("Bricks_V2");
	bricks = spnew PBRMaterialInstance("Bricks_V2");
	water = spnew PBRMaterialInstance("Water");

	scene->Add(spnew Plane(vec3(0, 0, 0), water));

	int amount = 16;
	int multiply = 32;
	for (int x = -amount; x < amount; x++) {
		for (int z = -amount; z < amount; z++) {
			scene->Add(spnew Ball(vec3(x * multiply, 5, z * multiply), bricks));
		}
	}
}

void RenderLayer::OnTick()
{
	Application& app = Application::GetApplication();
}

void RenderLayer::OnUpdate(const Timestep& ts)
{
	water->SetUniform("u_Time", ts.GetElapsedSeconds());
}

void RenderLayer::OnRender(Renderer3D& renderer)
{
	Layer3D::OnRender(renderer);
}

void RenderLayer::OnEvent(Event& event)
{
	if (event.GetType() == Event::Type::KEY_PRESSED)
	{
		KeyPressedEvent* kpe = (KeyPressedEvent*)&event;
		if (kpe->GetRepeat() == 0)
		{
			switch (kpe->GetKeyCode())
			{
			case SP_KEY_R:
				ShaderManager::Reload("Deferred");
				ShaderManager::Reload("Default");
				ShaderManager::Reload("Water");
				break;
			}
		}
	}
	Layer::OnEvent(event);
}