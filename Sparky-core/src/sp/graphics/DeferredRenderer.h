#pragma once

#include "sp/Common.h"

#include "Renderer3D.h"

#include "shaders/Shader.h"

#include "API/Framebuffer2D.h"

namespace sp { namespace graphics {

	class SP_API DeferredRenderer : public Renderer3D
	{
	private:
		API::Framebuffer2D* m_FrameBuffer;
		API::Shader* m_Shader;
		API::VertexArray* m_VertexArray;
		API::IndexBuffer* m_IndexBuffer;

		Mesh* m_Mesh;
		Material* m_Material;

		byte* m_VSSystemUniformBuffer;
		uint m_VSSystemUniformBufferSize;
		byte* m_GSSystemUniformBuffer;
		uint m_GSSystemUniformBufferSize;
		byte* m_PSSystemUniformBuffer;
		uint m_PSSystemUniformBufferSize;

		std::vector<uint> m_VSSystemUniformBufferOffsets;
		std::vector<uint> m_GSSystemUniformBufferOffsets;
		std::vector<uint> m_PSSystemUniformBufferOffsets;

		API::Texture2D* m_PreintegratedFG;
	public:
		DeferredRenderer(uint width, uint height);
		DeferredRenderer();
		
		void Init() override;
		void Begin() override;
		void BeginScene(Camera* camera) override;
		void Submit(const RenderCommand& command) override;
		void SubmitMesh(Mesh* mesh, const maths::mat4& transform, maths::mat4 joints[NUMBONES], Bone* rootBone) override;
		void SubmitLightSetup(const LightSetup& lightSetup);
		void EndScene() override;
		void End() override;
		void Present() override;
	private:
		void SetSystemUniforms(API::Shader* shader);

		void PerformBoneTransforms(Bone* bone, maths::mat4 joints[NUMBONES], std::vector<maths::mat4> &matArray) override;
	};

} }