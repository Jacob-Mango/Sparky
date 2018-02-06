#pragma once

#include "sp/graphics/API/Framebuffer2D.h"
#include "GLTexture2D.h"
#include "GLCommon.h"
#include "GLShader.h"

namespace sp { namespace graphics { namespace API {

	class GLFramebuffer2D : public Framebuffer2D
	{
	private:
		uint m_FramebufferHandle;
		uint m_DepthbufferHandle;

		uint m_NumberTextures;

		GLuint* m_TextureHandles;

		uint m_Width, m_Height;
		maths::vec4 m_ClearColor;
	public:
		GLFramebuffer2D(uint width, uint height, uint numberTextures);
		~GLFramebuffer2D();

		void Bind() const override;
		void Unbind() const override;
		void Clear() override;

		void Render(API::Shader* shader) override;

		inline uint GetWidth() const override { return m_Width; }
		inline uint GetHeight() const override { return m_Height; }

		inline void SetClearColor(const maths::vec4& color) override { m_ClearColor = color; }
	private:
		void Init();
	};

} } }
