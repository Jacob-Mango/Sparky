#pragma once

#include "sp/Common.h"
#include "Renderer3D.h"
#include "shaders/Shader.h"

namespace sp { namespace graphics {

	class SP_API ForwardRenderer : public Renderer3D
	{
	private:
		byte* m_VSSystemUniformBuffer;
		uint m_VSSystemUniformBufferSize;
		byte* m_GSSystemUniformBuffer;
		uint m_GSSystemUniformBufferSize;
		byte* m_PSSystemUniformBuffer;
		uint m_PSSystemUniformBufferSize;

		std::vector<uint> m_VSSystemUniformBufferOffsets;
		std::vector<uint> m_GSSystemUniformBufferOffsets;
		std::vector<uint> m_PSSystemUniformBufferOffsets;

		uint m_MaxLights;

		API::TextureCube* m_Environment;
	public:
		ForwardRenderer();
		ForwardRenderer(uint width, uint height);

		void Init() override;
		void Begin() override;
		void BeginScene(Camera* camera) override;
		void Submit(const RenderCommand& command) override;
		void SubmitMesh(Mesh* mesh, const maths::mat4& transform, maths::mat4 joints[NUMBONES], Bone* rootBone) override;
		void SubmitLightSetup(LightSetup& lightSetup);
		void EndScene() override;
		void End() override;
		void Present() override;
	private:
		void SetSystemUniforms(API::Shader* shader);

		void PerformBoneTransforms(Bone* bone, maths::mat4 joints[NUMBONES], std::vector<maths::mat4> &matArray) override;
	};

} }
