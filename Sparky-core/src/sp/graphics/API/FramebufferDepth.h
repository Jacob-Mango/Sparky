#pragma once

#include "Framebuffer.h"

namespace sp { namespace graphics { namespace API {

	class SP_API FramebufferDepth : public Framebuffer
	{
	public:
		static FramebufferDepth* Create(uint width, uint height, uint numberTextures = 0);
	};

} } }