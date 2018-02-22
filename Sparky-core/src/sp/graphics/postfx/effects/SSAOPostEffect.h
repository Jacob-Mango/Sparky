#pragma once

#include "../PostEffectsPass.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			class SP_API SSAOPostEffect : public PostEffectsPass
			{
			private:
			public:
				SSAOPostEffect();
				~SSAOPostEffect();

				//void PreRenderPass(API::Framebuffer* source, API::Framebuffer* target) { target = source; };
				//void PostRenderPass(API::Framebuffer* source, API::Framebuffer* target);
			};

		}
	}
}