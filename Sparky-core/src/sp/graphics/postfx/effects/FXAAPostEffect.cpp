#include "sp/sp.h"

#include "FXAAPostEffect.h"

#include "sp/system/Memory.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			FXAAPostEffect::FXAAPostEffect() : PostEffectsPass(ShaderManager::Get("FXAA_PEP"))
			{
			}

			FXAAPostEffect::~FXAAPostEffect()
			{
			}


		}
	}
}