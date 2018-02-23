#include "TerrainNode.h"

namespace sp {
	namespace scene {
		namespace terrain {

			using namespace graphics;

			TerrainNode::TerrainNode()
			{
				using namespace maths;

				const int size = 16;
				int index = 0;

				TerrainVertex data[size];
				memset(data, 0, size * sizeof(TerrainVertex));

				data[index++] = CreateVertex(vec2(0, 0));
				data[index++] = CreateVertex(vec2(0.333f, 0));
				data[index++] = CreateVertex(vec2(0.666f, 0));
				data[index++] = CreateVertex(vec2(1, 0));

				data[index++] = CreateVertex(vec2(0, 0.333f));
				data[index++] = CreateVertex(vec2(0.333f, 0.333f));
				data[index++] = CreateVertex(vec2(0.666f, 0.333f));
				data[index++] = CreateVertex(vec2(1, 0.333f));

				data[index++] = CreateVertex(vec2(0, 0.666f));
				data[index++] = CreateVertex(vec2(0.333f, 0.666f));
				data[index++] = CreateVertex(vec2(0.666f, 0.666f));
				data[index++] = CreateVertex(vec2(1, 0.666f));

				data[index++] = CreateVertex(vec2(0, 1));
				data[index++] = CreateVertex(vec2(0.333f, 1));
				data[index++] = CreateVertex(vec2(0.666f, 1));
				data[index++] = CreateVertex(vec2(1, 1));

				API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
				buffer->SetData(size * sizeof(TerrainVertex), data);

				API::BufferLayout layout;
				layout.Push<vec2>("POSITION");
				buffer->SetLayout(layout);

				API::VertexArray* va = API::VertexArray::Create();
				va->PushBuffer(buffer);

				uint* indices = spnew uint[size]
				{
					0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
				};

				API::IndexBuffer* ib = API::IndexBuffer::Create(indices, size);



				Mesh* mesh = spnew Mesh(va, ib, spnew MaterialInstance(MaterialManager::Get("Terrain")));
				AddComponent(spnew component::MeshComponent(mesh));
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

			TerrainVertex TerrainNode::CreateVertex(maths::vec2 pos)
			{
				TerrainVertex vertex;
				vertex.position = pos;
				return vertex;
			}
			
		}
	}
}