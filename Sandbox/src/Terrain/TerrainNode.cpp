#include "TerrainNode.h"

namespace sp {
	namespace scene {
		namespace terrain {

			using namespace graphics;

			TerrainNode::TerrainNode(Camera* camera) : m_Camera(camera)
			{
				using namespace maths;

				const int size = 12;
				int index = 0;

				TerrainVertex data[size];
				memset(data, 0, size * sizeof(TerrainVertex));

				data[index++] = CreateVertex(vec3(0.000f, 0.000f, 1.000f));
				data[index++] = CreateVertex(vec3(0.894f, 0.000f, 0.447f));
				data[index++] = CreateVertex(vec3(0.276f, 0.851f, 0.447f));
				data[index++] = CreateVertex(vec3(-0.724f, 0.526f, 0.447f));
				data[index++] = CreateVertex(vec3(-0.724f, -0.526f, 0.447f));
				data[index++] = CreateVertex(vec3(0.276f, -0.851f, 0.447f));
				data[index++] = CreateVertex(vec3(0.724f, 0.526f, -0.447f));
				data[index++] = CreateVertex(vec3(-0.276f, 0.851f, -0.447f));
				data[index++] = CreateVertex(vec3(-0.894f, 0.000f, -0.447f));
				data[index++] = CreateVertex(vec3(-0.276f, -0.851f, -0.447f));
				data[index++] = CreateVertex(vec3(0.724f, -0.526f, -0.447f));
				data[index++] = CreateVertex(vec3(0.000f, 0.000f, -1.000f));


				API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
				buffer->SetData(size * sizeof(TerrainVertex), data);

				API::BufferLayout layout(RenderType::PATCHES);
				layout.Push<vec2>("POSITION");
				buffer->SetLayout(layout);

				API::VertexArray* va = API::VertexArray::Create();
				va->PushBuffer(buffer);

				uint indices[] =
				{
					2, 1, 0,
					3, 2, 0,
					4, 3, 0,
					5, 4, 0,
					1, 5, 0,
					11, 6,  7,
					11, 7,  8,
					11, 8,  9,
					11, 9,  10,
					11, 10, 6,
					1, 2, 6,
					2, 3, 7,
					3, 4, 8,
					4, 5, 9,
					5, 1, 10,
					2,  7, 6,
					3,  8, 7,
					4,  9, 8,
					5, 10, 9,
					1, 6, 10
				};

				uint isize = sizeof(indices) / sizeof(indices[0]);


				API::IndexBuffer* ib = API::IndexBuffer::Create(indices, isize);

				m_Instance = spnew MaterialInstance(MaterialManager::Get("Terrain"));
				Mesh* mesh = spnew Mesh(va, ib, m_Instance);
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

			TerrainVertex TerrainNode::CreateVertex(maths::vec3 pos)
			{
				TerrainVertex vertex;
				vertex.position = pos;
				return vertex;
			}

		}
	}
}