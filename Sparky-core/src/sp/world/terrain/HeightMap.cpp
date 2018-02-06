#include "sp/sp.h"
#include "HeightMap.h"

#include "Sparky.h"

#include <stdlib.h>
#include <vector>

#include "sp/system/Memory.h"
#include "sp/graphics/shaders/ShaderManager.h"
#include "sp/system/VFS.h"

using namespace sp;
using namespace graphics;
using namespace maths;

using namespace events;
using namespace entity;
using namespace component;

using namespace API;

namespace sp { namespace world { 

HeightMap::HeightMap(sp::maths::vec3 offset)
{
	const float size = 32.0f;

	offset *= vec3(size - 1, 1, size - 1);

	using namespace maths;

	float vertexCount = size;

	uint size2 = vertexCount * vertexCount;

	Vertex* data = spnew Vertex[size2];
	memset(data, 0, size2 * sizeof(Vertex));

	for (uint x = 0; x < vertexCount; x++)
	{
		for (uint z = 0; z < vertexCount; z++)
		{
			float height = generateHeight(offset.x + x, offset.z + z);

			uint index = (x * (int)vertexCount) + z;
			data[index].position = vec3(x - (vertexCount / 2), height, z - (vertexCount / 2));
			data[index].normal = vec3(0, 1, 0);
			data[index].uv = vec2(data[index].position.x, data[index].position.z);
			data[index].binormal = mat4::Rotate(90.0f, vec3(0, 1, 0)) * vec3(0, 1, 0);
			data[index].tangent = mat4::Rotate(90.0f, vec3(0, 0, 1)) * vec3(0, 1, 0);
			data[index].jointWeights = vec4(1);
			data[index].jointIndices = vec4(0);
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

	Mesh* mesh = new Mesh(va, ib, spnew PBRMaterialInstance("Bricks_V2"));

	AddComponent(spnew MeshComponent(mesh));

	m_ShouldUpdate = true;
}

void HeightMap::OnInit()
{
}

void HeightMap::OnUpdate(const sp::Timestep& ts)
{
}

float HeightMap::generateHeight(int x, int z) {
	float total = 0;
	float d = (float)pow(2, OCTAVES - 1);
	for (int i = 0; i < OCTAVES; i++) {
		float freq = (float)(pow(2, i) / d);
		float amp = (float)pow(ROUGHNESS, i) * AMPLITUDE;
		total += getInterpolatedNoise(x * freq, z * freq) * amp;
	}
	return total;
}

float HeightMap::getInterpolatedNoise(float x, float z) {
	int intX = (int)x;
	int intZ = (int)z;
	float fracX = x - intX;
	float fracZ = z - intZ;

	float v1 = getSmoothNoise(intX, intZ);
	float v2 = getSmoothNoise(intX + 1, intZ);
	float v3 = getSmoothNoise(intX, intZ + 1);
	float v4 = getSmoothNoise(intX + 1, intZ + 1);
	float i1 = interpolate(v1, v2, fracX);
	float i2 = interpolate(v3, v4, fracX);
	return interpolate(i1, i2, fracZ);
}

float HeightMap::interpolate(float a, float b, float blend) {
	double theta = blend * M_PI;
	float f = (float)(1.0f - cos(theta)) * 0.5f;
	return a * (1.0f - f) + b * f;
}

float HeightMap::getSmoothNoise(int x, int z) {
	float corners = (getNoise(x - 1, z - 1) + getNoise(x + 1, z - 1) + getNoise(x - 1, z + 1) + getNoise(x + 1, z + 1)) / 16.0f;
	float sides = (getNoise(x - 1, z) + getNoise(x + 1, z) + getNoise(x, z - 1) + getNoise(x, z + 1)) / 8.0f;
	float center = getNoise(x, z) / 4.0f;
	return corners + sides + center;
}

float HeightMap::getNoise(int x, int z) {
	long seed = 10000;

	srand(x * 49632 + z * 325176 + seed);
	return ((rand() % 10000) / 10000.0f);
}

} }