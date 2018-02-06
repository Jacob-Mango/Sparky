#include "sp/sp.h"
#include "FramebufferDepth.h"

#include "Context.h"

#include "sp/platform/opengl/GLFramebufferDepth.h"
#include "sp/platform/directx/DXFramebufferDepth.h"

#include "sp/system/Memory.h"

namespace sp { namespace graphics { namespace API {

	FramebufferDepth* FramebufferDepth::Create(uint width, uint height, uint numberTextures)
	{
		switch (API::Context::GetRenderAPI())
		{
			case API::RenderAPI::OPENGL:	return spnew GLFramebufferDepth(width, height, numberTextures);
			case API::RenderAPI::DIRECT3D:	return spnew D3DFramebufferDepth(width, height, numberTextures);
		}
		return nullptr;
	}

	
} } }