#pragma once

#include "sp/sp.h"
#include "sp/Common.h"

#include "sp/graphics/API/Framebuffer.h"
#include "sp/graphics/API/VertexArray.h"
#include "sp/graphics/API/IndexBuffer.h"
#include "PostEffectsPass.h"

namespace sp { namespace graphics {

	// The Post Effects pipeline
	class SP_API PostEffects
	{
	private:
		std::vector<PostEffectsPass*> m_Passes;

		API::VertexArray* m_VertexArray;
		API::IndexBuffer* m_IndexBuffer;
	public:
		PostEffects();
		~PostEffects();
		void Push(PostEffectsPass* pass);
		void Pop();

		void Render(API::Framebuffer* source);
	};

} }