#include "sp/sp.h"
#include "PBRMaterial.h"

#include "sp/system/Memory.h"
#include "sp/graphics/shaders/ShaderResource.h"

#include "MaterialManager.h"

namespace sp { namespace graphics {

	using namespace API;

	API::Texture2D* PBRMaterial::s_PreintegratedFG = nullptr;


	PBRMaterial::PBRMaterial(String name, API::Shader* shader)
		: Material(name, shader)
	{
		SetUniform("u_UsingAlbedoMap", 0.0f);
		SetUniform("u_AlbedoColor", maths::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		SetUniform("u_MetallicColor", maths::vec3(1.0f, 1.0f, 1.0f));
		SetUniform("u_UsingMetallicMap", 0.0f);

		SetUniform("u_UsingNormalMap", 0.0f);

		TextureParameters tp = TextureParameters();
		tp.wrap = TextureWrap::REPEAT;
		tp.filter = TextureFilter::LINEAR;

		SetAlbedoMap(Texture2D::CreateFromFile("/materials/" + name + "/" + name + "_Albedo.tga", tp));
		SetMetallicMap(Texture2D::CreateFromFile("/materials/" + name + "/" + name + "_Metallic.tga", tp));
		SetNormalMap(Texture2D::CreateFromFile("/materials/" + name + "/" + name + "_Normal.tga", tp));
	}

	PBRMaterial::~PBRMaterial()
	{
		spdel m_Shader;
	}

	void PBRMaterial::SetEnviromentMap(API::TextureCube* texture)
	{
		SetTexture("u_EnvironmentMap", texture);
	}

	void PBRMaterial::SetAlbedo(const maths::vec4& color)
	{
		SetUniform("u_AlbedoColor", color);
		SetUniform("u_UsingAlbedoMap", 0.0f);
	}

	void PBRMaterial::SetMetallic(const maths::vec3& color)
	{
		SetUniform("u_MetallicColor", color);
		SetUniform("u_UsingMetallicMap", 0.0f);
	}

	void PBRMaterial::UsingNormalMap(bool value)
	{
		SetUniform("u_UsingNormalMap", value ? 1.0f : 0.0f);
	}

	void PBRMaterial::SetAlbedoMap(API::Texture2D* texture)
	{
		SetTexture("u_AlbedoMap", texture);
		SetUniform("u_UsingAlbedoMap", 1.0f);
	}

	void PBRMaterial::SetMetallicMap(API::Texture2D* texture)
	{
		SetTexture("u_MetallicMap", texture);
		SetUniform("u_UsingMetallicMap", 1.0f);
	}

	void PBRMaterial::SetNormalMap(API::Texture2D* texture)
	{
		SetTexture("u_NormalMap", texture);
		SetUniform("u_UsingNormalMap", 1.0f);
	}

	API::Texture* PBRMaterial::GetAlbedoMap()
	{
		ShaderResourceDeclaration* declaration = FindResourceDeclaration("u_AlbedoMap");
		SP_ASSERT(declaration);
		uint slot = declaration->GetRegister();
		return m_Textures.size() > slot ? m_Textures[slot] : nullptr;
	}

	API::Texture* PBRMaterial::GetMetallicMap()
	{
		ShaderResourceDeclaration* declaration = FindResourceDeclaration("u_MetallicMap");
		SP_ASSERT(declaration);
		uint slot = declaration->GetRegister();
		return m_Textures.size() > slot ? m_Textures[slot] : nullptr;
	}

	API::Texture* PBRMaterial::GetNormalMap()
	{
		ShaderResourceDeclaration* declaration = FindResourceDeclaration("u_NormalMap");
		SP_ASSERT(declaration);
		uint slot = declaration->GetRegister();
		return m_Textures.size() > slot ? m_Textures[slot] : nullptr;
	}

	PBRMaterialInstance::PBRMaterialInstance(PBRMaterial* material)
		: MaterialInstance(material)
	{
	}

	PBRMaterialInstance::PBRMaterialInstance(String name)
		: MaterialInstance(MaterialManager::Get(name))
	{
	}

	void PBRMaterialInstance::SetEnviromentMap(API::TextureCube* texture)
	{
		SetTexture("u_EnvironmentMap", texture);
	}

	void PBRMaterialInstance::SetAlbedo(const maths::vec4& color)
	{
		SetUniform("u_AlbedoColor", color);
		SetUniform("u_UsingAlbedoMap", 0.0f);
	}

	void PBRMaterialInstance::SetMetallic(const maths::vec3& color)
	{
		SetUniform("u_MetallicColor", color);
		SetUniform("u_UsingMetallicMap", 0.0f);
	}

	void PBRMaterialInstance::UsingNormalMap(bool value)
	{
		SetUniform("u_UsingNormalMap", value ? 1.0f : 0.0f);
	}

	void PBRMaterialInstance::SetAlbedoMap(API::Texture2D* texture)
	{
		SetTexture("u_AlbedoMap", texture);
		SetUniform("u_UsingAlbedoMap", 1.0f);
	}

	void PBRMaterialInstance::SetMetallicMap(API::Texture2D* texture)
	{
		SetTexture("u_MetallicMap", texture);
		SetUniform("u_UsingMetallicMap", 1.0f);
	}

	void PBRMaterialInstance::SetNormalMap(API::Texture2D* texture)
	{
		SetTexture("u_NormalMap", texture);
		SetUniform("u_UsingNormalMap", 1.0f);
	}

} }