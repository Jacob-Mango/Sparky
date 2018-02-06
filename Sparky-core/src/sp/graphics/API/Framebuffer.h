#pragma once

#include "sp/Common.h"
#include "sp/Types.h"
#include "sp/maths/maths.h"
#include "Texture.h"

namespace sp { namespace graphics { namespace API {

	class Shader;

#define FB_COLOR_ATTACHMENT0
#define FB_COLOR_ATTACHMENT1
#define FB_COLOR_ATTACHMENT2
#define FB_COLOR_ATTACHMENT3
#define FB_COLOR_ATTACHMENT4
#define FB_COLOR_ATTACHMENT5
#define FB_COLOR_ATTACHMENT6
#define FB_COLOR_ATTACHMENT7

#define FB_DEPTH_ATTACHMENT
#define FB_STENCIL_ATTACHMENT
#define FB_DEPTH_STENCIL_ATTACHMENT

#define FB_FRAMEBUFFER
#define FB_READ_FRAMEBUFFER
#define FB_DRAW_FRAMEBUFFER

	class SP_API Framebuffer
	{
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Clear() = 0;

		virtual uint GetWidth() const = 0;
		virtual uint GetHeight() const = 0;

		virtual void Render(API::Shader* shader) = 0;
	protected:
		virtual void Init() {};
	};

} } }