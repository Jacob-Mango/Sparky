#pragma once

#include "../PostEffectsPass.h"

namespace sp {
	namespace graphics {
		namespace postfx {

			class SP_API HDRPostEffect : public PostEffectsPass
			{
			private:
				int m_Exponent;
			public:
				HDRPostEffect();
				~HDRPostEffect();

				//virtual void PreRenderPass(API::Framebuffer* source, API::Framebuffer* target) { target = source; };
				//virtual void PostRenderPass(API::Framebuffer* source, API::Framebuffer* target);
			};

		}
	}
}