#include "sp/sp.h"
#include "SSAOPostEffect.h"

#include "sp/system/Memory.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			SSAOPostEffect::SSAOPostEffect() : PostEffectsPass(ShaderManager::Get("SSAO_PEP"))
			{
			}

			SSAOPostEffect::~SSAOPostEffect()
			{
			}

		}
	}
}