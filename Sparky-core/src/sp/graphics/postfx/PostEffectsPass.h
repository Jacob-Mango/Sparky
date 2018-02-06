#pragma once

#include "sp/Common.h"
#include "sp/graphics/API/Framebuffer.h"
#include "../material/Material.h"

namespace sp { namespace graphics {

	class SP_API PostEffectsPass
	{
	private:
		Material* m_Material;
	public:
		PostEffectsPass(API::Shader* shader);
		~PostEffectsPass();

		void RenderPass(API::Framebuffer* target);
	};

} }