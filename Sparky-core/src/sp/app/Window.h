#pragma once

#include "sp/sp.h"

#include "sp/audio/SoundManager.h"
#include "sp/events/Events.h"

#include "sp/graphics/FontManager.h"
#include "sp/graphics/TextureManager.h"

#include "sp/app/Input.h"

#define VSYNC_DISABLED 0
#define VSYNC_ENABLED 1

namespace sp {

	typedef std::function<void(events::Event& event)> WindowEventCallback;

	struct SP_API WindowProperties
	{
		uint width, height;
		bool fullscreen;
		uint vsync;
	};

	class SP_API Window
	{
	private:
		static std::map<void*, Window*> s_Handles;
	private:
		String m_Title;
		WindowProperties* m_Properties;
		bool m_Closed;
		void* m_Handle;

		WindowEventCallback m_EventCallback;
		InputManager* m_InputManager;
	public:
		Window(const String& name, WindowProperties* properties);
		~Window();
		void Clear() const;
		void Update();
		bool Closed() const;

		void SetTitle(const String& title);

		inline uint GetWidth() const { return m_Properties->width; }
		inline uint GetHeight() const { return m_Properties->height; }

		void SetVsync(uint syncInterval);
		inline bool IsVsync() const { return m_Properties->vsync != 0; }

		inline InputManager* GetInputManager() const { return m_InputManager; }
		 
		void SetEventCallback(const WindowEventCallback& callback);
	private:
		bool Init();
		
		bool PlatformInit();
		void PlatformUpdate();

		// Window event callbacks (TODO: more platform independent)
		friend void ResizeCallback(Window* window, int32 width, int32 height);
		friend void FocusCallback(Window* window, bool focused);
	public:
		static void RegisterWindowClass(void* handle, Window* window);
		static Window* GetWindowClass(void* handle);
	};

}
