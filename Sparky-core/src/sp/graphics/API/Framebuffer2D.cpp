#include "sp/sp.h"
#include "Framebuffer2D.h"

#include "Context.h"

#include "sp/platform/opengl/GLFramebuffer2D.h"
#include "sp/platform/directx/DXFramebuffer2D.h"

#include "Texture.h"

#include "sp/system/Memory.h"

namespace sp { namespace graphics { namespace API {

	Framebuffer2D* Framebuffer2D::Create(uint width, uint height, uint numberTextures)
	{
		switch (API::Context::GetRenderAPI())
		{
			case API::RenderAPI::OPENGL:	return spnew GLFramebuffer2D(width, height, numberTextures);
			case API::RenderAPI::DIRECT3D:	return spnew D3DFramebuffer2D(width, height, numberTextures);
		}
		return nullptr;
	}

} } }