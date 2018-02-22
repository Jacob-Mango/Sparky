#pragma once

#include "sp/Common.h"
#include "sp/graphics/API/Framebuffer.h"
#include "sp/graphics/shaders/ShaderManager.h"
#include "sp/graphics/material/Material.h"

#include "sp/graphics/API/Framebuffer.h"
#include "sp/graphics/API/VertexArray.h"
#include "sp/graphics/API/IndexBuffer.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			class SP_API PostEffectsPass
			{
			private:
				Material * m_Material;

				API::VertexArray* m_VertexArray;
				API::IndexBuffer* m_IndexBuffer;
			public:
				PostEffectsPass(API::Shader* shader);
				~PostEffectsPass();

				virtual void PreRenderPass(API::Framebuffer* source, API::Framebuffer* target) { target = source; };
				virtual void PostRenderPass(API::Framebuffer* source, API::Framebuffer* target);

				inline Material* GetMaterial() { return m_Material; }
			};

		}
	}
}