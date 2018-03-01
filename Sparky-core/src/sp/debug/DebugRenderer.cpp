#include "sp/sp.h"
#include "DebugRenderer.h"

#include "sp/graphics/shaders/ShaderFactory.h"

#ifdef SP_DEBUG
namespace sp { namespace debug {

#define RENDERER_VERTEX_SIZE	sizeof(LineVertex)
#define RENDERER_MAX_LINES		65536
#define RENDERER_BUFFER_SIZE	RENDERER_VERTEX_SIZE * RENDERER_MAX_LINES
#define RENDERER_INDICES_SIZE	RENDERER_MAX_LINES * 2

	using namespace graphics;
	using namespace maths;

	enum VSSystemUniformIndices : int32
	{
		VSSystemUniformIndex_ProjectionMatrix = 0,
		VSSystemUniformIndex_ViewMatrix = 1,
		VSSystemUniformIndex_Size
	};

	static byte* m_VSSystemUniformBuffer;
	static uint m_VSSystemUniformBufferSize;

	static std::vector<uint> m_VSSystemUniformBufferOffsets;

	struct LineVertex
	{
		vec3 position;
		uint color;
	};

	static API::Shader* s_Shader;
	static Camera* s_Camera;
	static API::VertexArray* s_VertexArray;
	static API::IndexBuffer* s_IndexBuffer;
	static LineVertex* s_LineVertexPointer;
	static uint s_IndexCount;

	void DebugRenderer::Init()
	{
		s_Camera = nullptr;
		s_Shader = ShaderFactory::DebugShader();

		API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::DYNAMIC);
		buffer->Bind();
		buffer->Resize(RENDERER_BUFFER_SIZE);

		API::BufferLayout layout;
		layout.Push<vec3>("position");
		layout.Push<byte>("color", 4, true);
		buffer->SetLayout(layout);

		s_VertexArray = API::VertexArray::Create();
		s_VertexArray->PushBuffer(buffer);

		uint* indices = new uint[RENDERER_INDICES_SIZE];
		for (int32 i = 0; i < RENDERER_INDICES_SIZE; i++)
			indices[i] = i;

		s_IndexBuffer = API::IndexBuffer::Create(indices, RENDERER_INDICES_SIZE);
		buffer->Unbind();
		s_VertexArray->Unbind();
		s_IndexCount = 0;


		m_VSSystemUniformBufferSize = sizeof(mat4) + sizeof(mat4);
		m_VSSystemUniformBuffer = spnew byte[m_VSSystemUniformBufferSize];
		memset(m_VSSystemUniformBuffer, 0, m_VSSystemUniformBufferSize);
		m_VSSystemUniformBufferOffsets.resize(VSSystemUniformIndex_Size);

		m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix] = 0;
		m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ViewMatrix] = m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix] + sizeof(mat4);
	}

	void DebugRenderer::Shutdown()
	{
		delete s_Shader;
		delete s_VertexArray;
		delete s_IndexBuffer;
	}

	void DebugRenderer::Begin()
	{
		s_VertexArray->GetBuffer()->Bind();
		s_LineVertexPointer = s_VertexArray->GetBuffer()->GetPointer<LineVertex>();

		memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionMatrix], &s_Camera->GetProjectionMatrix(), sizeof(mat4));
		memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ViewMatrix], &s_Camera->GetViewMatrix(), sizeof(mat4));

	}

	void DebugRenderer::End()
	{
		s_VertexArray->GetBuffer()->ReleasePointer();
		s_VertexArray->GetBuffer()->Unbind();
	}

	void DebugRenderer::DrawLineInternal(const maths::vec3& start, const maths::vec3& end, uint color)
	{
		s_LineVertexPointer->position = start;
		s_LineVertexPointer->color = color;
		s_LineVertexPointer++;

		s_LineVertexPointer->position = end;
		s_LineVertexPointer->color = color;
		s_LineVertexPointer++;

		s_IndexCount += 2;

		if (s_IndexCount >= RENDERER_INDICES_SIZE)
		{
			End();
			Present();
			Begin();
		}
	}

	void DebugRenderer::DrawLine(const maths::vec3& start, const maths::vec3& end, uint color)
	{
		Begin();
		DrawLineInternal(start, end, color);
		End();
	}

	void DebugRenderer::DrawMesh(const Mesh* mesh, DebugRenderMeshFlags flags, const maths::mat4& transform)
	{
		Vertex* vertices = nullptr;
		uint count = mesh->GetDebugData(vertices);
		float scalar = 2.0f;
		Begin();
		for (uint i = 0; i < count; i++)
		{
			Vertex& v = vertices[i];
			vec3 position = transform * v.position;
			DrawLineInternal(vec3(position.x, position.y, position.z), vec3(position.x + v.normal.x * scalar, position.y + v.normal.y * scalar, position.z + v.normal.z * scalar), 0xff0000ff);
			DrawLineInternal(vec3(position.x, position.y, position.z), vec3(position.x + v.binormal.x * scalar, position.y + v.binormal.y * scalar, position.z + v.binormal.z * scalar), 0xff00ff00);
			DrawLineInternal(vec3(position.x, position.y, position.z), vec3(position.x + v.tangent.x * scalar, position.y + v.tangent.y * scalar, position.z + v.tangent.z * scalar), 0xffff0000);
		}
		End();
	}

	void DebugRenderer::SetCamera(Camera* camera)
	{
		Present();
		s_Camera = camera;
	}

	void DebugRenderer::Present()
	{
		if (!s_Camera)
			return;

		s_Shader->Bind();
		s_VertexArray->Bind();
		s_IndexBuffer->Bind();
		s_VertexArray->Draw(s_IndexBuffer->GetCount());
		s_IndexBuffer->Unbind();
		s_VertexArray->Unbind();
		s_Shader->Unbind();
		s_IndexCount = 0;
	}

} }
#endif