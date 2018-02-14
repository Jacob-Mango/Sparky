#include <Sparky.h>

#include "Test3D.h"

using namespace sp;
using namespace graphics;
using namespace maths;

class Game : public Application
{
public:
	Game()
		: Application("Sandbox", spnew WindowProperties { 1280, 720, false, false }, API::RenderAPI::OPENGL, API::RendererType::DEFERRED)
	{
	}

	~Game()
	{
	}

	void Init() override
	{
		Application::Init();

		VFS::Get()->Mount("engine", "data/Engine");
		VFS::Get()->Mount("models", "data/Models");
		VFS::Get()->Mount("materials", "data/Materials");
		VFS::Get()->Mount("shaders", "data/Shaders");

		PushLayer(spnew Test3D());
	}
};

int main()
{
	Game game;
	game.Start();
	return 0;
}