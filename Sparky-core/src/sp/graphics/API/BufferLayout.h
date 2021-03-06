#pragma once

#include "sp/sp.h"
#include "sp/Common.h"
#include "sp/Types.h"
#include "sp/maths/maths.h"

#include "Context.h"

#include "sp/platform/opengl/GLTypes.h"
#include "sp/platform/directx/DXTypes.h"

#include "RenderType.h"

namespace sp { namespace graphics { namespace API {

	struct SP_API BufferElement
	{
		String name;
		uint type;
		uint size;
		uint count;
		uint offset;
		bool normalized;
	};

	class SP_API BufferLayout
	{
	private:
		uint m_Size;
		std::vector<BufferElement> m_Layout;

		RenderType m_RenderType;
	public:
		BufferLayout(RenderType renderType = RenderType::TRIANGLES);

		RenderType GetRenderType();

		template<typename T>
		void Push(const String& name, uint count = 1, bool normalized = false)
		{
			SP_ASSERT(false, "Unknown type!");
		}

		template<>
		void Push<float>(const String& name, uint count, bool normalized)
		{
			switch (Context::GetRenderAPI())
			{
				case RenderAPI::OPENGL:
					Push(name, GL_FLOAT, sizeof(float), count, normalized);
					break;
				case RenderAPI::DIRECT3D:
					Push(name, DX_TYPE_R32_FLOAT, sizeof(float), count, normalized);
					break;
			}
		}

		template<>
		void Push<uint>(const String& name, uint count, bool normalized)
		{
			switch (Context::GetRenderAPI())
			{
				case RenderAPI::OPENGL:
					Push(name, GL_UNSIGNED_INT, sizeof(uint), count, normalized);
					break;
				case RenderAPI::DIRECT3D:
					Push(name, DX_TYPE_R32_UINT, sizeof(uint), count, normalized);
					break;
			}
		}

		template<>
		void Push<byte>(const String& name, uint count, bool normalized)
		{
			switch (Context::GetRenderAPI())
			{
				case RenderAPI::OPENGL:
					Push(name, GL_UNSIGNED_BYTE, sizeof(byte), count, normalized);
					break;
				case RenderAPI::DIRECT3D:
					Push(name, DX_TYPE_R8G8B8A8_UNORM, sizeof(byte) * 4, 1, normalized);
					break;
			}
		}

		template<>
		void Push<maths::vec2>(const String& name, uint count, bool normalized)
		{
			switch (Context::GetRenderAPI())
			{
				case RenderAPI::OPENGL:
					Push(name, GL_FLOAT, sizeof(float), 2, normalized);
					break;
				case RenderAPI::DIRECT3D:
					Push(name, DX_TYPE_R32G32_FLOAT, sizeof(maths::vec2), 2, normalized);
					break;
			}
		}

		template<>
		void Push<maths::vec3>(const String& name, uint count, bool normalized)
		{
			switch (Context::GetRenderAPI())
			{
				case RenderAPI::OPENGL:
					Push(name, GL_FLOAT, sizeof(float), 3, normalized);
					break;
				case RenderAPI::DIRECT3D:
					Push(name, DX_TYPE_R32G32B32_FLOAT, sizeof(maths::vec3), 3, normalized);
					break;
			}
		}

		template<>
		void Push<maths::vec4>(const String& name, uint count, bool normalized)
		{
			switch (Context::GetRenderAPI())
			{
				case RenderAPI::OPENGL:
					Push(name, GL_FLOAT, sizeof(float), 4, normalized);
					break;
				case RenderAPI::DIRECT3D:
					Push(name, DX_TYPE_R32G32B32A32_FLOAT, sizeof(maths::vec4), 4, normalized);
					break;
			}
		}

		template<>
		void Push<maths::mat4>(const String& name, uint count, bool normalized)
		{
			switch (Context::GetRenderAPI())
			{
			case RenderAPI::OPENGL:
				Push(name, GL_FLOAT, sizeof(float), 16, normalized);
				break;
			case RenderAPI::DIRECT3D:
				//Push(name, DX_TYPE_R32G32B32A32_FLOAT, sizeof(maths::vec4), 4, normalized);
				break;
			}
		}

		inline const std::vector<BufferElement>& GetLayout() const { return m_Layout; }
		inline uint GetStride() const { return m_Size; }
	private:
		void Push(const String& name, uint type, uint size, uint count, bool normalized);
	};


} } }