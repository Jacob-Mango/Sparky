#pragma once

#include "sp/sp.h"
#include "sp/Common.h"
#include "sp/Types.h"

#include "Material.h"

namespace sp { namespace graphics {
	
	class SP_API MaterialManager
	{
	private:
		static std::vector<Material*> s_Materials;
	public:
		static void Add(Material* material);
		static Material* Get(const String& name);
		static void Clean();
	private:
		MaterialManager() { }
	};

} }