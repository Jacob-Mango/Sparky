#pragma once

#include "Mesh.h"
#include "RenderCommand.h"
#include "camera/Camera.h"

#include "LightSetup.h"

#include "API\Framebuffer2D.h"

#define NUMBONES 64

namespace sp { namespace graphics {

	typedef std::vector<RenderCommand> CommandQueue;
	typedef std::vector<RendererUniform> SystemUniformList;

	class SP_API Renderer3D
	{
	protected:
		uint m_ScreenBufferWidth, m_ScreenBufferHeight;

		CommandQueue m_CommandQueue;
		SystemUniformList m_SystemUniforms;

		API::FrameBuffer2D* m_FrameBuffer;
	public:
		virtual ~Renderer3D() {}

		virtual void Init() = 0;
		virtual void Begin() = 0;
		virtual void BeginScene(Camera* camera) = 0;
		// TODO: Submit needs to be replaced by some sort of macro
		virtual void Submit(const RenderCommand& command) = 0;
		virtual void SubmitMesh(Mesh* mesh, const maths::mat4& transform, maths::mat4 joints[NUMBONES], Bone* rootBone) = 0;
		virtual void SubmitLightSetup(LightSetup& lightSetup) = 0;
		virtual void EndScene() = 0;
		virtual void End() = 0;
		virtual void Present() = 0;

		virtual void SetScreenBufferSize(uint width, uint height) { m_ScreenBufferWidth = width; m_ScreenBufferHeight = height; }

		virtual API::FrameBuffer2D* GetFrameBuffer() { return m_FrameBuffer;  }
	protected:
		virtual void PerformBoneTransforms(Bone* bone, maths::mat4 joints[NUMBONES], std::vector<maths::mat4> &matArray) = 0;
	};

} }