#pragma once

#include "sp/graphics/API/Framebuffer2D.h"
#include "GLTexture2D.h"
#include "GLCommon.h"
#include "GLShader.h"

namespace sp { namespace graphics { namespace API {

	class GLFramebuffer2D : public FrameBuffer2D
	{
	private:
		uint m_FramebufferHandle;
		uint m_DepthbufferHandle;

		std::vector<TextureParameters> m_Parameters;

		uint m_Width, m_Height;
		maths::vec4 m_ClearColor;
	public:
		GLFramebuffer2D(uint width, uint height, std::vector<TextureParameters> parameters);
		~GLFramebuffer2D();

		void Bind() const override;
		void Unbind() const override;
		void Clear() override;

		inline uint GetWidth() const override { return m_Width; }
		inline uint GetHeight() const override { return m_Height; }

		inline void SetClearColor(const maths::vec4& color) override { m_ClearColor = color; }
	private:
		void Init();
	};

} } }
