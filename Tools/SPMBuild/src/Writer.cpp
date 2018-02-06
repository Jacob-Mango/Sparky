#include "Writer.h"

#include <stdlib.h>

#include <iostream>
#include <string>

#include "Mesh.h"

namespace sp { namespace tools {

	void addtoArrayI(std::vector<byte> &dst, int src) {
		union elementBuffer
		{
			unsigned char buf[4];
			int i;
		} bufEle;

		bufEle.i = src;

		for (int i = 0; i < 4; i++) {
			dst.push_back(bufEle.buf[i]);
		}
	}

	void addMat4(std::vector<byte> &dst, float* src) {
		addtoArrayI(dst, 64);

		union elementBuffer
		{
			unsigned char buf[64];
			float elements[16];
		} bufEle;

		for (int i = 0; i < 16; i++) {
			bufEle.elements[i] = src[i];
		}

		for (int i = 0; i < 64; i++) {
			dst.push_back(bufEle.buf[i]);
		}
	}

	void addtoArray(std::vector<byte> &dst, byte* src, int srcLength) {
		addtoArrayI(dst, srcLength);

		for (int i = 0; i < srcLength; i++) {
			dst.push_back(src[i]);
		}
	}

	void addtoArray(std::vector<byte> &dst, byte src) {
		dst.push_back(src);
	}

	Writer::Writer(String name, uint attributes, std::vector<MeshEntry> meshEntries, const std::vector<Bone*>& boneInfo)
		: m_Name(name), m_Attributes(attributes)
	{

		for (int i = 0; i < 1; i++) {

			for (int j = 0; j < meshEntries[i].Indices->size(); j++)
				m_IndexBuffer.push_back(meshEntries[i].Indices->at(j));

			for (int j = 0; j < meshEntries[i].Vertices->size(); j++)
				m_VertexBuffer.push_back(meshEntries[i].Vertices->at(j));

		}

		m_Format.nameLength = name.length();
		m_Format.name = &m_Name[0];
		m_Format.attributes = attributes;
		m_Format.vertexBufferSize = m_VertexBuffer.size() * sizeof(Vertex);
		m_Format.vertexData = (byte*)&m_VertexBuffer[0];
		m_Format.indexBufferSize = m_IndexBuffer.size() * sizeof(uint);
		m_Format.indexData = (byte*)&m_IndexBuffer[0];

		std::vector<byte> boneInfoVector;

		for (int i = 0; i < boneInfo.size(); i++)
		{
			Bone* bone = boneInfo[i];

			char const *c = bone->Name.c_str();

			const uint size = 4 + 4 + 4 + (bone->Name.length() * sizeof(c)) + sizeof(mat4);

			addtoArrayI(boneInfoVector, bone->Index);
			addtoArrayI(boneInfoVector, bone->ParentIndex);
			addtoArray(boneInfoVector, (byte*)c, bone->Name.length() * sizeof(c));

			addMat4(boneInfoVector, bone->BoneOffset.elements);
		}

		m_Format.boneInfoBufferSize = boneInfoVector.size() * sizeof(byte);

		m_Format.boneInfoData = new byte[m_Format.boneInfoBufferSize];
		for (int i = 0; i < m_Format.boneInfoBufferSize; i++) {
			m_Format.boneInfoData[i] = boneInfoVector[i];
		}

	}

	Writer::~Writer()
	{
	}

	void Writer::WriteBytes(FILE* file, const byte* data, uint size)
	{
		fwrite(data, 1, size, file);
	}

	void Writer::Write(const String& file)
	{
		const SPMFormat& format = m_Format;

		FILE* f = fopen(file.c_str(), "wb");

		WriteBytes(f, (byte*)format.header, 4);

		WriteBytes(f, &format.nameLength, 1);
		WriteBytes(f, (byte*)format.name, format.nameLength);

		WriteBytes(f, (byte*)&format.attributes, sizeof(uint));

		WriteBytes(f, (byte*)&format.vertexBufferSize, sizeof(uint));
		WriteBytes(f, format.vertexData, format.vertexBufferSize);

		WriteBytes(f, (byte*)&format.indexBufferSize, sizeof(uint));
		WriteBytes(f, format.indexData, format.indexBufferSize);

		WriteBytes(f, (byte*)&format.boneInfoBufferSize, sizeof(uint));
		WriteBytes(f, format.boneInfoData, format.boneInfoBufferSize);

		WriteBytes(f, (byte*)format.footer, 4);

		fclose(f);
	}

} }