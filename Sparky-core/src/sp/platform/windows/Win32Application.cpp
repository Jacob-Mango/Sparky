#include "sp/sp.h"
#include "sp/app/Application.h"

#include <iostream>
#include <conio.h>
#include <windows.h>

#include "sp\utils\Log.h"
#include "sp/maths/maths.h"

namespace sp {

	Application::Application(const String& name)
		: m_Name(name), m_Properties(nullptr), m_Frametime(0.0f)
	{
		s_Instance = this;
	}

	Application::Application(const String& name, WindowProperties* properties, graphics::API::RenderAPI api)
		: m_Name(name), m_Properties(properties), m_Frametime(0.0f)
	{
		s_Instance = this;
		graphics::API::Context::SetRenderAPI(api);
	}

	Application::~Application()
	{
		delete window;
	}

	void Application::PlatformInit()
	{
		if (!IsServer()) {
			window = new Window(m_Name, m_Properties);
			window->SetEventCallback(METHOD(&Application::OnEvent));
		}
	}

	void Application::Start()
	{
		Init();
		m_Running = true;
		m_Suspended = false;
		Run();
	}

	void Application::Suspend()
	{
		m_Suspended = true;
	}

	void Application::Resume()
	{
		m_Suspended = false;
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		float updateTick = 1000.0f / m_MaxUpdatesPerSecond;
		float frameTick = 1000.0f / m_MaxFramesPerSecond;

		m_Timer = new Timer();
		float timer = 0.0f; 
		float updateTimer = m_Timer->ElapsedMillis();

		uint frames = 0;
		uint updates = 0;

		Timestep timestep(m_Timer->ElapsedMillis());
		while (m_Running)
		{
			float now = m_Timer->ElapsedMillis();
			if (now - updateTimer > updateTick)
			{
				Timer updateTime;

				timestep.Update(now);
				OnUpdate(timestep);
				updates++;
				updateTimer += updateTick;

				m_UpdateTime = updateTime.ElapsedMillis();
			}

			if (!IsServer()) {
				window->Clear();

				Timer frametime;
				OnRender();
				frames++;
				m_Frametime = frametime.ElapsedMillis();

				int sleepTime = max(1, floor(frameTick - m_Frametime));
				Sleep(sleepTime);

				window->Update();

				if (window->Closed())
					m_Running = false;
			}

			if (m_Timer->Elapsed() - timer > 1.0f)
			{
				timer += 1.0f;
				m_FramesPerSecond = frames;
				m_UpdatesPerSecond = updates;
				frames = 0;
				updates = 0;
				OnTick();
				updateTick = 1000.0f / m_MaxUpdatesPerSecond;
				frameTick = 1000.0f / m_MaxFramesPerSecond;
			}
		}
	}

	String Application::GetPlatform()
	{
#if defined(SP_PLATFORM_WIN32)
		return "Win32";
#elif defined(SP_PLATFORM_WIN64)
		return "Win64";
#else
		return "Unknown Platform";
#endif
	}

}