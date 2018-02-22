#include "sp/sp.h"
#include "PostEffects.h"

#include "sp/graphics/MeshFactory.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			PostEffects::PostEffects()
			{
				Mesh* mesh = MeshFactory::CreateQuad(maths::vec2(-1, -1), maths::vec2(2, 2), nullptr);
				m_IndexBuffer = mesh->m_IndexBuffer;
				m_VertexArray = mesh->m_VertexArray;
			}

			PostEffects::~PostEffects()
			{

			}

			void PostEffects::Push(PostEffectsPass* pass)
			{
				m_Passes.push_back(pass);
			}

			void PostEffects::Pop()
			{
				m_Passes.pop_back();
			}

			void PostEffects::RenderPass(API::Framebuffer* source, PostEffectsPass* pass) {
				Material* material = pass->GetMaterial();

				material->Bind();

				std::vector<API::Texture2D*> textures = source->GetTextures();
				material->SetTexture("u_Screen", textures[0]);
				material->SetTexture("u_Normal", textures[1]);

				float aspectX = (float)source->GetWidth() / (float)source->GetHeight();
				float aspectY = (float)source->GetHeight() / (float)source->GetWidth();
				if (aspectX < 1.0f) aspectX = 1.0f;
				if (aspectY < 1.0f) aspectY = 1.0f;

				aspectX *= 0.5f;
				aspectY *= 0.5f;

				material->SetUniform("u_ProjectionMatrix", maths::mat4::Orthographic(-aspectX, aspectX, aspectY, -aspectY, -10.0f, 10.0f));
				material->SetUniform("u_ModelMatrix", maths::mat4::Scale(maths::vec3(aspectX, -aspectY)));

				m_VertexArray->Bind();
				m_IndexBuffer->Bind();
				m_VertexArray->Draw(RenderType::TRIANGLES, 6);
				m_IndexBuffer->Unbind();
				m_VertexArray->Unbind();

				material->Unbind();
			}

			void PostEffects::Render(API::Framebuffer* source) {
				int i = 0;
				for (i = 0; i < m_Passes.size() - 1; i++) {
					source->Bind();
					RenderPass(source, m_Passes[i]);
					source->Unbind();
				}

				RenderPass(source, m_Passes[i]);
			}
		}

	}
}