#pragma once

#include "Framebuffer.h"

namespace sp { namespace graphics { namespace API {

	class SP_API Framebuffer2D : public Framebuffer
	{
	public:
		virtual void SetClearColor(const maths::vec4& color) = 0;
	public:
		static Framebuffer2D* Create(uint width, uint height, uint numberTextures = 0);
	};

} } }