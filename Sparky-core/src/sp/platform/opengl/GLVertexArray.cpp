#include "sp/sp.h"
#include "GLVertexArray.h"

#include <GL/glew.h>
#include "sp/utils/Log.h"

namespace sp { namespace graphics { namespace API {

	GLVertexArray::GLVertexArray()
	{
	}

	GLVertexArray::~GLVertexArray()
	{
	}

	void GLVertexArray::PushBuffer(API::VertexBuffer* buffer)
	{
		m_Buffers.push_back(buffer);
	}

	void GLVertexArray::Bind() const
	{
		m_Buffers.front()->Bind();
	}

	void GLVertexArray::Unbind() const
	{
		m_Buffers.front()->Unbind();
	}

	GLenum SPRenderTypeToGL(RenderType type)
	{
		switch (type)
		{
		case TRIANGLES: return GL_TRIANGLES;
		case POINTS: return GL_POINTS;
		case LINES: return GL_LINES;
		default: return GL_TRIANGLES;
		}
	}

	void GLVertexArray::Draw(RenderType type, uint count) const
	{
		// SP_INFO(type == RenderType::TRIANGLES ? "Triangles" : (type == RenderType::POINTS ? "Points" : (type == RenderType::LINES ? "Lines" : "Unknown")));
		GLCall(glDrawElements(SPRenderTypeToGL(type), count, GL_UNSIGNED_INT, NULL));
	}

} } }
