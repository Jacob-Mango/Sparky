#include "sp/sp.h"
#include "ForwardRenderer.h"

#include "sp/app/Application.h"
#include "sp/graphics/API/Renderer.h"

namespace sp { namespace graphics {

	using namespace maths;

	enum VSSystemUniformIndices : int32
	{
		VSSystemUniformIndex_ProjectionMatrix	= 0,
		VSSystemUniformIndex_ViewMatrix			= 1,
		VSSystemUniformIndex_ModelMatrix		= 2,
		VSSystemUniformIndex_CameraPosition		= 3,
		VSSystemUniformIndex_JointMatrix		= 4,
		VSSystemUniformIndex_Size
	};

	enum PSSystemUniformIndices : int32
	{
		PSSystemUniformIndex_Lights = 0,
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
		m_CommandQueue.reserve(1000);

		//
		// Vertex shader system uniforms
		//
		m_VSSystemUniformBufferSize = sizeof(mat4) + sizeof(mat4) + sizeof(mat4) + sizeof(vec3) + (sizeof(mat4) * NUMBONES);
		m_VSSystemUniformBuffer = spnew byte[m_VSSystemUniformBufferSize];
		memset(m_VSSystemUniformBuffer, 0, m_VSSystemUniformBufferSize);
		m_VSSystemUniformBufferOffsets.resize(VSSystemUniformIndex_Size);

		// Per Scene System Uniforms
		m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix] = 0;
		m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ViewMatrix] = m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix] + sizeof(mat4);
		m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ModelMatrix] = m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ViewMatrix] + sizeof(mat4);
		m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_CameraPosition] = m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ModelMatrix] + sizeof(mat4);
		m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_JointMatrix] = m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_CameraPosition] + (sizeof(vec3));
		
		//
		// Pixel/fragment shader system uniforms
		//
		m_PSSystemUniformBufferSize = sizeof(Light);
		m_PSSystemUniformBuffer = spnew byte[m_PSSystemUniformBufferSize];
		memset(m_PSSystemUniformBuffer, 0, m_PSSystemUniformBufferSize);
		m_PSSystemUniformBufferOffsets.resize(PSSystemUniformIndex_Size);

		// Per Scene System Uniforms
		m_PSSystemUniformBufferOffsets[PSSystemUniformIndex_Lights] = 0;

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
		memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_CameraPosition], &camera->GetPosition(), sizeof(vec3));
	}

	void ForwardRenderer::Submit(const RenderCommand& command)
	{
		m_CommandQueue.push_back(command);
	}

	void ForwardRenderer::PerformBoneTransforms(Bone* bone, mat4 joints[NUMBONES], std::vector<mat4> &matArray) {
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

	void ForwardRenderer::SubmitMesh(Mesh* mesh, const maths::mat4& transform, mat4 joints[NUMBONES], Bone* rootBone)
	{
		RenderCommand command;
		command.mesh = mesh;
		command.transform = transform;
		
		bool nullptrRoot = false;

		if (rootBone != nullptr)
		{
			if (StringEquals(rootBone->Name, "defaultBone")) {
				nullptrRoot = true;
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
		else nullptrRoot = true;

		if (nullptrRoot) {

			mat4* temp = new mat4[NUMBONES];
			for (int i = 0; i < NUMBONES; i++)
				temp[i] = mat4::Identity();
			memcpy(command.bones, temp, sizeof(maths::mat4) * NUMBONES);
		}

		command.shader = mesh->GetMaterialInstance()->GetMaterial()->GetShader();

		Submit(command);
	}

	void ForwardRenderer::SubmitLightSetup(const LightSetup& lightSetup)
	{
		const auto& lights = lightSetup.GetLights();
		SP_ASSERT(lights.size() <= 1, "Only one light is supported at the moment!");
		for (uint i = 0; i < lights.size(); i++)
			memcpy(m_PSSystemUniformBuffer + m_PSSystemUniformBufferOffsets[PSSystemUniformIndex_Lights], lights[i], sizeof(Light));
	}

	void ForwardRenderer::EndScene()
	{
	}

	void ForwardRenderer::End()
	{
		// TODO: Batching and sorting
	}

	void ForwardRenderer::SetSystemUniforms(API::Shader* shader)
	{
		// TODO: Set per-mesh buffer to slot 1
		shader->SetVSSystemUniformBuffer(m_VSSystemUniformBuffer, m_VSSystemUniformBufferSize, 0);
		shader->SetGSSystemUniformBuffer(m_GSSystemUniformBuffer, m_GSSystemUniformBufferSize, 0);
		shader->SetPSSystemUniformBuffer(m_PSSystemUniformBuffer, m_PSSystemUniformBufferSize, 0);
	}

	void ForwardRenderer::Present()
	{
		// TODO: Shader binding, texture sorting, visibility testing, etc.
		for (uint i = 0; i < m_CommandQueue.size(); i++)
		{
			RenderCommand& command = m_CommandQueue[i];
			MaterialInstance* material = command.mesh->GetMaterialInstance();
			int materialRenderFlags = material->GetRenderFlags();
			Renderer::SetDepthTesting((materialRenderFlags & (int)Material::RenderFlags::DISABLE_DEPTH_TEST) == 0);
			memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ModelMatrix], &command.transform, sizeof(mat4));
			memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_JointMatrix], &command.bones, sizeof(mat4) * NUMBONES);
			
			command.shader->Bind();

			SetSystemUniforms(command.shader);
			command.mesh->Render(*this);

#if defined(SP_DEBUG) && 0
			uint j;
			for (j = 0; j < command.uniforms.size(); j++)
			{
				if (command.uniforms[j].uniform == "ml_matrix")
				{
					command.mesh->DebugRender(*(maths::mat4*)command.uniforms[j].value);
					break;
				}
			}
#endif
		}
	}

} }