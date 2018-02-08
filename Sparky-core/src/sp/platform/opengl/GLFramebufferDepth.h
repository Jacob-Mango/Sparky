#pragma once

#include "sp/graphics/API/FramebufferDepth.h"
#include "GLTextureDepth.h"
#include "GLTexture2D.h"
#include "GLCommon.h"
#include "GLShader.h"

namespace sp { namespace graphics { namespace API {

	class GLFramebufferDepth : public FrameBufferDepth
	{
	private:
		uint m_FramebufferHandle;
		uint m_DepthbufferHandle;

		std::vector<TextureParameters> m_Parameters;

		uint m_Width, m_Height;
		maths::vec4 m_ClearColor;
	public:
		GLFramebufferDepth(uint width, uint height, std::vector<TextureParameters> parameters);
		~GLFramebufferDepth();

		void Bind() const override;
		void Unbind() const override;
		void Clear() override;

		inline uint GetWidth() const override { return m_Width; }
		inline uint GetHeight() const override { return m_Height; }
	private:
		void Init();
	};

} } }