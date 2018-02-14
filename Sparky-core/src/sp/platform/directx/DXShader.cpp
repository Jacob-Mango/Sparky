#include "sp/sp.h"
#include "DXShader.h"

#include "sp/system/Memory.h"
#include "sp/utils/Log.h"
#include "sp/utils/FileUtils.h"

#include "DXContext.h"

namespace sp { namespace graphics { namespace API {

	const D3DShader* D3DShader::s_CurrentlyBound = nullptr;

	bool D3DShader::TryCompile(const String& source, String& error)
	{
		D3DShaderErrorInfo info;

		if ( !Compile(source, "vs_4_0", "VSMain", info) )
		{
			error += info.message;
			return false;
		}

		if ( !Compile(source, "ps_4_0", "PSMain", info) )
		{
			error += info.message;
			return false;
		}
		return true;
	}

	bool D3DShader::TryCompileFromFile(const String& filepath, String& error)
	{
		String source = VFS::Get()->ReadTextFile(filepath);
		return TryCompile(source, error);
	}

	D3DShader::D3DShader(const String& name, const String& source)
		: m_Name(name)
	{
		Load(source);
		Parse(source);
		CreateBuffers();
	}

	D3DShader::~D3DShader()
	{
		m_Data.vertexShader->Release();
		m_Data.pixelShader->Release();
	}

	void D3DShader::Load(const String& source)
	{
		D3DShaderErrorInfo info;
		m_Data.vs = Compile(source, "vs_4_0", "VSMain", info);
		SP_ASSERT(m_Data.vs, info.profile, info.message);
		m_Data.ps = Compile(source, "ps_4_0", "PSMain", info);
		SP_ASSERT(m_Data.ps, info.profile, info.message);
		D3DContext::GetDevice()->CreateVertexShader(m_Data.vs->GetBufferPointer(), m_Data.vs->GetBufferSize(), NULL, &m_Data.vertexShader);
		D3DContext::GetDevice()->CreatePixelShader(m_Data.ps->GetBufferPointer(), m_Data.ps->GetBufferSize(), NULL, &m_Data.pixelShader);
	}

	ID3DBlob* D3DShader::Compile(const String& source, const String& profile, const String& main, D3DShaderErrorInfo& info)
	{
		ID3DBlob* shaderBlob;
		ID3DBlob* errorBlob;
		HRESULT status = D3DCompile(source.c_str(), source.size(), NULL, NULL, NULL, main.c_str(), profile.c_str(), D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
		if (status != S_OK)
			info.message = "Unable to compile shader from source\n";
		if (errorBlob)
		{
			info.profile += profile + "\n";
			if (errorBlob->GetBufferSize())
				std::cout << "Shader Compile Errors" << std::endl << (const char*)errorBlob->GetBufferPointer() << std::endl;
			errorBlob->Release();
		}
		if (status == S_OK)
			return shaderBlob;
		return NULL;
	}

	String D3DShader::RemoveComments(const String& source)
	{
		const char* str = source.c_str();

		String result = source;
		int32 startPos;
		while ((startPos = FindStringPosition(result, "/*")) != -1)
		{
			int32 endPos = FindStringPosition(result, "*/");
			SP_ASSERT(endPos != -1);
			result = RemoveStringRange(result, startPos, endPos - startPos + 2);
		}

		while ((startPos = FindStringPosition(result, "//")) != -1)
		{
			int32 endPos = FindStringPosition(result, "\n", startPos);
			SP_ASSERT(endPos != -1);
			result = RemoveStringRange(result, startPos, endPos - startPos + 1);
		}

		return result;
	}

	void D3DShader::Parse(const String& source)
	{
		const char* token;
		const char* str;

		String src = RemoveComments(source);

		str = src.c_str();
		while (token = FindToken(str, "struct"))
			ParseStruct(GetBlock(token, &str));

		str = src.c_str();
		while (token = FindToken(str, "cbuffer"))
			ParseCBuffer(GetBlock(token, &str));

		str = src.c_str();
		while (token = FindToken(str, "Texture2D"))
			ParseTexture(GetStatement(token, &str));

		str = src.c_str();
		while (token = FindToken(str, "TextureCube"))
			ParseTexture(GetStatement(token, &str));

		str = src.c_str();
		while (token = FindToken(str, "SamplerState"))
			ParseSamplerState(GetStatement(token, &str));
	}

	void D3DShader::ParseStruct(const String& block)
	{
		std::vector<String> tokens = Tokenize(block);
		uint index = 0;
		index++; // struct
		String structName = tokens[index++];
		ShaderStruct* shaderStruct = spnew ShaderStruct(structName);
		index++; // {
		while (index < tokens.size())
		{
			if (tokens[index] == "}")
				break;

			String type = tokens[index++];
			String name = tokens[index++];

			if (type == ":") // TODO: Temp
				continue;

			// Strip ; from name if present
			if (const char* s = strstr(name.c_str(), ";"))
				name = String(name.c_str(), s - name.c_str());

			ShaderUniformDeclaration* field = spnew D3DShaderUniformDeclaration(D3DShaderUniformDeclaration::StringToType(type), name);
			shaderStruct->AddField(field);
		}
		m_Structs.push_back(shaderStruct);
	}

	void D3DShader::ParseCBuffer(const String& block)
	{
		std::vector<String> tokens = Tokenize(block);

		uint index = 1;
		String bufferName = tokens[index++];
		uint reg = 0;
		if (tokens[index++] == ":") // Register specified
		{
			String cbRegister = tokens[index++];
			reg = NextInt(cbRegister);
		}

		D3DShaderUniformBufferDeclaration* buffer = nullptr;

		ShaderType shaderType;
		if (StartsWith(bufferName, "VS"))
			shaderType = ShaderType::VERTEX;
		else if (StartsWith(bufferName, "PS"))
			shaderType = ShaderType::FRAGMENT;
		else
			SP_ASSERT(false);

		index++; // {
		while (index < tokens.size())
		{
			if (tokens[index] == "}")
				break;

			String type = tokens[index++];
			String name = tokens[index++];

			// Strip ; from name if present
			if (const char* s = strstr(name.c_str(), ";"))
				name = String(name.c_str(), s - name.c_str());

			// TODO: Debug mode validation: make sure every single uniform in a system uniform buffer starts with sys_!

			if (buffer == nullptr)
			{
				buffer = new D3DShaderUniformBufferDeclaration(bufferName, reg, shaderType);
				if (StartsWith(name, "sys_"))
				{
					m_UniformBuffers[shaderType].push_back(buffer);
				}
				else
				{
					m_UserUniformBuffers[shaderType] = buffer;
				}
			}
			D3DShaderUniformDeclaration::Type t = D3DShaderUniformDeclaration::StringToType(type);
			D3DShaderUniformDeclaration* declaration = nullptr;
			if (t == D3DShaderUniformDeclaration::Type::NONE)
			{
				// Find struct
				ShaderStruct* s = FindStruct(type);
				SP_ASSERT(s);
				declaration = new D3DShaderUniformDeclaration(s, name);
			}
			else
			{
				declaration = new D3DShaderUniformDeclaration(t, name);
			}
			buffer->PushUniform(declaration);
		}
		buffer->Align();
	}

	void D3DShader::ParseTexture(const String& statement)
	{
		std::vector<String> tokens = Tokenize(statement);
		// Support Texture2D and TextureCube for now
		uint index = 0;

		uint reg = 0;
		String type = tokens[index++];
		String name = tokens[index++];
		if (tokens[index++] == ":") // Register specified
		{
			String texRegister = tokens[index++];
			reg = NextInt(texRegister);
		}

		D3DShaderResourceDeclaration* declaration = spnew D3DShaderResourceDeclaration(D3DShaderResourceDeclaration::StringToType(type), name);
		declaration->m_Register = reg;
		m_Resources.push_back(declaration);
	}

	void D3DShader::ParseSamplerState(const String& statement)
	{
		std::vector<String> tokens = Tokenize(statement);
		SP_ASSERT(tokens.front() == "SamplerState");

		uint reg = 0;
		uint index = 1;
		String name = tokens[index++];
		if (tokens[index++] == ":") // Register specified
		{
			String sampRegister = tokens[index++];
			reg = NextInt(sampRegister);
		}
	}

	void D3DShader::CreateBuffers()
	{
		for (auto shader : m_UniformBuffers) {
			// Note: We only support a single uniform buffer per shader
			m_ConstantBuffersCount[shader.first] = m_UniformBuffers.size() + (m_UserUniformBuffers[shader.first] ? 1 : 0);
			m_ConstantBuffers[shader.first] = new ID3D11Buffer*[m_ConstantBuffersCount[shader.first]];

			for (uint i = 0; i < shader.second.size(); i++)
			{
				D3DShaderUniformBufferDeclaration* decl = (D3DShaderUniformBufferDeclaration*)shader.second[i];

				D3D11_BUFFER_DESC desc;
				ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
				desc.ByteWidth = decl->GetSize();
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				D3D11_SUBRESOURCE_DATA data;
				data.pSysMem = new byte[desc.ByteWidth];
				data.SysMemPitch = 0;
				data.SysMemSlicePitch = 0;
				D3DContext::GetDevice()->CreateBuffer(&desc, &data, &m_ConstantBuffers[shader.first][decl->GetRegister()]);
			}
		}

		for (auto shader : m_UserUniformBuffers) {
			if (shader.second)
			{
				D3D11_BUFFER_DESC desc;
				ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
				desc.ByteWidth = shader.second->GetSize();
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				D3D11_SUBRESOURCE_DATA data;
				data.pSysMem = new byte[desc.ByteWidth];
				data.SysMemPitch = 0;
				data.SysMemSlicePitch = 0;
				D3DContext::GetDevice()->CreateBuffer(&desc, &data, &m_ConstantBuffers[shader.first][shader.second->GetRegister()]);
			}
		}
	}

	void D3DShader::SetSystemUniformBuffer(ShaderType type, byte* data, uint size, uint slot)
	{
		if (m_UserUniformBuffers[type])
			SP_ASSERT(slot != m_UserUniformBuffers[type]->GetRegister());

		ID3D11Buffer* cbuffer = m_ConstantBuffers[type][slot];

		D3D11_MAPPED_SUBRESOURCE msr;
		memset(&msr, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

		D3DContext::GetDeviceContext()->Map(cbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msr);
		memcpy(msr.pData, data, size);
		D3DContext::GetDeviceContext()->Unmap(cbuffer, NULL);
	}

	void D3DShader::SetUserUniformBuffer(ShaderType type, byte* data, uint size)
	{
		ID3D11Buffer* cbuffer = m_ConstantBuffers[type][m_UserUniformBuffers[type]->GetRegister()];

		D3D11_MAPPED_SUBRESOURCE msr;
		memset(&msr, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

		D3DContext::GetDeviceContext()->Map(cbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msr);
		memcpy(msr.pData, data, size);
		D3DContext::GetDeviceContext()->Unmap(cbuffer, NULL);
	}

	void D3DShader::Bind() const
	{
		s_CurrentlyBound = this;

		D3DContext::GetDeviceContext()->VSSetShader(m_Data.vertexShader, NULL, 0);
		D3DContext::GetDeviceContext()->PSSetShader(m_Data.pixelShader, NULL, 0);

		//D3DContext::GetDeviceContext()->VSSetConstantBuffers(0, m_ConstantBuffersCount[ShaderType::VERTEX], m_ConstantBuffers[ShaderType::VERTEX]);
		//D3DContext::GetDeviceContext()->PSSetConstantBuffers(0, m_ConstantBuffersCount[ShaderType::FRAGMENT], m_ConstantBuffers[ShaderType::FRAGMENT]);
	}

	void D3DShader::Unbind() const
	{

	}

	bool D3DShader::Reload()
	{
		return false;
	}

	ShaderStruct* D3DShader::FindStruct(const String& name)
	{
		for (ShaderStruct* s : m_Structs)
		{
			if (s->GetName() == name)
				return s;
		}
		return nullptr;
	}

} } }