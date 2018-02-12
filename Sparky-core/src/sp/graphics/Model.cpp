#include "sp/sp.h"
#include "Model.h"

#include <GL/glew.h>

#include "sp/system/Memory.h"
#include "sp/graphics/shaders/ShaderManager.h"
#include "sp/system/VFS.h"

#include "Renderer3D.h"

namespace sp { namespace graphics {

	using namespace maths;

	enum VertexAttribute
	{
		POSITION = BIT(0),
		NORMAL = BIT(1),
		UV = BIT(2),
		BINORMAL = BIT(3),
		TANGENT = BIT(4),
		JOINTWEIGHT = BIT(5),
		JOINTINDEX = BIT(6),
	};

	struct SPMFormat
	{
		char* header = "SPMF";
		byte nameLength;
		char* name;
		uint attributes;
		uint vertexBufferSize;
		byte* vertexData;
		uint indexBufferSize;
		byte* indexData;
		uint boneInfoBufferSize;
		byte* boneInfoData;
		char* footer = "1234";
	};

	Model::Model(const String& path, MaterialInstance* materialInstance)
	{
		Load(path);
		m_Mesh->SetMaterial(materialInstance);
	}

	Model::~Model()
	{
		delete m_Mesh;
	}

	byte* ReadBytes(FILE* file, byte* buffer, uint size)
	{
		fread(buffer, 1, size, file);
		return buffer;
	}

	void Model::Load(const String& path)
	{
		// TODO: Change to VFS::OpenMemoryMap()
		String physicalPath;
		VFS::Get()->ResolvePhysicalPath(path, physicalPath);
		SPMFormat format;
		FILE* f = fopen(physicalPath.c_str(), "rb");
		SP_ASSERT(f);
		{
			byte header[4];
			ReadBytes(f, header, 4);
			SP_ASSERT(memcmp(header, format.header, 4) == 0);
		}

		{
			byte buffer[1];
			ReadBytes(f, buffer, 1);
			format.nameLength = *buffer;
		}

		{
			format.name = new char[format.nameLength + 1];
			ReadBytes(f, (byte*)format.name, format.nameLength);
			format.name[format.nameLength] = '\0';
		}

		{
			byte buffer[4];
			ReadBytes(f, buffer, 4);
			format.attributes = *(uint*)buffer;
		}

		{
			byte buffer[4];
			ReadBytes(f, buffer, 4);
			format.vertexBufferSize = *(uint*)buffer;
		}

		{
			format.vertexData = spnew byte[format.vertexBufferSize];
			ReadBytes(f, format.vertexData, format.vertexBufferSize);
		}

		{
			byte buffer[4];
			ReadBytes(f, buffer, 4);
			format.indexBufferSize = *(uint*)buffer;
		}

		{
			format.indexData = spnew byte[format.indexBufferSize];
			ReadBytes(f, format.indexData, format.indexBufferSize);
		}

		{
			byte buffer[4];
			ReadBytes(f, buffer, 4);
			format.boneInfoBufferSize = *(uint*)buffer;
		}

		{
			format.boneInfoData = spnew byte[format.boneInfoBufferSize];
			ReadBytes(f, format.boneInfoData, format.boneInfoBufferSize);
		}

		{
			byte footer[4];
			ReadBytes(f, footer, 4);
			SP_ASSERT(memcmp(footer, format.footer, 4) == 0);
		}

		fclose(f);

		ShaderManager::Get("Default")->Bind();

		API::VertexBuffer* buffer = API::VertexBuffer::Create(API::BufferUsage::STATIC);
		buffer->SetData(format.vertexBufferSize, format.vertexData);

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

		API::IndexBuffer* ib = API::IndexBuffer::Create((uint*)format.indexData, format.indexBufferSize / sizeof(uint));

		Vertices* vertices = spnew Vertices{ (graphics::Vertex*)format.vertexData, format.vertexBufferSize / sizeof(Vertex) };

		std::vector<Bone*> bones;
		std::map<Bone*, int> parents;
		bones.reserve(NUMBONES);

		for (int i = 0; i < format.boneInfoBufferSize; i)
		{
			Bone* bone = spnew Bone();


			uint id = toInt(format.boneInfoData, i);
			i += 4;
			bone->ID = id;

			uint parentIndex = toInt(format.boneInfoData, i);
			i += 4;
			parents[bone] = parentIndex;

			uint nameSize = toInt(format.boneInfoData, i);
			i += 4;
			bone->Name = toData<String>(format.boneInfoData, i, i + nameSize);
			i += nameSize;

			uint boneOffsetSize = toInt(format.boneInfoData, i);
			i += 4;

			

			float* elements = toData<float*>(format.boneInfoData, i, i + boneOffsetSize);
			
			bone->Offset = mat4(elements);

			i += boneOffsetSize;

			bones.push_back(bone);
		}

		for (int i = 0; i < bones.size(); i++) {
			Bone* currentBone = bones[i];
			uint pid = parents[currentBone];
			if (pid < 0 || pid >= NUMBONES) continue;
			if (currentBone->Parent != nullptr) continue;

			Bone* parent = bones[pid];
			currentBone->Parent = parent;
			currentBone->Parent->Children.push_back(currentBone);
		}

		Bone* root = nullptr;
		for (int i = 0; i < bones.size(); i++) {
			if (bones[i]->Parent == nullptr) {
				root = bones[i];
				break;
			}
		}

		m_Mesh = spnew Mesh(va, ib, nullptr, root);

#ifdef SP_DEBUG
		m_Mesh->SetDebugData((Vertex*)format.vertexData, format.vertexBufferSize / sizeof(Vertex));
#else
#endif
	}

	void Model::Render(Renderer3D& renderer)
	{
		m_Mesh->Render(renderer);
	}

} }