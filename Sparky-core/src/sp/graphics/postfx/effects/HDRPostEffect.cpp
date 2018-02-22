#include "sp/sp.h"

#include "HDRPostEffect.h"

#include "sp/system/Memory.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			HDRPostEffect::HDRPostEffect() : PostEffectsPass(ShaderManager::Get("HDR_PEP"))
			{
			}

			HDRPostEffect::~HDRPostEffect()
			{
			}


		}
	}
}