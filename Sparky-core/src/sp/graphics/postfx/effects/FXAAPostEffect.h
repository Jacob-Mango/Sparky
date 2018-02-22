#pragma once

#include "../PostEffectsPass.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			class SP_API FXAAPostEffect : public PostEffectsPass
			{
			private:
			public:
				FXAAPostEffect();
				~FXAAPostEffect();

				//virtual void PreRenderPass(API::Framebuffer* source, API::Framebuffer* target) { target = source; };
				//virtual void PostRenderPass(API::Framebuffer* source, API::Framebuffer* target);
			};

		}
	}
}