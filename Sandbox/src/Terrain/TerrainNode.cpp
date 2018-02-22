#include "TerrainNode.h"

namespace sp {
	namespace scene {
		namespace terrain {

			using namespace graphics;

			TerrainNode::TerrainNode()
			{
			}

			TerrainNode::~TerrainNode()
			{
			}

			void TerrainNode::OnInit()
			{
			}

			void TerrainNode::OnUpdate(const sp::Timestep& ts)
			{
			}

			/*
			void TerrainNode::CreatePatch()
			{
				using namespace maths;


				Vertex data[4];
				memset(data, 0, 4 * sizeof(Vertex));

				float size = 0.0f;
				vec3 normal = vec3(0, 1, 0);

				vec2 textureScale = vec2(size, size).Divide(128.0f);

				data[0].position = vec3(-size / 2.0f, 0.0f, -size / 2.0f);
				data[0].normal = normal;
				data[0].uv = (vec2)vec2(0.0f, 0.0f).Multiply(textureScale);
				data[0].binormal = mat4::Rotate(90.0f, vec3(0, 1, 0)) * normal;
				data[0].tangent = mat4::Rotate(90.0f, vec3(0, 0, 1)) * normal;
				data[0].weights = vec4(1);
				data[0].indices = vec4(0);

				data[1].position = vec3(-size / 2.0f, 0.0f, size / 2.0f);
				data[1].normal = normal;
				data[1].uv = (vec2)vec2(0.0f, 1.0f).Multiply(textureScale);
				data[1].binormal = mat4::Rotate(90.0f, vec3(0, 1, 0)) * normal;
				data[1].tangent = mat4::Rotate(90.0f, vec3(0, 0, 1)) * normal;
				data[1].weights = vec4(1);
				data[1].indices = vec4(1);

				data[2].position = vec3(size / 2.0f, 0.0f, size / 2.0f);
				data[2].normal = normal;
				data[2].uv = (vec2)vec2(1.0f, 1.0f).Multiply(textureScale);
				data[2].binormal = mat4::Rotate(90.0f, vec3(0, 1, 0)) * normal;
				data[2].tangent = mat4::Rotate(90.0f, vec3(0, 0, 1)) * normal;
				data[2].weights = vec4(1);
				data[2].indices = vec4(2);

				data[3].position = vec3(size / 2.0f, 0.0f, -size / 2.0f);
				data[3].normal = normal;
				data[3].uv = (vec2)vec2(1.0f, 0.0f).Multiply(textureScale);
				data[3].binormal = mat4::Rotate(90.0f, vec3(0, 1, 0)) * normal;
				data[3].tangent = mat4::Rotate(90.0f, vec3(0, 0, 1)) * normal;
				data[3].weights = vec4(1);
				data[3].indices = vec4(3);


				API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
				buffer->SetData(4 * sizeof(Vertex), data);

				API::BufferLayout layout;
				layout.Push<vec3>("POSITION");
				layout.Push<vec3>("NORMAL");
				layout.Push<vec2>("TEXCOORD");
				layout.Push<vec3>("BINORMAL");
				layout.Push<vec3>("TANGENT");
				buffer->SetLayout(layout);

				API::VertexArray* va = API::VertexArray::Create();
				va->PushBuffer(buffer);

				uint* indices = new uint[6]
				{
					0, 1, 2,
					2, 3, 0
				};

				API::IndexBuffer* ib = API::IndexBuffer::Create(indices, 6);

				Mesh* mesh = new Mesh(va, ib, nullptr);
				AddComponent(spnew component::MeshComponent(mesh));
			}
			*/
		}
	}
}