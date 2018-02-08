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

RenderLayer::RenderLayer() : Layer3D(spnew Scene(), spnew ForwardRenderer())
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

	light = spnew Light(vec3(0.0f, 5.0f, 0.0f), 1.0f);
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

	scene->Add(spnew Plane(vec3(0, -1, 0), water));

	scene->Add(spnew Ball(vec3(0, 0, -10), bricks));

	for (int i = 0; i < 1; i++) {
		int max = 25;
		decimal x = (rand() % (max * 2)) - (max);
		decimal z = (rand() % (max * 2)) - (max);

		scene->Add(spnew Ball(vec3(x, 0, z), bricks));
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
				ShaderManager::Reload("PBRDeferred");
				ShaderManager::Reload("DefaultShader");
				ShaderManager::Reload("WaterShader");
				break;
			}
		}
	}
	Layer::OnEvent(event);
}