#include "sp/sp.h"
#include "PostEffectsPass.h"

#include "sp/system/Memory.h"

#include "sp/graphics/MeshFactory.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			PostEffectsPass::PostEffectsPass(API::Shader* shader)
			{
				m_Material = spnew Material("mat_" + shader->GetName(), shader);

				Mesh* mesh = MeshFactory::CreateQuad(maths::vec2(-1, -1), maths::vec2(2, 2), nullptr);
				m_IndexBuffer = mesh->m_IndexBuffer;
				m_VertexArray = mesh->m_VertexArray;
			}

			PostEffectsPass::~PostEffectsPass()
			{
				spdel m_Material;
			}

			void PostEffectsPass::PostRenderPass(API::Framebuffer* source, API::Framebuffer* target)
			{
				//if (target != nullptr) source->Bind();

				m_Material->Bind();

				std::vector<API::Texture2D*> textures = source->GetTextures();
				m_Material->SetTexture("u_Screen", textures[0]);
				m_Material->SetTexture("u_Normal", textures[1]);

				float aspectX = (float)source->GetWidth() / (float)source->GetHeight();
				float aspectY = (float)source->GetHeight() / (float)source->GetWidth();
				if (aspectX < 1.0f) aspectX = 1.0f;
				if (aspectY < 1.0f) aspectY = 1.0f;

				aspectX *= 0.5f;
				aspectY *= 0.5f;

				m_Material->SetUniform("u_ProjectionMatrix", maths::mat4::Orthographic(-aspectX, aspectX, aspectY, -aspectY, -10.0f, 10.0f));
				m_Material->SetUniform("u_ModelMatrix", maths::mat4::Scale(maths::vec3(aspectX, -aspectY)));

				m_VertexArray->Bind();
				m_IndexBuffer->Bind();
				m_VertexArray->Draw(RenderType::TRIANGLES, 6);
				m_IndexBuffer->Unbind();
				m_VertexArray->Unbind();

				m_Material->Unbind();

				//if (target != nullptr) source->Unbind();
			}


		}
	}
}