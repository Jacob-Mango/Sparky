#include "sp/sp.h"
#include "MaterialManager.h"

#include "sp/graphics/API/Texture2D.h"

#include "sp/system/Memory.h"
#include "sp/graphics/shaders/ShaderResource.h"
#include "sp/graphics/shaders/ShaderManager.h"

#include <sstream>

namespace sp {
	namespace graphics {

		using namespace API;

		Material::Material(String name, API::Shader* shader)
			: m_Name(name), m_Shader(shader)
		{
			AllocateStorage();
			m_Resources = &shader->GetResources();
		}
		
		Material::Material(String name, String shader)
			: m_Name(name)
		{
			m_Shader = ShaderManager::Get(shader);

			AllocateStorage();
			m_Resources = &m_Shader->GetResources();
		}

		Material::~Material()
		{
		}

		void Material::AllocateStorage()
		{
			for (ShaderType type : m_Shader->GetShaderTypes()) {
				const ShaderUniformBufferDeclaration* buffer = m_Shader->GetUserUniformBuffer(type);
				if (buffer)
				{
					m_UserUniformBuffersSize[type] = buffer->GetSize();
					m_UserUniformBuffers[type] = spnew byte[m_UserUniformBuffersSize[type]];
					memset(m_UserUniformBuffers[type], 0, m_UserUniformBuffersSize[type]);
					m_UserUniforms[type] = buffer->GetUniformDeclarations();
				}
			}
		}

		void Material::Bind()
		{
			m_Shader->Bind();

			for (auto user : m_UserUniformBuffers) {
				if (user.second)
					m_Shader->SetUserUniformBuffer(user.first, user.second, m_UserUniformBuffersSize[user.first]);
			}

			for (uint i = 0; i < m_Textures.size(); i++)
			{
				Texture* texture = m_Textures[i];
				if (texture) {
					texture->Bind(i);
				}
			}
		}

		void Material::Unbind()
		{
			for (uint i = 0; i < m_Textures.size(); i++)
			{
				Texture* texture = m_Textures[i];
				if (texture)
					texture->Unbind(i);
			}
		}

		void Material::SetUniformData(const String& uniform, byte* data)
		{
			byte* buffer;
			ShaderUniformDeclaration* declaration = FindUniformDeclaration(uniform, &buffer);
			memcpy(buffer + declaration->GetOffset(), data, declaration->GetSize());
		}

		void Material::SetUniformData(const String& uniform, byte* data, uint size)
		{
			byte* buffer;
			ShaderUniformDeclaration* declaration = FindUniformDeclaration(uniform, &buffer);
			memcpy(buffer + declaration->GetOffset(), data, size);
		}

		void Material::SetTexture(const String& name, Texture* texture)
		{
			ShaderResourceDeclaration* declaration = FindResourceDeclaration(name);
			if (declaration == nullptr) return;
			uint slot = declaration->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize(slot + 1);
			m_Textures[slot] = texture;
		}

		ShaderUniformDeclaration* Material::FindUniformDeclaration(const String& name, byte** outBuffer)
		{
			for (auto user : m_UserUniformBuffers) {
				if (user.second) {
					for (ShaderUniformDeclaration* uniform : m_UserUniforms[user.first])
					{
						if (uniform->GetName() == name)
						{
							*outBuffer = user.second;
							return uniform;
						}
					}
				}
			}
			return nullptr;
		}

		ShaderResourceDeclaration* Material::FindResourceDeclaration(const String& name)
		{
			for (ShaderResourceDeclaration* resource : *m_Resources)
			{
				if (resource->GetName() == name)
					return resource;
			}
			return nullptr;
		}


		MaterialInstance::MaterialInstance(Material* material)
			: m_Material(material)
		{
			AllocateStorage();
			for (auto user : m_Material->m_UserUniformBuffers) {
				memcpy(m_UserUniformBuffers[user.first], user.second, m_UserUniformBuffersSize[user.first]);
			}

			m_Resources = &m_Material->GetShader()->GetResources();
			m_RenderFlags = material->m_RenderFlags;
		}

		MaterialInstance::MaterialInstance(String name)
			: MaterialInstance(MaterialManager::Get(name))
		{

		}

		void MaterialInstance::AllocateStorage()
		{
			for (auto user : m_UserUniformBuffers) {
				const ShaderUniformBufferDeclaration* buffer = m_Material->m_Shader->GetUserUniformBuffer(user.first);
				if (buffer)
				{
					m_UserUniformBuffersSize[user.first] = buffer->GetSize();
					m_UserUniformBuffers[user.first] = spnew byte[m_UserUniformBuffersSize[user.first]];
					m_UserUniforms[user.first] = buffer->GetUniformDeclarations();
				}
			}
		}

		void MaterialInstance::Bind()
		{
			m_Material->Bind();

			for (auto user : m_UserUniformBuffers) {
				if (user.second)
					m_Material->m_Shader->SetUserUniformBuffer(user.first, user.second, m_UserUniformBuffersSize[user.first]);
			}

			for (uint i = 0; i < m_Textures.size(); i++)
			{
				Texture* texture = m_Textures[i];
				if (texture)
					texture->Bind(i);
			}
		}

		void MaterialInstance::Unbind()
		{
			m_Material->Unbind();

			for (uint i = 0; i < m_Textures.size(); i++)
			{
				Texture* texture = m_Textures[i];
				if (texture)
					texture->Unbind(i);
			}
		}

		void MaterialInstance::SetUniformData(const String& uniform, byte* data)
		{
			byte* buffer;
			ShaderUniformDeclaration* declaration = FindUniformDeclaration(uniform, &buffer);
			if (declaration == nullptr) return;
			SP_ASSERT(buffer);
			memcpy(buffer + declaration->GetOffset(), data, declaration->GetSize());
		}

		void MaterialInstance::SetTexture(const String& name, Texture* texture)
		{
			ShaderResourceDeclaration* declaration = FindResourceDeclaration(name);
			uint slot = declaration->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize(slot + 1);
			m_Textures[slot] = texture;
		}

		ShaderUniformDeclaration* MaterialInstance::FindUniformDeclaration(const String& name, byte** outBuffer)
		{
			for (auto user : m_UserUniformBuffers) {
				if (user.second) {
					for (ShaderUniformDeclaration* uniform : m_UserUniforms[user.first])
					{
						if (uniform->GetName() == name)
						{
							*outBuffer = user.second;
							return uniform;
						}
					}
				}
			}
			return nullptr;
		}

		ShaderResourceDeclaration* MaterialInstance::FindResourceDeclaration(const String& name)
		{
			for (ShaderResourceDeclaration* resource : *m_Resources)
			{
				if (resource->GetName() == name)
					return resource;
			}
			return nullptr;
		}
	}
}