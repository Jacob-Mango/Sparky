#include "sp/sp.h"
#include "BufferLayout.h"

namespace sp { namespace graphics { namespace API {

	BufferLayout::BufferLayout(RenderType renderType)
		: m_Size(0), m_RenderType(renderType)
	{
	}

	RenderType BufferLayout::GetRenderType()
	{
		return m_RenderType;
	}

	void BufferLayout::Push(const String& name, uint type, uint size, uint count, bool normalized)
	{
		m_Layout.push_back({ name, type, size, count, m_Size, normalized });
		m_Size += size * count;
	}

} } }