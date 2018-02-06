#pragma once

#include "sp/graphics/API/FramebufferDepth.h"

namespace sp { namespace graphics { namespace API {

	class D3DFramebufferDepth : public FramebufferDepth
	{
	private:
		uint m_Width, m_Height;
	public:
		D3DFramebufferDepth(uint width, uint height, uint numTextures);
		~D3DFramebufferDepth();

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