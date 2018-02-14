#include "sp/sp.h"
#include "ForwardRenderer.h"

#include "sp/app/Application.h"
#include "sp/graphics/API/Renderer.h"

#include "API/Framebuffer2D.h"
#include "API/Texture.h"

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

		enum GSSystemUniformIndices : int32
		{
			GSSystemUniformIndex_Size
		};

		enum PSSystemUniformIndices : int32
		{
			PSSystemUniformIndex_Lights = 0,
			PSSystemUniformIndex_CameraPosition = 1,
			PSSystemUniformIndex_Size
		};

		ForwardRenderer::ForwardRenderer()
		{
			SetScreenBufferSize(Application::GetApplication().GetWindowWidth(), Application::GetApplication().GetWindowHeight());
		}

		ForwardRenderer::ForwardRenderer(uint width, uint height)
		{
			SetScreenBufferSize(width, height);
		}

		void ForwardRenderer::Init()
		{
			m_FrameBuffer = API::FrameBuffer2D::Create(m_ScreenBufferWidth, m_ScreenBufferHeight, {
					API::TextureParameters(API::TextureFormat::RGBA32F, API::TextureFilter::LINEAR, API::TextureWrap::CLAMP_TO_EDGE, API::TextureType::FLOAT)
				});

			m_MaxLights = 4;

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

			m_PSSystemUniformBufferSize = (m_MaxLights * sizeof(Light)) + sizeof(vec3);
			m_PSSystemUniformBuffer = spnew byte[m_PSSystemUniformBufferSize];
			memset(m_PSSystemUniformBuffer, 0, m_PSSystemUniformBufferSize);
			m_PSSystemUniformBufferOffsets.resize(PSSystemUniformIndex_Size);

			m_PSSystemUniformBufferOffsets[PSSystemUniformIndex_Lights] = 0;
			m_PSSystemUniformBufferOffsets[PSSystemUniformIndex_CameraPosition] = m_PSSystemUniformBufferOffsets[PSSystemUniformIndex_Lights] + m_MaxLights * sizeof(Light);
		}

		void ForwardRenderer::Begin()
		{
			Renderer::SetViewport(0, 0, m_ScreenBufferWidth, m_ScreenBufferHeight);

			m_CommandQueue.clear();
			m_SystemUniforms.clear();
		}

		void ForwardRenderer::BeginScene(Camera* camera)
		{
			memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix], &camera->GetProjectionMatrix(), sizeof(mat4));
			memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ViewMatrix], &camera->GetViewMatrix(), sizeof(mat4));
			memcpy(m_PSSystemUniformBuffer + m_PSSystemUniformBufferOffsets[PSSystemUniformIndex_CameraPosition], &camera->GetPosition(), sizeof(vec3));
		}

		void ForwardRenderer::Submit(const RenderCommand& command)
		{
			m_CommandQueue.push_back(command);
		}

		void ForwardRenderer::PerformBoneTransforms(Bone* bone, mat4 boneTransforms[NUMBONES], std::vector<mat4> &matArray) {
			mat4 concatenated_transforms = mat4::Identity();
			if (bone && bone->Parent) {
				graphics::Bone* b = bone->Parent;
				std::vector<mat4> mats;
				while (b != nullptr)
				{
					mats.push_back(boneTransforms[b->ID]);
					b = b->Parent;
				}
				for (int i = mats.size() - 1; i >= 0; i--)
					concatenated_transforms *= mats.at(i);
			}

			matArray[bone->ID] = concatenated_transforms.Multiply(boneTransforms[bone->ID]);
			for (int i = 0; i < bone->Children.size(); i++) {
				PerformBoneTransforms(bone->Children[i], boneTransforms, matArray);
			}
		}

		void ForwardRenderer::SubmitMesh(Mesh* mesh, const maths::mat4& transform, mat4 boneTransforms[NUMBONES], Bone* rootBone)
		{
			RenderCommand command;
			command.mesh = mesh;
			command.transform = transform;

			if (mesh->HasRoot() && boneTransforms != nullptr) {
				std::vector<mat4> boneTransformArray;
				boneTransformArray.reserve(NUMBONES);

				for (int i = 0; i < NUMBONES; i++)
					boneTransformArray.push_back(mat4::Identity());

				PerformBoneTransforms(rootBone, boneTransforms, boneTransformArray);

				memcpy(command.bones, boneTransformArray.data(), sizeof(maths::mat4) * NUMBONES);
			}
			else {
				mat4* temp = new mat4[NUMBONES];
				for (int i = 0; i < NUMBONES; i++)
					temp[i] = mat4::Identity();
				memcpy(command.bones, temp, sizeof(maths::mat4) * NUMBONES);

				delete temp;
			}

			command.shader = mesh->GetMaterialInstance()->GetMaterial()->GetShader();

			Submit(command);
		}

		void ForwardRenderer::SubmitLightSetup(LightSetup& lightSetup)
		{
			//byte* data = lightSetup.GetLightData(0, m_MaxLights);
			//memcpy(m_PSSystemUniformBuffer + m_PSSystemUniformBufferOffsets[PSSystemUniformIndex_Lights], data, m_MaxLights * sizeof(Light));
		}

		void ForwardRenderer::EndScene()
		{
		}

		void ForwardRenderer::End()
		{
		}

		void ForwardRenderer::SetSystemUniforms(API::Shader* shader)
		{
			shader->SetSystemUniformBuffer(API::ShaderType::VERTEX, m_VSSystemUniformBuffer, m_VSSystemUniformBufferSize, 0);
			shader->SetSystemUniformBuffer(API::ShaderType::GEOMETRY, m_GSSystemUniformBuffer, m_GSSystemUniformBufferSize, 0);
			shader->SetSystemUniformBuffer(API::ShaderType::FRAGMENT, m_PSSystemUniformBuffer, m_PSSystemUniformBufferSize, 0);
		}

		void ForwardRenderer::Present()
		{
			Renderer::SetViewport(0, 0, m_ScreenBufferWidth, m_ScreenBufferHeight);

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
		}

	}
}