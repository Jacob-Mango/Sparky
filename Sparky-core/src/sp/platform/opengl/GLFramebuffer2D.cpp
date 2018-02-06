#include "sp/sp.h"
#include "GLFramebuffer2D.h"

#include "sp\app\Application.h"

#include "sp/system/Memory.h"
#include "sp/utils/Log.h"

namespace sp { namespace graphics {

	using namespace API;

	GLFramebuffer2D::GLFramebuffer2D(uint width, uint height, uint numberTextures)
		: m_Width(width), m_Height(height), m_NumberTextures(numberTextures)
	{
		Init();
	}

	GLFramebuffer2D::~GLFramebuffer2D()
	{
		GLCall(glDeleteFramebuffers(1, &m_FramebufferHandle));
	}

	void GLFramebuffer2D::Init()
	{
		GLCall(glGenFramebuffers(1, &m_FramebufferHandle));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandle));
		
		GLuint* attachments = new GLuint[m_NumberTextures];
		m_TextureHandles = new GLuint[m_NumberTextures];

		for (unsigned int i = 0; i < m_NumberTextures; i++) {
			glGenTextures(1, &m_TextureHandles[i]);
			glBindTexture(GL_TEXTURE_2D, m_TextureHandles[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, m_TextureHandles[i], 0);
		}

		GLCall(glDrawBuffers(m_NumberTextures, attachments));

		GLCall(glGenRenderbuffers(1, &m_DepthbufferHandle));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthbufferHandle));
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_Width, m_Height));
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthbufferHandle));
		
		GLenum status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

		switch (status)
		{
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			SP_ERROR("OpenGL framebuffer format not supported. ");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			SP_ERROR("OpenGL framebuffer missing attachment.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			SP_ERROR("OpenGL framebuffer attached images must have same dimensions.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			SP_ERROR("OpenGL framebuffer attached images must have same format.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			SP_ERROR("OpenGL framebuffer missing draw buffer.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			SP_ERROR("OpenGL framebuffer missing read buffer.");
			break;
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			SP_INFO("OpenGL framebuffer was created successfully.");
			break;
		default:
			SP_ERROR("OpenGL framebuffer had an unknown error.");
			break;
		}

		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void GLFramebuffer2D::Bind() const
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandle));
		GLCall(glViewport(0, 0, m_Width, m_Height));
	}

	void GLFramebuffer2D::Unbind() const
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glViewport(0, 0, Application::GetApplication().GetWindowWidth(), Application::GetApplication().GetWindowHeight()));
	}

	void GLFramebuffer2D::Clear()
	{
		GLCall(glClearColor(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void GLFramebuffer2D::Render(API::Shader* shader)
	{
		for (unsigned int i = 0; i < m_NumberTextures; i++) {
			GLCall(glActiveTexture(GL_TEXTURE0 + i));
			GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureHandles[i]));
		}
	}

} }