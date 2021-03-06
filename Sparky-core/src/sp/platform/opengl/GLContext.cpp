#include "sp/sp.h"
#include "GLContext.h"

#undef NOGDI
#include <Windows.h>
#define NOGDI
#include "sp/utils/Log.h"

#include <GL/glew.h>
#include <GL/wglew.h>

namespace sp { namespace graphics { namespace API {

	static HDC hDc;

	GLContext::GLContext(WindowProperties* properties, void* deviceContext)
	{
		m_Properties = properties;

		hDc = GetDC((HWND)deviceContext);
		HGLRC hrc = wglCreateContext(hDc);
		if (hrc)
		{
			if (!wglMakeCurrent(hDc, hrc))
			{
				SP_ERROR("Failed setting OpenGL context!");
				SP_ASSERT(false);
			}
		}
		else
		{
			SP_ERROR("Failed creating OpenGL context!");
			SP_ASSERT(false);
		}

		if (glewInit() != GLEW_OK)
		{
			SP_FATAL("Could not initialize GLEW!");
			SP_ASSERT(false);
		}

	}

	void GLContext::Present()
	{
		wglSwapIntervalEXT(m_Properties->vsync);
		SwapBuffers(hDc);
	}

} } }