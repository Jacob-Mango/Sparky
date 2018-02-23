#pragma once

#include "sp/Common.h"
#include "API/RenderType.h"
#include "API/VertexArray.h"
#include "API/IndexBuffer.h"
#include "material/Material.h"

#include "IRenderable.h"

namespace sp {
	namespace graphics {

		class Renderer3D;
		enum RenderType;

		struct SP_API Vertex
		{
			maths::vec3 position;
			maths::vec3 normal;
			maths::vec2 uv;
			maths::vec3 binormal;
			maths::vec3 tangent;
			maths::vec4 weights;
			maths::vec4 indices;
		};

		struct SP_API Vertices
		{
			Vertex* data;
			uint length;
		};

		struct SP_API Bone
		{
		public:
			uint ID;
			Bone* Parent;
			std::vector<Bone*> Children;
			String Name;
			maths::mat4 Offset;

			Bone(String name, uint id, Bone* parent, maths::mat4 offset = maths::mat4::Identity())
				: Name(name), ID(id), Offset(offset), Parent(parent)
			{
			}
			
			Bone()
				: Name("root"), ID(0), Offset(maths::mat4::Identity()), Parent(nullptr)
			{
			}

			uint ParentID() {
				return Parent == nullptr ? -1 : Parent->ID;
			}
		};

		class SP_API Mesh : public IRenderable
		{
		public:
			API::VertexArray* m_VertexArray;
			API::IndexBuffer* m_IndexBuffer;
			MaterialInstance* m_MaterialInstance;

			Vertices* m_Verts;
			uint* m_Indices;
			uint m_IndexLength;

			Bone* m_RootBone;

			RenderType m_RenderType;

#ifdef SP_DEBUG
		private:
			Vertex * m_DebugVertexData;
			uint m_DebugVertexDataCount;
			bool m_DebugDraw;
#endif
		public:
			Mesh(API::VertexArray* vertexArray, API::IndexBuffer* indexBuffer, MaterialInstance* materialInstance, RenderType renderType = RenderType::TRIANGLES, Bone* rootBone = spnew Bone());
			Mesh(const Mesh* mesh);
			~Mesh();

			inline void SetMaterial(MaterialInstance* materialInstance) { m_MaterialInstance = materialInstance; }
			inline MaterialInstance* GetMaterialInstance() const { return m_MaterialInstance; }

			void Render(Renderer3D& renderer) override;

#ifdef SP_DEBUG
			void DebugRender(const maths::mat4& transform);

			inline void SetDebugData(Vertex* vertices, uint count) { m_DebugVertexData = vertices; m_DebugVertexDataCount = count; }
			inline uint GetDebugData(Vertex*& vertices) const { vertices = m_DebugVertexData; return m_DebugVertexDataCount; }

			inline void SetDebugDraw(bool debug) { m_DebugDraw = debug; }
#endif

			inline bool HasRoot() { return m_RootBone != nullptr; };
		};

	}
}
