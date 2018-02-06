#include "sp/sp.h"

#include "sp\app\Application.h"

#include "GLFramebufferDepth.h"

#include "sp/system/Memory.h"

#include <GL/glew.h>
#include "sp/utils/Log.h"

namespace sp { namespace graphics { namespace API {

	using namespace API;

	GLFramebufferDepth::GLFramebufferDepth(uint width, uint height, uint numberTextures)
		: m_Width(width), m_Height(height), m_NumberTextures(numberTextures)
	{
		Init();
	}

	GLFramebufferDepth::~GLFramebufferDepth()
	{
		GLCall(glDeleteFramebuffers(1, &m_FramebufferHandle));
	}

	void GLFramebufferDepth::Init()
	{
		GLCall(glGenFramebuffers(1, &m_FramebufferHandle));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandle));

		std::vector<GLuint> attachments;

		//for (int i = 0; i < m_TextureParameters.size(); i++) {
		//	GLTexture2D* texture = spnew GLTexture2D(m_Width, m_Height, m_TextureParameters[i]);

		//	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GetHandle(), 0));

		//	attachments.push_back(GL_COLOR_ATTACHMENT0 + i);

		//	m_Textures.push_back(texture);
		//}

		GLCall(glDrawBuffers(attachments.size(), &attachments[0]));

		GLCall(glGenRenderbuffers(1, &m_DepthbufferHandle));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthbufferHandle));
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height));
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthbufferHandle));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			SP_ERROR("Framebuffer not successfully created!");
		}

		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void GLFramebufferDepth::Bind() const
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandle));
		GLCall(glViewport(0, 0, m_Width, m_Height));
	}

	void GLFramebufferDepth::Unbind() const
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glViewport(0, 0, Application::GetApplication().GetWindowWidth(), Application::GetApplication().GetWindowHeight()));
	}

	void GLFramebufferDepth::Clear()
	{
		GLCall(glClearColor(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void GLFramebufferDepth::Render(API::Shader* shader)
	{

	}

} } }