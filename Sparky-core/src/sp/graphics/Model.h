#pragma once

#include "sp/sp.h"
#include "sp/Common.h"
#include "sp/Types.h"

#include "sp/maths/maths.h"

#include "Mesh.h"

namespace sp { namespace graphics {

	class SP_API Model : public IRenderable
	{
	private:
		Mesh* m_Mesh;
	public:
		// This eventually needs to be replaced by a global Asset Server.
		Model(const String& path, MaterialInstance* materialInstance = nullptr);
		~Model();

		void Render(Renderer3D& renderer) override;

		inline Mesh* GetMesh() const { return m_Mesh; }
	private:
		struct VertexSet
		{
			std::vector<maths::vec3> positions, normals;
			std::vector<maths::vec2> uvs;
		};

		// TODO: Replace with uvec3, whenever that begins to exist
		struct IndexSet
		{
			uint position;
			uint uv;
			uint normal;

			bool operator==(const IndexSet& other) const
			{
				return position == other.position && uv == other.uv && normal == other.normal;
			}
		};

		friend struct std::hash<IndexSet>;

		void Load(const String& path);
		void InsertVertex(std::vector<Vertex>& vertices, std::vector<uint>& indices, std::unordered_map<IndexSet, int32>& mapping, VertexSet& inputVertices, IndexSet& indexSet);

		template <typename T>
		T toData(byte* src, uint start, uint end)
		{

		}

		template <>
		float* toData<float*>(byte* src, uint start, uint end)
		{
			uint size = end - start;

			union elementBuffer
			{
				unsigned char buf[64];
				float elements[16];
			} bufEle;


			for (int i = 0; i < size; i++) {
				bufEle.buf[i] = src[start + i];
			}

			return bufEle.elements;
		}

		template <>
		int toData<int>(byte* src, uint start, uint end)
		{
			int result = 0;

#ifdef TEST
			if (true) {
			result =  (src[start] & 0xff000000) << 24
					| (src[start + 1] & 0x00ff0000) << 16
					| (src[start + 2] & 0x0000ff00) << 8
					| (src[start + 3] & 0x000000ff);
			}
			else {
			result =  (src[start] & 0xff) << 24
					| (src[start + 1] & 0xff) << 16
					| (src[start + 2] & 0xff) << 8
					| (src[start + 3] & 0xff);
			}
#endif

			uint size = end - start;

			union elementBuffer
			{
				unsigned char buf[4];
				int i;
			} bufEle;

			for (int i = 0; i < size; i++) {
				bufEle.buf[i] = src[start + i];
			}

			return bufEle.i;
		}

		int toInt(byte* src, uint start)
		{
			return toData<int>(src, start, start + sizeof(int));
		}

		template <>
		String toData<String>(byte* src, uint start, uint end)
		{
			String result = "";
			const uint size = end - start;
			for (int k = 0; k < size; k++) {
				char c = src[start + k];
				result += c;
			}
			return result;
		}

		template <>
		maths::mat4 toData<maths::mat4>(byte* src, uint start, uint end)
		{
			maths::mat4 result;

			const uint size = end - start;
			byte* temp = new byte[size];
			for (int i = 0; i < size; i++)
				temp[i] = src[start + i];

			memcpy(&result, temp, sizeof(maths::mat4));

			return result;
		}
	};

} }