#pragma once

#include "sp/graphics/API/FramebufferDepth.h"
#include "GLTextureDepth.h"
#include "GLTexture2D.h"
#include "GLCommon.h"
#include "GLShader.h"

namespace sp { namespace graphics { namespace API {

	class GLFramebufferDepth : public FramebufferDepth
	{
	private:
		uint m_FramebufferHandle;
		uint m_DepthbufferHandle;

		uint m_NumberTextures;

		GLuint* m_TextureHandles;

		uint m_Width, m_Height;
		maths::vec4 m_ClearColor;
	public:
		GLFramebufferDepth(uint width, uint height, uint numberTextures);
		~GLFramebufferDepth();

		void Bind() const override;
		void Unbind() const override;
		void Clear() override;

		void Render(API::Shader* shader) override;

		inline uint GetWidth() const override { return m_Width; }
		inline uint GetHeight() const override { return m_Height; }
	private:
		void Init();
	};

} } }