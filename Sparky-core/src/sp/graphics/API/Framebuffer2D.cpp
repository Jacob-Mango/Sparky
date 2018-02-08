#include "sp/sp.h"
#include "Framebuffer2D.h"

#include "Context.h"

#include "sp/platform/opengl/GLFramebuffer2D.h"
#include "sp/platform/directx/DXFramebuffer2D.h"

#include "Texture.h"

#include "sp/system/Memory.h"

namespace sp { namespace graphics { namespace API {

	FrameBuffer2D* FrameBuffer2D::Create(uint width, uint height, std::vector<TextureParameters> parameters)
	{
		switch (API::Context::GetRenderAPI())
		{
			case API::RenderAPI::OPENGL:	return spnew GLFramebuffer2D(width, height, parameters);
			case API::RenderAPI::DIRECT3D:	return spnew D3DFramebuffer2D(width, height, parameters);
		}
		return nullptr;
	}

} } }