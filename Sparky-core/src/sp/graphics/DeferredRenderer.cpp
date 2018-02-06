#include "sp/sp.h"
#include "DeferredRenderer.h"

#include "sp/app/Application.h"
#include "sp/graphics/API/Renderer.h"

#include "sp/graphics/MeshFactory.h"

#include "sp/graphics/shaders/Shader.h"
#include "sp/graphics/shaders/ShaderFactory.h"
#include "sp/graphics/shaders/ShaderManager.h"

namespace sp {
	namespace graphics {

		using namespace maths;

		enum VSSystemUniformIndices : int32
		{
			VSSystemUniformIndex_ProjectionMatrix = 0,
			VSSystemUniformIndex_ViewMatrix = 1,
			VSSystemUniformIndex_ModelMatrix = 2,
			VSSystemUniformIndex_JointMatrix = 3,
			VSSystemUniformIndex_Size
		};

		enum PSSystemUniformIndices : int32
		{
			PSSystemUniformIndex_Size
		};

		enum GSSystemUniformIndices : int32
		{
			GSSystemUniformIndex_Size
		};

		DeferredRenderer::DeferredRenderer()
		{
			SetScreenBufferSize(Application::GetApplication().GetWindowWidth(), Application::GetApplication().GetWindowHeight());
		}

		DeferredRenderer::DeferredRenderer(uint width, uint height)
		{
			SetScreenBufferSize(width, height);
		}

		void DeferredRenderer::Init()
		{
			m_FrameBuffer = API::Framebuffer2D::Create(m_ScreenBufferWidth, m_ScreenBufferHeight, 6);

			m_Shader = ShaderManager::Get("PBRDeferred");
			m_Material = spnew Material("DeferredRendering", m_Shader);

			m_CommandQueue.reserve(1000);

			m_VSSystemUniformBufferSize = sizeof(mat4) + sizeof(mat4) + sizeof(mat4) + (sizeof(mat4) * NUMBONES);
			m_VSSystemUniformBuffer = spnew byte[m_VSSystemUniformBufferSize];
			memset(m_VSSystemUniformBuffer, 0, m_VSSystemUniformBufferSize);
			m_VSSystemUniformBufferOffsets.resize(VSSystemUniformIndex_Size + 1);

			m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix] = 0;
			m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ViewMatrix] = m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix] + sizeof(mat4);
			m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ModelMatrix] = m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ViewMatrix] + sizeof(mat4);
			m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_JointMatrix] = m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ModelMatrix] + (sizeof(mat4));

			m_GSSystemUniformBufferSize = 0;
			m_GSSystemUniformBuffer = spnew byte[m_GSSystemUniformBufferSize];
			memset(m_GSSystemUniformBuffer, 0, m_GSSystemUniformBufferSize);
			m_GSSystemUniformBufferOffsets.resize(GSSystemUniformIndex_Size);

			m_PSSystemUniformBufferSize = NUMLIGHTS * sizeof(Light);
			m_PSSystemUniformBuffer = spnew byte[m_PSSystemUniformBufferSize];
			memset(m_PSSystemUniformBuffer, 0, m_PSSystemUniformBufferSize);
			m_PSSystemUniformBufferOffsets.resize(PSSystemUniformIndex_Size);

			m_Mesh = MeshFactory::CreateQuad(vec2(-1, -1), vec2(2, 2), nullptr);
			m_IndexBuffer = m_Mesh->m_IndexBuffer;
			m_VertexArray = m_Mesh->m_VertexArray;

			m_PreintegratedFG = API::Texture2D::CreateFromFile("PreintegratedFG", "engine/PreintegratedFG.bmp");
		}

		void DeferredRenderer::Begin()
		{
			Renderer::SetViewport(0, 0, m_ScreenBufferWidth, m_ScreenBufferHeight);

			m_CommandQueue.clear();
			m_SystemUniforms.clear();
		}

		void DeferredRenderer::BeginScene(Camera* camera)
		{
			memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix], &camera->GetProjectionMatrix(), sizeof(mat4));
			memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ViewMatrix], &camera->GetViewMatrix(), sizeof(mat4));

			m_Material->SetUniform("u_CameraPosition", camera->GetPosition());
		}

		void DeferredRenderer::Submit(const RenderCommand& command)
		{
			m_CommandQueue.push_back(command);
		}

		void DeferredRenderer::PerformBoneTransforms(Bone* bone, mat4 joints[NUMBONES], std::vector<mat4> &matArray) {
			mat4 concatenated_transforms = mat4::Identity();
			if (bone && bone->Parent) {
				graphics::Bone* b = bone->Parent;
				std::vector<mat4> mats;
				while (b != nullptr)
				{
					mats.push_back(joints[b->Index]);
					b = b->Parent;
				}

				for (int i = mats.size() - 1; i >= 0; i--)
					concatenated_transforms *= mats.at(i);
			}

			matArray[bone->Index] = concatenated_transforms.Multiply(joints[bone->Index]);
			for (int i = 0; i < bone->Children.size(); i++) {
				PerformBoneTransforms(bone->Children[i], joints, matArray);
			}
		}

		void DeferredRenderer::SubmitMesh(Mesh* mesh, const maths::mat4& transform, mat4 joints[NUMBONES], Bone* rootBone)
		{
			RenderCommand command;
			command.mesh = mesh;
			command.transform = transform;

			bool noRootBone = false;

			if (rootBone != nullptr)
			{
				if (StringEquals(rootBone->Name, "defaultBone")) {
					noRootBone = true;
				}
				else {
					std::vector<mat4> boneTransformArray;
					boneTransformArray.reserve(NUMBONES);

					for (int i = 0; i < NUMBONES; i++)
						boneTransformArray.push_back(mat4::Identity());

					PerformBoneTransforms(rootBone, joints, boneTransformArray);

					memcpy(command.bones, boneTransformArray.data(), sizeof(maths::mat4) * NUMBONES);
				}
			}
			else {
				noRootBone = true;
			}
			if (noRootBone) {
				std::vector<mat4> boneTransformArray;
				boneTransformArray.reserve(NUMBONES);

				for (int i = 0; i < NUMBONES; i++)
					boneTransformArray.push_back(mat4::Identity());

				memcpy(command.bones, boneTransformArray.data(), sizeof(maths::mat4) * NUMBONES);
			}
			command.shader = mesh->GetMaterialInstance()->GetMaterial()->GetShader();

			Submit(command);
		}

		void DeferredRenderer::SubmitLightSetup(const LightSetup& lightSetup)
		{
			union LightToByte {
				Light lights[NUMLIGHTS];
				byte data[NUMLIGHTS * sizeof(Light)];

				LightToByte() {}
			} ltb;

			for (int i = 0; i < NUMLIGHTS; i++) {
				if (lightSetup.GetLights().size() <= i) {
					ltb.lights[i] = Light();
				}
				else {
					ltb.lights[i] = *lightSetup.GetLights()[i];
				}
			}
			m_Material->SetUniformData("u_LightSetup", ltb.data, NUMLIGHTS * sizeof(Light));

		}

		void DeferredRenderer::EndScene()
		{
		}

		void DeferredRenderer::End()
		{
		}

		void DeferredRenderer::SetSystemUniforms(API::Shader* shader)
		{
			shader->SetVSSystemUniformBuffer(m_VSSystemUniformBuffer, m_VSSystemUniformBufferSize, 0);
			//shader->SetGSSystemUniformBuffer(m_GSSystemUniformBuffer, m_GSSystemUniformBufferSize, 0);
			//shader->SetPSSystemUniformBuffer(m_PSSystemUniformBuffer, m_PSSystemUniformBufferSize, 0);
		}

		void DeferredRenderer::Present()
		{
			m_FrameBuffer->Bind();
			Renderer::SetViewport(0, 0, m_ScreenBufferWidth, m_ScreenBufferHeight);
			m_FrameBuffer->Clear();
			for (uint i = 0; i < m_CommandQueue.size(); i++)
			{
				RenderCommand& command = m_CommandQueue[i];

				MaterialInstance* material = command.mesh->GetMaterialInstance();
				int materialRenderFlags = material->GetRenderFlags();
				Renderer::SetDepthTesting((materialRenderFlags & (int)Material::RenderFlags::DISABLE_DEPTH_TEST) == 0);
				memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ModelMatrix], &command.transform, sizeof(mat4));
				memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_JointMatrix], &command.bones, sizeof(mat4) * NUMBONES);

				SetSystemUniforms(command.shader);
				command.mesh->Render(*this);
			}

			m_FrameBuffer->Unbind();


			float aspectX = (float)m_ScreenBufferWidth / (float)m_ScreenBufferHeight;
			float aspectY = (float)m_ScreenBufferHeight / (float)m_ScreenBufferWidth;
			if (aspectX < 1.0f) aspectX = 1.0f;
			if (aspectY < 1.0f) aspectY = 1.0f;

			aspectX *= 0.5f;
			aspectY *= 0.5f;

			m_Material->SetUniform("u_ProjectionMatrix", maths::mat4::Orthographic(-aspectX, aspectX, aspectY, -aspectY, -10.0f, 10.0f));
			m_Material->SetUniform("u_ModelMatrix", maths::mat4::Scale(vec3(aspectX, -aspectY)));
			m_Material->SetTexture("u_PreintegratedFG", m_PreintegratedFG);

			m_Shader->Bind();
			m_Material->Bind();

			m_FrameBuffer->Render(m_Shader);

			m_VertexArray->Bind();
			m_IndexBuffer->Bind();
			m_VertexArray->Draw(RenderType::TRIANGLES, 6);
			m_IndexBuffer->Unbind();
			m_VertexArray->Unbind();

			m_Material->Unbind();
			m_Shader->Unbind();
		}
	}
}