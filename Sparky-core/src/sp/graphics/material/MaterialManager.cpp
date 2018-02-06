#include "sp/sp.h"
#include "MaterialManager.h"

#include "sp/system/Memory.h"

namespace sp { namespace graphics {

	std::vector<Material*> MaterialManager::s_Materials;

	void MaterialManager::Add(Material* shader)
	{
		s_Materials.push_back(shader);
	}

	Material* MaterialManager::Get(const String& name)
	{
		for (Material* material : s_Materials)
		{
			if (material->GetName() == name)
				return material;
		}
		return nullptr;
	}

	void MaterialManager::Clean()
	{
		for (uint i = 0; i < s_Materials.size(); i++)
			spdel s_Materials[i];
	}

} }