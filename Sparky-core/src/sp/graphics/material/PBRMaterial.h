#pragma once

#include "Material.h"

#ifndef PBR_MATERIAL_H
#define PBR_MATERIAL_H

namespace sp { namespace graphics {

	class SP_API PBRMaterial : public Material
	{
	private:
		static API::Texture2D* s_PreintegratedFG;
	public:
		PBRMaterial(String name, API::Shader* shader);
		PBRMaterial(String name, String shader = DEFAULT_SHADER_NAME);
		~PBRMaterial();
		
		void Init();

		void SetEnviromentMap(API::TextureCube* texture);

		// PBR Statics
		void SetAlbedo(const maths::vec4& color);
		void SetMetallic(const maths::vec3& color);
		void UsingNormalMap(bool value);

		// PBR Maps
		void SetAlbedoMap(API::Texture2D* texture);
		void SetMetallicMap(API::Texture2D* texture);
		void SetNormalMap(API::Texture2D* texture);

		API::Texture* GetAlbedoMap();
		API::Texture* GetMetallicMap();
		API::Texture* GetNormalMap();
	};

	class SP_API PBRMaterialInstance : public MaterialInstance
	{
	public:
		PBRMaterialInstance(PBRMaterial* material);
		PBRMaterialInstance(String name);

		void SetEnviromentMap(API::TextureCube* texture);

		// PBR Statics
		void SetAlbedo(const maths::vec4& color);
		void SetMetallic(const maths::vec3& color);
		void UsingNormalMap(bool value);

		// PBR Maps
		void SetAlbedoMap(API::Texture2D* texture);
		void SetMetallicMap(API::Texture2D* texture);
		void SetNormalMap(API::Texture2D* texture);
	};

} }
#endif // !PBR_MATERIAL_H