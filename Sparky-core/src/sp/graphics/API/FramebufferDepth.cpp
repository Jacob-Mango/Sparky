#include "sp/sp.h"
#include "FramebufferDepth.h"

#include "Context.h"

#include "sp/platform/opengl/GLFramebufferDepth.h"
#include "sp/platform/directx/DXFramebufferDepth.h"

#include "sp/system/Memory.h"

namespace sp { namespace graphics { namespace API {

	FrameBufferDepth* FrameBufferDepth::Create(uint width, uint height, std::vector<TextureParameters> parameters)
	{
		switch (API::Context::GetRenderAPI())
		{
			case API::RenderAPI::OPENGL:	return spnew GLFramebufferDepth(width, height, parameters);
			case API::RenderAPI::DIRECT3D:	return spnew D3DFramebufferDepth(width, height, parameters);
		}
		return nullptr;
	}

	
} } }