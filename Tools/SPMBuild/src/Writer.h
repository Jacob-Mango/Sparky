#pragma once

#include <iostream>
#include <vector>

#include "Utils.h"

struct Bone;
struct Vertex;

class MeshEntry;

namespace sp { namespace tools {

	enum VertexAttribute
	{
		POSITION	= BIT(0),
		NORMAL		= BIT(1),
		UV			= BIT(2),
		BINORMAL	= BIT(3),
		TANGENT		= BIT(4),
		JOINTWEIGHT	= BIT(5),
		JOINTINDEX	= BIT(6)
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

	class Writer
	{
	private:
		String m_Name;
		uint m_Attributes;
		std::vector<Vertex> m_VertexBuffer;
		std::vector<uint> m_IndexBuffer;
		SPMFormat m_Format;
	public:
		Writer(String name, uint attributes, std::vector<MeshEntry> meshEntries, const std::vector<Bone*>& boneInfo);
		~Writer();

		void WriteBytes(FILE* file, const byte* data, uint size);
		void Write(const String& file);
	};

} }
