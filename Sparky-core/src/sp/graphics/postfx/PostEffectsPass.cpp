#include "sp/sp.h"
#include "PostEffectsPass.h"

#include "sp/system/Memory.h"

namespace sp { namespace graphics {

	PostEffectsPass::PostEffectsPass(API::Shader* shader)
	{
		m_Material = spnew Material("mat_" + shader->GetName(), shader);
	}

	PostEffectsPass::~PostEffectsPass()
	{
		spdel m_Material;
	}


} }