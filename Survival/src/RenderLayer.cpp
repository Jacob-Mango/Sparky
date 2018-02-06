#include "RenderLayer.h"

#include "GameObjects\Player.h"
#include "GameObjects\Plane.h"
#include "GameObjects\Ball.h"

#include <sp/world/terrain/HeightMap.h>

using namespace sp;
using namespace graphics;
using namespace maths;

using namespace events;
using namespace entity;
using namespace component;

using namespace API;

using namespace world;

RenderLayer::RenderLayer() : Layer3D(spnew World(), spnew DeferredRenderer())
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

void RenderLayer::OnInit(Renderer3D* renderer, World* world)
{
	Shader* pbrShader = Shader::CreateFromFile("PBRDeferred", String("/shaders/PBR/PBR.shader"));
	Shader* mainShader = Shader::CreateFromFile("DefaultShader", String("/shaders/GBuffer/GBuffer.shader"));
	Shader* waterShader = Shader::CreateFromFile("WaterShader", String("/shaders/Water/Water.shader"));
	Shader* testShader = Shader::CreateFromName("Test");

	ShaderManager::Add(pbrShader);
	ShaderManager::Add(mainShader);
	ShaderManager::Add(waterShader);
	ShaderManager::Add(testShader);

	LightSetup* lights = spnew LightSetup();

	light = spnew Light(vec3(0.0f, 5.0f, 0.0f), 1.0f);
	lights->Add(light);

	world->PushLightSetup(lights);

	//Player* player = spnew Player(world, defaultShader);
	//world->Add(player);

	world->SetCamera(spnew FPSCamera(mat4::Perspective(90.0f, 16.0f / 9.0f, 0.1f, 1000.0f)));

	MaterialManager::Add(spnew Material("test", testShader));
	MaterialManager::Add(spnew PBRMaterial("Bricks_V2", mainShader));
	MaterialManager::Add(spnew PBRMaterial("Water", waterShader));

	test = spnew MaterialInstance("test");
	bricks = spnew PBRMaterialInstance("test");
	water = spnew PBRMaterialInstance("Water");

	//world->Add(spnew Plane(vec3(0, -1, 0), water));

	for (int i = 0; i < 100; i++) {
		int max = 25;
		decimal x = (rand() % (max * 2)) - (max);
		decimal z = (rand() % (max * 2)) - (max);

		world->Add(spnew Ball(vec3(x, 0, z), test));
		world->Add(spnew Ball(vec3(x + 1, 0, z), bricks));
	}

	// world->CreateHeightMap();

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