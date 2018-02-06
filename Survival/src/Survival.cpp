#include "Survival.h"
#include "RenderLayer.h"

#include <sp\debug\DebugLayer.h>

Survival::Survival()
	: Application("Survival", spnew WindowProperties { 1280, 720, false, false }, sp::graphics::API::RenderAPI::OPENGL)
{
	
}

Survival::~Survival()
{
}

void Survival::Init()
{
	Application::Init();
	VFS::Get()->Mount("engine", "engine");
	VFS::Get()->Mount("models", "data/Models");
	VFS::Get()->Mount("materials", "data/Materials");
	VFS::Get()->Mount("shaders", "data/Shaders");

	PushLayer(spnew RenderLayer());
}