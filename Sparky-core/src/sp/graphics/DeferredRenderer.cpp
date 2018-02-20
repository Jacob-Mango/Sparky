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

		enum GSSystemUniformIndices : int32
		{
			GSSystemUniformIndex_Size
		};

		enum PSSystemUniformIndices : int32
		{
			PSSystemUniformIndex_Size
		};

		std::vector<API::TextureParameters> deferredBufferParameters = {
			API::TextureParameters(API::TextureFormat::RGBA32F, API::TextureFilter::LINEAR, API::TextureWrap::CLAMP_TO_EDGE, API::TextureType::FLOAT, false),
			API::TextureParameters(API::TextureFormat::RGBA32F, API::TextureFilter::LINEAR, API::TextureWrap::CLAMP_TO_EDGE, API::TextureType::FLOAT, false),
			API::TextureParameters(API::TextureFormat::RGBA32F, API::TextureFilter::LINEAR, API::TextureWrap::CLAMP_TO_EDGE, API::TextureType::FLOAT, false),
			API::TextureParameters(API::TextureFormat::RGBA32F, API::TextureFilter::LINEAR, API::TextureWrap::CLAMP_TO_EDGE, API::TextureType::FLOAT, false),
		};

		std::vector<API::TextureParameters> framebufferParameters = {
			API::TextureParameters(API::TextureFormat::RGBA32F, API::TextureFilter::LINEAR, API::TextureWrap::CLAMP_TO_EDGE, API::TextureType::FLOAT, false),
			API::TextureParameters(API::TextureFormat::RGBA32F, API::TextureFilter::LINEAR, API::TextureWrap::CLAMP_TO_EDGE, API::TextureType::FLOAT, false),
			API::TextureParameters(API::TextureFormat::RGBA32F, API::TextureFilter::LINEAR, API::TextureWrap::CLAMP_TO_EDGE, API::TextureType::FLOAT, false)
		};

		DeferredRenderer::DeferredRenderer()
		{
			SetScreenBufferSize(Application::GetApplication().GetWindowWidth(), Application::GetApplication().GetWindowHeight());
		}

		DeferredRenderer::DeferredRenderer(uint width, uint height)
		{
			SetScreenBufferSize(width, height);
		}

		void DeferredRenderer::SetScreenBufferSize(uint width, uint height)
		{
			m_ScreenBufferWidth = width;
			m_ScreenBufferHeight = height;

			float w = m_ScreenBufferWidth;
			float h = m_ScreenBufferHeight;

			m_DeferredBuffer = API::FrameBuffer2D::Create(w, h, deferredBufferParameters);
			m_FrameBuffer = API::FrameBuffer2D::Create(w, h, framebufferParameters);
			// FontManager::SetScale(maths::vec2(w / 32.0f, h / 18.0f));
		}

		void DeferredRenderer::Init()
		{
			m_DeferredBuffer = API::FrameBuffer2D::Create(m_ScreenBufferWidth, m_ScreenBufferHeight, deferredBufferParameters);
			m_FrameBuffer = API::FrameBuffer2D::Create(m_ScreenBufferWidth, m_ScreenBufferHeight, framebufferParameters);

			m_MaxLights = 64;

			m_Shader = API::Shader::CreateFromFile("Deferred", String("/shaders/PBR_D/PBR_D.shader"));
			m_Material = spnew Material("Deferred", m_Shader);

			ShaderManager::Add(m_Shader);

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

			m_PSSystemUniformBufferSize = 0;
			m_PSSystemUniformBuffer = spnew byte[m_PSSystemUniformBufferSize];
			memset(m_PSSystemUniformBuffer, 0, m_PSSystemUniformBufferSize);
			m_PSSystemUniformBufferOffsets.resize(PSSystemUniformIndex_Size);


			m_Mesh = MeshFactory::CreateQuad(vec2(-1, -1), vec2(2, 2), nullptr);
			m_IndexBuffer = m_Mesh->m_IndexBuffer;
			m_VertexArray = m_Mesh->m_VertexArray;

			m_PreintegratedFG = API::Texture2D::CreateFromFile("PreintegratedFG", "engine/PreintegratedFG.bmp");
			
			String environmentFiles[11] =
			{
				"/materials/cubemap/CubeMap0.tga",
				"/materials/cubemap/CubeMap1.tga",
				"/materials/cubemap/CubeMap2.tga",
				"/materials/cubemap/CubeMap3.tga",
				"/materials/cubemap/CubeMap4.tga",
				"/materials/cubemap/CubeMap5.tga",
				"/materials/cubemap/CubeMap6.tga",
				"/materials/cubemap/CubeMap7.tga",
				"/materials/cubemap/CubeMap8.tga",
				"/materials/cubemap/CubeMap9.tga",
				"/materials/cubemap/CubeMap10.tga"
			};

			m_Environment = API::TextureCube::CreateFromVCross(environmentFiles, 11);
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

		void DeferredRenderer::PerformBoneTransforms(Bone* bone, mat4 boneTransforms[NUMBONES], std::vector<mat4> &matArray) {
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

		void DeferredRenderer::SubmitMesh(Mesh* mesh, const maths::mat4& transform, mat4 boneTransforms[NUMBONES], Bone* rootBone)
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

		void DeferredRenderer::SubmitLightSetup(LightSetup& lightSetup)
		{
			//byte* data = lightSetup.GetLightData(0, m_MaxLights);
			//m_Material->SetUniformData("u_LightSetup", data, m_MaxLights * sizeof(Light));
		}

		void DeferredRenderer::EndScene()
		{
		}

		void DeferredRenderer::End()
		{
		}

		void DeferredRenderer::SetSystemUniforms(API::Shader* shader)
		{
			shader->SetSystemUniformBuffer(API::ShaderType::VERTEX, m_VSSystemUniformBuffer, m_VSSystemUniformBufferSize, 0);
			shader->SetSystemUniformBuffer(API::ShaderType::GEOMETRY, m_GSSystemUniformBuffer, m_GSSystemUniformBufferSize, 0);
			shader->SetSystemUniformBuffer(API::ShaderType::FRAGMENT, m_PSSystemUniformBuffer, m_PSSystemUniformBufferSize, 0);
		}

		void DeferredRenderer::Present()
		{
			m_DeferredBuffer->Bind();
			m_DeferredBuffer->SetClearColor(maths::vec4(0));
			m_DeferredBuffer->Clear();
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
			m_DeferredBuffer->Unbind();

			m_FrameBuffer->Bind();
			m_FrameBuffer->SetClearColor(maths::vec4(0));
			m_FrameBuffer->Clear();
			m_Material->Bind();

			float aspectX = (float)m_ScreenBufferWidth / (float)m_ScreenBufferHeight;
			float aspectY = (float)m_ScreenBufferHeight / (float)m_ScreenBufferWidth;
			if (aspectX < 1.0f) aspectX = 1.0f;
			if (aspectY < 1.0f) aspectY = 1.0f;

			aspectX *= 0.5f;
			aspectY *= 0.5f;

			m_Material->SetUniform("u_ProjectionMatrix", maths::mat4::Orthographic(-aspectX, aspectX, aspectY, -aspectY, -10.0f, 10.0f));
			m_Material->SetUniform("u_ModelMatrix", maths::mat4::Scale(vec3(aspectX, -aspectY)));
			m_Material->SetTexture("u_PreintegratedFG", m_PreintegratedFG);
			m_Material->SetTexture("u_EnvironmentMap", m_Environment);

			

			std::vector<API::Texture2D*> textures = m_DeferredBuffer->GetTextures();
			m_Material->SetTexture("u_Position", textures[0]);
			m_Material->SetTexture("u_Albedo", textures[1]);
			m_Material->SetTexture("u_Metallic", textures[2]);
			m_Material->SetTexture("u_Normal", textures[3]);

			m_VertexArray->Bind();
			m_IndexBuffer->Bind();
			m_VertexArray->Draw(RenderType::TRIANGLES, 6);
			m_IndexBuffer->Unbind();
			m_VertexArray->Unbind();

			m_Material->Unbind();

			m_FrameBuffer->Unbind();
		}
	}
}