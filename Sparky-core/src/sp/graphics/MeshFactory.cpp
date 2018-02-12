#include "sp/sp.h"
#include "MeshFactory.h"

#include "sp/graphics/Renderer2D.h"

#include "sp/graphics/Renderable2D.h"

#include "sp/graphics/shaders/Shader.h"

#include <GL/glew.h>

namespace sp { namespace graphics { namespace MeshFactory {

	Mesh* CreatePoint(MaterialInstance* material)
	{
		using namespace maths;


		struct Vert {
			vec2 position;
			vec2 uv;
		};

		Vert data[1];

		data[0].position = maths::vec2(0, 0);
		data[0].uv = maths::vec2(0, 0);

		API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
		buffer->SetData(sizeof(Vert), data);

		API::BufferLayout layout;
		layout.Push<vec2>("POSITION");
		layout.Push<vec2>("UV");
		buffer->SetLayout(layout);

		API::VertexArray* va = API::VertexArray::Create();
		va->PushBuffer(buffer);
		uint* indices = new uint[1]{ 0 };
		API::IndexBuffer* ib = API::IndexBuffer::Create(indices, 1);
		Mesh* mesh = new Mesh(va, ib, material);
		mesh->m_RenderType = RenderType::POINTS;
		return mesh;
	}

	Mesh* CreateQuad(float x, float y, float width, float height, MaterialInstance* material)
	{
		using namespace maths;

		struct QuadVertex
		{
			maths::vec4 position;
			maths::vec2 uv;
		};

		QuadVertex* data = spnew QuadVertex[4];

		data[0].position = maths::vec4(x, y, 0);
		data[0].uv = maths::vec2(0, 0);

		data[1].position = maths::vec4(x + width, y, 0);
		data[1].uv = maths::vec2(1, 0);

		data[2].position = maths::vec4(x + width, y + height, 0);
		data[2].uv = maths::vec2(1, 1);

		data[3].position = maths::vec4(x, y + height, 0);
		data[3].uv = maths::vec2(0, 1);

		API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
		buffer->SetData(sizeof(QuadVertex) * 4, data);

		API::BufferLayout layout;
		layout.Push<vec4>("POSITION");
		layout.Push<vec2>("TEXCOORD");
		buffer->SetLayout(layout);

		API::VertexArray* va = API::VertexArray::Create();
		va->PushBuffer(buffer);
		uint* indices = new uint[6] { 0, 3, 2, 2, 1, 0, };
		API::IndexBuffer* ib = API::IndexBuffer::Create(indices, 6);
		return new Mesh(va, ib, material);
	}

	Mesh* CreateQuad(const maths::vec2& position, const maths::vec2& size, MaterialInstance* material)
	{
		return CreateQuad(position.x, position.y, size.x, size.y, material);
	}

	Mesh* CreateCube(float size, MaterialInstance* material)
	{
		using namespace maths;

		Vertex data[24];

		memset(data, 0, 24 * sizeof(Vertex));

		for (uint i = 0; i < 3; i++)
		{
			data[0 + 8 * i].position = vec3(-size / 2.0f, -size / 2.0f, size / 2.0f);
			data[1 + 8 * i].position = vec3(size / 2.0f, -size / 2.0f, size / 2.0f);
			data[2 + 8 * i].position = vec3(size / 2.0f, size / 2.0f, size / 2.0f);
			data[3 + 8 * i].position = vec3(-size / 2.0f, size / 2.0f, size / 2.0f);
			data[4 + 8 * i].position = vec3(-size / 2.0f, -size / 2.0f, -size / 2.0f);
			data[5 + 8 * i].position = vec3(size / 2.0f, -size / 2.0f, -size / 2.0f);
			data[6 + 8 * i].position = vec3(size / 2.0f, size / 2.0f, -size / 2.0f);
			data[7 + 8 * i].position = vec3(-size / 2.0f, size / 2.0f, -size / 2.0f);
		}

		data[0].normal = vec3(-1.0f, 0.0f, 0.0f);
		data[8].normal = vec3(0.0f, -1.0f, 0.0f);
		data[16].normal = vec3(0.0f, 0.0f, 1.0f);

		data[1].normal = vec3(1.0f, 0.0f, 0.0f);
		data[9].normal = vec3(0.0f, -1.0f, 0.0f);
		data[17].normal = vec3(0.0f, 0.0f, 1.0f);

		data[2].normal = vec3(1.0f, 0.0f, 0.0f);
		data[10].normal = vec3(0.0f, 1.0f, 0.0f);
		data[18].normal = vec3(0.0f, 0.0f, 1.0f);

		data[3].normal = vec3(-1.0f, 0.0f, 0.0f);
		data[11].normal = vec3(0.0f, 1.0f, 0.0f);
		data[19].normal = vec3(0.0f, 0.0f, 1.0f);

		data[4].normal = vec3(-1.0f, 0.0f, 0.0f);
		data[12].normal = vec3(0.0f, -1.0f, 0.0f);
		data[20].normal = vec3(0.0f, 0.0f, -1.0f);

		data[5].normal = vec3(1.0f, 0.0f, 0.0f);
		data[13].normal = vec3(0.0f, -1.0f, 0.0f);
		data[21].normal = vec3(0.0f, 0.0f, -1.0f);

		data[6].normal = vec3(1.0f, 0.0f, 0.0f);
		data[14].normal = vec3(0.0f, 1.0f, 0.0f);
		data[22].normal = vec3(0.0f, 0.0f, -1.0f);

		data[7].normal = vec3(-1.0f, 0.0f, 0.0f);
		data[15].normal = vec3(0.0f, 1.0f, 0.0f);
		data[23].normal = vec3(0.0f, 0.0f, -1.0f);

		for (int i = 0; i < 24; i++) {
			data[i].binormal = mat4::Rotate(90.0f, vec3(0, 1, 0)) * data[i].normal;
			data[i].tangent = mat4::Rotate(90.0f, vec3(0, 0, 1)) * data[i].normal;
			data[i].uv = vec3(0, 0, 0);
			data[i].indices = vec4(0, 0, 0, 0);
			data[i].weights = vec4(1, 1, 1, 1);
		}

		API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
		buffer->SetData(24 * sizeof(Vertex), data);

		API::BufferLayout layout;
		layout.Push<vec3>("POSITION");
		layout.Push<vec3>("NORMAL");
		layout.Push<vec2>("TEXCOORD");
		layout.Push<vec3>("BINORMAL");
		layout.Push<vec3>("TANGENT");
		layout.Push<vec4>("JOINTWEIGHTS");
		layout.Push<vec4>("JOINTINDICES");
		buffer->SetLayout(layout);

		API::VertexArray* va = API::VertexArray::Create();
		va->PushBuffer(buffer);

		uint* indices = new uint[36]
		{
			4,  7,  3,  3,  0,  4, // 4 7 3 0 : -x
			5,  6,  2,  2,  1,  5, // 5 6 2 1 : +x
			12, 13,  9,  9,  8, 12, // 4 5 1 0 : -y
			15, 14, 10, 10, 11, 15, // 7 6 2 3 : +y
			20, 21, 22, 22, 23, 20, // 4 5 6 7 : -z
			16, 17, 18, 18, 19, 16  // 0 1 2 3 : +z
		};

		API::IndexBuffer* ib = API::IndexBuffer::Create(indices, 36);
		return new Mesh(va, ib, material);
	}

	Mesh* CreatePlane(float size, const maths::vec3& normal, MaterialInstance* material)
	{
		using namespace maths;

		vec3 vec = normal.Multiply(90.0f);
		mat4 rotation = mat4::Rotate(vec.z, vec3(1, 0, 0)).Multiply(mat4::Rotate(vec.y, vec3(0, 1, 0))).Multiply(mat4::Rotate(vec.x, vec3(0, 0, 1)));

		Vertex data[4];
		memset(data, 0, 4 * sizeof(Vertex));

		vec2 textureScale = vec2(size, size).Divide(128.0f);

		data[0].position = vec3(-size / 2.0f, 0.0f, -size / 2.0f);
		data[0].normal = normal;
		data[0].uv = (vec2) vec2(0.0f, 0.0f).Multiply(textureScale);
		data[0].binormal = mat4::Rotate(90.0f, vec3(0, 1, 0)) * normal;
		data[0].tangent = mat4::Rotate(90.0f, vec3(0, 0, 1)) * normal;
		data[0].weights = vec4(1);
		data[0].indices = vec4(0);

		data[1].position = vec3(-size / 2.0f, 0.0f, size / 2.0f);
		data[1].normal = normal;
		data[1].uv = (vec2) vec2(0.0f, 1.0f).Multiply(textureScale);
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
		layout.Push<vec4>("JOINTWEIGHTS");
		layout.Push<vec4>("JOINTINDICES");
		buffer->SetLayout(layout);

		API::VertexArray* va = API::VertexArray::Create();
		va->PushBuffer(buffer);

		uint* indices = new uint[6]
		{
			0, 1, 2,
			2, 3, 0
		};

		API::IndexBuffer* ib = API::IndexBuffer::Create(indices, 6);

		Mesh* mesh = new Mesh(va, ib, material);

		return mesh;
	}

	Mesh* CreatePlane2(float size, float density, MaterialInstance* material)
	{
		using namespace maths;		

		float vertexCount = size * density;

		uint size2 = vertexCount * vertexCount;

		Vertex* data = spnew Vertex[size2];
		memset(data, 0, size2 * sizeof(Vertex));
		
		for (uint x = 0; x < vertexCount; x++)
		{
			for (uint z = 0; z < vertexCount; z++)
			{
				uint index = (x * (int)vertexCount) + z;
				data[index].position = vec3(x - (vertexCount / 2), 0, z - (vertexCount / 2));
				data[index].normal = vec3(0, 1, 0);
				data[index].uv = vec2(data[index].position.x / vertexCount, data[index].position.z / vertexCount);
				data[index].binormal = mat4::Rotate(90.0f, vec3(0, 1, 0)) * vec3(0, 1, 0);
				data[index].tangent = mat4::Rotate(90.0f, vec3(0, 0, 1)) * vec3(0, 1, 0);
				data[index].weights = vec4(1);
				data[index].indices = vec4(0);
			}

		}

		int size3 = (vertexCount - 1) * (vertexCount - 1);

		uint* indices = new uint[6 * size3];

		int pointer = 0;
		for (int gz = 0; gz < vertexCount - 1; gz++) {
			for (int gx = 0; gx < vertexCount - 1; gx++) {
				int topLeft = (gz * vertexCount) + gx;
				int topRight = topLeft + 1;
				int bottomLeft = ((gz + 1) * vertexCount) + gx;
				int bottomRight = bottomLeft + 1;


				indices[pointer++] = topLeft;
				indices[pointer++] = bottomLeft;
				indices[pointer++] = topRight;
				indices[pointer++] = topRight;
				indices[pointer++] = bottomLeft;
				indices[pointer++] = bottomRight;
			}
		}


		API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
		buffer->SetData(size2 * sizeof(Vertex), data);

		API::BufferLayout layout;
		layout.Push<vec3>("POSITION");
		layout.Push<vec3>("NORMAL");
		layout.Push<vec2>("TEXCOORD");
		layout.Push<vec3>("BINORMAL");
		layout.Push<vec3>("TANGENT");
		layout.Push<vec4>("JOINTWEIGHTS");
		layout.Push<vec4>("JOINTINDICES");
		buffer->SetLayout(layout);

		API::VertexArray* va = API::VertexArray::Create();
		va->PushBuffer(buffer);

		API::IndexBuffer* ib = API::IndexBuffer::Create(indices, 6 * size3);

		Mesh* mesh = new Mesh(va, ib, material);

		return mesh;
	}

	Mesh* CreateSphere(float radius, MaterialInstance* material)
	{
		using namespace maths;

		uint rings = 12;
		uint sectors = 24;

		float const R = 1. / (float)(rings - 1);
		float const S = 1. / (float)(sectors - 1);
		int r, s;

		Vertex* data = spnew Vertex[rings * sectors];
		memset(data, 0, rings * sectors * sizeof(Vertex));

		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			float const y = sin(-(M_PI * M_PI) + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			uint index = (r * rings) + s;
			data[index].position = vec3(x, y, z) * radius;
			data[index].normal = vec3(x, y, z);
			data[index].uv = vec2(s * S, r * R);
			data[index].tangent = vec3(1, 1, 1);
			data[index].binormal = vec3(1, 1, 1);
			data[index].weights = vec4(1, 1, 1, 1);
			data[index].indices = vec4(0, 0, 0, 0);
		}


		API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
		buffer->SetData(rings * sectors * sizeof(Vertex), data);

		API::BufferLayout layout;
		layout.Push<vec3>("POSITION");
		layout.Push<vec3>("NORMAL");
		layout.Push<vec2>("TEXCOORD");
		layout.Push<vec3>("BINORMAL");
		layout.Push<vec3>("TANGENT");
		layout.Push<vec4>("JOINTWEIGHTS");
		layout.Push<vec4>("JOINTINDICES");
		buffer->SetLayout(layout);

		API::VertexArray* va = API::VertexArray::Create();
		va->PushBuffer(buffer);

		std::vector<uint> indices;
		indices.resize(rings * sectors * 4);
		std::vector<uint>::iterator i = indices.begin();

		for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}

		API::IndexBuffer* ib = API::IndexBuffer::Create(&indices[0], rings * sectors * 4);

		Mesh* mesh = new Mesh(va, ib, material);

		return mesh;
	}

} } }