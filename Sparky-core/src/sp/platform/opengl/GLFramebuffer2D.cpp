#include "sp/sp.h"
#include "GLFramebuffer2D.h"

#include "sp\app\Application.h"

#include "sp/system/Memory.h"
#include "sp/utils/Log.h"

namespace sp { namespace graphics {

	using namespace API;

	GLFramebuffer2D::GLFramebuffer2D(uint width, uint height, std::vector<TextureParameters> parameters)
		: m_Width(width), m_Height(height), m_Parameters(parameters)
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

		GLuint* attachments = spnew GLuint[m_Parameters.size()];
		
		for (unsigned int i = 0; i < m_Parameters.size(); i++) {
			GLTexture2D* texture = spnew GLTexture2D(m_Width, m_Height, m_Parameters[i], TextureLoadOptions());
			attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, texture->GetHandle(), 0));
			m_Textures.push_back((Texture2D*)texture);
		}

		GLCall(glDrawBuffers(m_Parameters.size(), attachments));

		GLCall(glGenRenderbuffers(1, &m_DepthbufferHandle));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthbufferHandle));
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height));
		// GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_Width, m_Height));
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthbufferHandle));
		
		GLenum status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

		switch (status)
		{
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			SP_ERROR("OpenGL: Framebuffers are not supported/enabled on your system.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			SP_ERROR("OpenGL: Missing a framebuffer attachment.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			SP_ERROR("OpenGL: All framebuffer attached textures must have the same dimensions.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			SP_ERROR("OpenGL: All framebuffer attached textures must have same the format.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			SP_ERROR("OpenGL: The framebuffer is missing a draw buffer.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			SP_ERROR("OpenGL: The framebuffer is missing a read buffer.");
			break;
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			SP_INFO("OpenGL: The framebuffer was created successfully.");
			break;
		default:
			SP_ERROR("OpenGL: An unknown error occurred when creating the framebuffer object.");
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

} }