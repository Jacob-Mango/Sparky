#pragma once

#include "Framebuffer.h"

namespace sp { namespace graphics { namespace API {

	class SP_API FrameBufferDepth : public Framebuffer
	{
	public:
		static FrameBufferDepth* Create(uint width, uint height, std::vector<TextureParameters> parameters);
	};

} } }