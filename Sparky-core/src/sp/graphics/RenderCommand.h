#pragma once

#include "sp/Common.h"
#include "Mesh.h"

namespace sp { namespace graphics {

	struct SP_API RendererUniform
	{
		String uniform;
		byte* value;
	};

	struct SP_API RenderCommand
	{
		Mesh* mesh;
		maths::mat4 transform;
		API::Shader* shader;
		maths::mat4 bones[64];
		std::vector<RendererUniform> uniforms;
	};

} }
