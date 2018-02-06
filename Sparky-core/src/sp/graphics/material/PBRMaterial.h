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
		~PBRMaterial();

		void SetEnviromentMap(API::TextureCube* texture);

		// PBR Statics
		void SetAlbedo(const maths::vec4& color);
		void SetSpecular(const maths::vec3& color);
		void SetGloss(float value);
		void UsingNormalMap(bool value);

		// PBR Maps
		void SetAlbedoMap(API::Texture2D* texture);
		void SetSpecularMap(API::Texture2D* texture);
		void SetNormalMap(API::Texture2D* texture);
		void SetGlossMap(API::Texture2D* texture); // TODO: Grayscale texture

		API::Texture* GetAlbedoMap();
		API::Texture* GetSpecularMap();
		API::Texture* GetNormalMap();
		API::Texture* GetGlossMap();
	};

	class SP_API PBRMaterialInstance : public MaterialInstance
	{
	public:
		PBRMaterialInstance(PBRMaterial* material);
		PBRMaterialInstance(String name);

		void SetEnviromentMap(API::TextureCube* texture);

		// PBR Statics
		void SetAlbedo(const maths::vec4& color);
		void SetSpecular(const maths::vec3& color);
		void SetGloss(float value);
		void UsingNormalMap(bool value);

		// PBR Maps
		void SetAlbedoMap(API::Texture2D* texture);
		void SetSpecularMap(API::Texture2D* texture);
		void SetNormalMap(API::Texture2D* texture);
		void SetGlossMap(API::Texture2D* texture); // TODO: Grayscale texture
	};

} }
#endif // !PBR_MATERIAL_H