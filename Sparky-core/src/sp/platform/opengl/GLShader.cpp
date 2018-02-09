#include "sp/sp.h"
#include "GLShader.h"

#include <GL/glew.h>
#include "sp/system/Memory.h"

#include "sp/system/FileSystem.h"

namespace sp {
	namespace graphics {
		namespace API {

			bool* types = spnew bool[3];
			bool IGNORE_LINES = false;
			ShaderType type = ShaderType::UNKNOWN;

			bool GLShader::TryCompile(const String& source, String& error)
			{
				String vert, geo, frag;
				String* shaders[3] = { &vert, &geo, &frag };
				GLShader::PreProcess(source, shaders);

				GLShaderErrorInfo info;
				if (!GLShader::Compile(shaders, info))
				{
					error = info.message[info.shader];
					SP_ERROR(error);
					return false;
				}
				return true;
			}

			bool GLShader::TryCompileFromFile(const String& filepath, String& error)
			{
				String source = VFS::Get()->ReadTextFile(filepath);
				return TryCompile(source, error);
			}

			GLShader::GLShader(const String& name, const String& source)
				: m_Name(name), m_Source(source)
			{
				Init();
			}

			GLShader::~GLShader()
			{
			}

			bool GLShader::Init()
			{
				SP_WARN("Compiling shader: ", m_Name);
				m_VSUserUniformBuffer = nullptr;
				m_GSUserUniformBuffer = nullptr;
				m_PSUserUniformBuffer = nullptr;

				String* shaders[3] = { &m_VertexSource, &m_GeometrySource, &m_FragmentSource };
				PreProcess(m_Source, shaders);
				Parse(m_VertexSource, m_GeometrySource, m_FragmentSource);

				_SP_WARN("Vertex:\n");
				_SP_INFO(m_VertexSource);
				_SP_WARN("Geometry:\n");
				_SP_INFO(m_GeometrySource);
				_SP_WARN("Fragment:\n");
				_SP_INFO(m_FragmentSource);

				GLShaderErrorInfo error;
				m_Handle = Compile(shaders, error);
				if (!m_Handle) {
					SP_ERROR(error.message[error.shader]);
					return false;
				}
				SP_ASSERT(m_Handle);
				ResolveUniforms();
				ValidateUniforms();
				SP_WARN("Successfully compiled shader: ", m_Name);
				return true;
			}

			bool GLShader::Reload()
			{
				return Init();
			}

			void GLShader::Shutdown()
			{
				GLCall(glDeleteProgram(m_Handle));
			}

			void GLShader::PreProcess(const String& source, String** shaders)
			{
				type = ShaderType::UNKNOWN;

				types[0] = false;
				types[1] = false;
				types[2] = false;

				std::vector<String> lines = GetLines(source);
				ReadShaderFile(lines, shaders);
			}

			void GLShader::ReadShaderFile(std::vector<String> lines, String** shaders) {
				for (uint i = 0; i < lines.size(); i++)
				{
					String str = String(lines[i]);
					str = StringReplace(str, '\t');

					if (IGNORE_LINES) {
						if (StartsWith(str, "#end")) {
							IGNORE_LINES = false;
						}
					}
					else if (StartsWith(str, "#shader"))
					{
						if (StringContains(str, "vertex")) {
							type = ShaderType::VERTEX;
							types[0] = true;
						}
						else if (StringContains(str, "geometry")) {
							type = ShaderType::GEOMETRY;
							types[1] = true;
						}
						else if (StringContains(str, "fragment")) {
							type = ShaderType::FRAGMENT;
							types[2] = true;
						}
						else if (StringContains(str, "end")) {
							type = ShaderType::UNKNOWN;
						}
					}
					else if (StartsWith(str, "#include"))
					{
						String rem = "#include ";
						String file = String(str);
						if (strstr(file.c_str(), rem.c_str())) {
							std::string::size_type j = file.find(rem);
							if (j != std::string::npos)
								file.erase(j, rem.length());
							file = StringReplace(file, '\"');
							SP_WARN("Including file \'", file, "\' into shader.");

							ReadShaderFile(GetLines(VFS::Get()->ReadTextFile(file)), shaders);
						}
					}
					else if (StartsWith(str, "#if"))
					{
						String rem = "#if ";
						String def = String(str);
						if (strstr(def.c_str(), rem.c_str())) {
							std::string::size_type j = def.find(rem);
							if (j != std::string::npos)
								def.erase(j, rem.length());
							def = StringReplace(def, '\"');

							if (def != graphics::API::Context::GetRendererString()) {
								IGNORE_LINES = true;
							}

							if (def == "0") {
								IGNORE_LINES = true;
							}
						}
					}
					else if (type != ShaderType::UNKNOWN)
					{
						shaders[(int32)type - 1]->append(lines[i].c_str());
						shaders[(int32)type - 1]->append("\n");
					}
				}
			}

			uint GLShader::Compile(String** shaders, GLShaderErrorInfo& info)
			{
				const char* vertexSource = shaders[0]->c_str();
				const char* geometrySource = shaders[1]->c_str();
				const char* fragmentSource = shaders[2]->c_str();

				GLCall(uint program = glCreateProgram());
				GLCall(GLuint vertex = glCreateShader(GL_VERTEX_SHADER));

				GLuint geometry;
				if (types[1])
					GLCall(geometry = glCreateShader(GL_GEOMETRY_SHADER));


				GLCall(GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER));

				GLCall(glShaderSource(vertex, 1, &vertexSource, NULL));
				GLCall(glCompileShader(vertex));

				GLint result;
				GLCall(glGetShaderiv(vertex, GL_COMPILE_STATUS, &result));
				if (result == GL_FALSE)
				{
					GLint length;
					GLCall(glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length));
					std::vector<char> error(length);
					GLCall(glGetShaderInfoLog(vertex, length, &length, &error[0]));
					String errorMessage(&error[0]);
					int32 lineNumber;
					sscanf(&error[0], "%*s %*d:%d", &lineNumber);
					info.shader = 0;
					info.message[info.shader] += "Failed to compile vertex shader!\n";

					uint absoluteLine = lineNumber + 1;
					info.line[info.shader] = absoluteLine;
					info.message[info.shader] += errorMessage;
					GLCall(glDeleteShader(vertex));
					return 0;
				}

				if (types[1]) {

					GLCall(glShaderSource(geometry, 1, &geometrySource, NULL));
					GLCall(glCompileShader(geometry));

					GLCall(glGetShaderiv(geometry, GL_COMPILE_STATUS, &result));
					if (result == GL_FALSE)
					{
						GLint length;
						GLCall(glGetShaderiv(geometry, GL_INFO_LOG_LENGTH, &length));
						std::vector<char> error(length);
						GLCall(glGetShaderInfoLog(geometry, length, &length, &error[0]));
						String errorMessage(&error[0], length);
						int32 lineNumber;
						sscanf(&error[0], "%*s %*d:%d", &lineNumber);
						info.shader = 1;
						info.message[info.shader] += "Failed to compile geometry shader!\n";

						uint absoluteLine = GetLines(vertexSource).size() + lineNumber + 2;
						info.line[info.shader] = absoluteLine;
						info.message[info.shader] += errorMessage;
						GLCall(glDeleteShader(geometry));
						return 0;
					}
				}

				GLCall(glShaderSource(fragment, 1, &fragmentSource, NULL));
				GLCall(glCompileShader(fragment));

				GLCall(glGetShaderiv(fragment, GL_COMPILE_STATUS, &result));
				if (result == GL_FALSE)
				{
					GLint length;
					GLCall(glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length));
					std::vector<char> error(length);
					GLCall(glGetShaderInfoLog(fragment, length, &length, &error[0]));
					String errorMessage(&error[0], length);
					int32 lineNumber;
					sscanf(&error[0], "%*s %*d:%d", &lineNumber);
					info.shader = 2;
					info.message[info.shader] += "Failed to compile fragment shader!\n";

					uint absoluteLine = GetLines(vertexSource).size() + GetLines(geometrySource).size() + lineNumber + 3;
					info.line[info.shader] = absoluteLine;
					info.message[info.shader] += errorMessage;
					GLCall(glDeleteShader(fragment));
					return 0;
				}

				GLCall(glAttachShader(program, vertex));
				if (types[1]) {
					GLCall(glAttachShader(program, geometry));
				}
				GLCall(glAttachShader(program, fragment));

				GLCall(glLinkProgram(program));

				GLCall(glGetProgramiv(program, GL_LINK_STATUS, &result));
				if (result == GL_FALSE)
				{
					GLint length;
					GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
					std::vector<char> error(length);
					GLCall(glGetProgramInfoLog(program, length, &length, &error[0]));
					String errorMessage(&error[0], length);
					int32 lineNumber;
					sscanf(&error[0], "%*s %*d:%d", &lineNumber);
					info.shader = 3;
					info.message[info.shader] += "Failed to link shader!\n";

					// String line = utils::GetLines(m_FragmentSource)[lineNumber - 1];
					// uint absoluteLine = utils::GetLines(m_VertexSource).size() + lineNumber + 2;
					// info.message += lineNumber + "(" + StringFormat::ToString(absoluteLine) + ")  " + line;
					info.line[info.shader] = 0;// lineNumber;
					info.message[info.shader] += errorMessage;
					return 0;
				}

				GLCall(glValidateProgram(program));

				GLCall(glDetachShader(program, vertex));
				if (types[1]) {
					GLCall(glDetachShader(program, geometry));
				}
				GLCall(glDetachShader(program, fragment));

				GLCall(glDeleteShader(vertex));
				if (types[1]) {
					GLCall(glDeleteShader(geometry));
				}
				GLCall(glDeleteShader(fragment));

				return program;
			}

			void GLShader::Bind() const
			{
				SP_ASSERT((glIsProgram(m_Handle) == GL_TRUE), "Program doesn't exist!");
				GLCall(glUseProgram(m_Handle));
				s_CurrentlyBound = this;
			}

			void GLShader::Unbind() const
			{
				GLCall(glUseProgram(0));
				s_CurrentlyBound = nullptr;
			}

			void GLShader::Parse(const String& vertexSource, const String& geometrySource, const String& fragmentSource)
			{
				m_VSUniformBuffers.push_back(spnew GLShaderUniformBufferDeclaration("Global", 0));
				if (types[1])
					m_GSUniformBuffers.push_back(spnew GLShaderUniformBufferDeclaration("Global", 1));
				m_PSUniformBuffers.push_back(spnew GLShaderUniformBufferDeclaration("Global", 2));

				const char* token;
				const char* vstr;
				const char* fstr;
				const char* gstr;

				// Vertex Shader
				vstr = vertexSource.c_str();
				while (token = FindToken(vstr, "struct"))
					ParseUniformStruct(GetBlock(token, &vstr), 0);

				vstr = vertexSource.c_str();
				while (token = FindToken(vstr, "uniform"))
					ParseUniform(GetStatement(token, &vstr), 0);

				// Geometry Shader
				if (types[1]) {
					gstr = geometrySource.c_str();
					while (token = FindToken(gstr, "struct"))
						ParseUniformStruct(GetBlock(token, &gstr), 1);

					gstr = geometrySource.c_str();
					while (token = FindToken(gstr, "uniform"))
						ParseUniform(GetStatement(token, &gstr), 1);
				}

				// Fragment Shader
				fstr = fragmentSource.c_str();
				while (token = FindToken(fstr, "struct"))
					ParseUniformStruct(GetBlock(token, &fstr), 2);

				fstr = fragmentSource.c_str();
				while (token = FindToken(fstr, "uniform"))
					ParseUniform(GetStatement(token, &fstr), 2);
			}

			void GLShader::ParseUniform(const String& statement, uint shaderType)
			{
				std::vector<String> tokens = Tokenize(statement);
				uint index = 0;

				index++; // "uniform"
				String typeString = tokens[index++];
				String name = tokens[index++];
				// Strip ; from name if present
				if (const char* s = strstr(name.c_str(), ";"))
					name = String(name.c_str(), s - name.c_str());

				String n(name);
				int32 count = 1;
				const char* namestr = n.c_str();
				if (const char* s = strstr(namestr, "["))
				{
					name = String(namestr, s - namestr);

					const char* end = strstr(namestr, "]");
					String c(s + 1, end - s);
					count = atoi(c.c_str());
				}

				if (IsTypeStringResource(typeString))
				{
					ShaderResourceDeclaration* declaration = new GLShaderResourceDeclaration(GLShaderResourceDeclaration::StringToType(typeString), name, count);
					m_Resources.push_back(declaration);
				}
				else
				{
					GLShaderUniformDeclaration::Type t = GLShaderUniformDeclaration::StringToType(typeString);
					GLShaderUniformDeclaration* declaration = nullptr;

					if (t == GLShaderUniformDeclaration::Type::NONE)
					{
						ShaderStruct* s = FindStruct(typeString);
						SP_ASSERT(s);
						declaration = new GLShaderUniformDeclaration(s, name, count);
					}
					else
					{
						declaration = new GLShaderUniformDeclaration(t, name, count);
					}

					if (StartsWith(name, "sys_"))
					{
						if (shaderType == 0)
							((GLShaderUniformBufferDeclaration*)m_VSUniformBuffers.front())->PushUniform(declaration);
						else if (shaderType == 1)
							((GLShaderUniformBufferDeclaration*)m_GSUniformBuffers.front())->PushUniform(declaration);
						else if (shaderType == 2)
							((GLShaderUniformBufferDeclaration*)m_PSUniformBuffers.front())->PushUniform(declaration);
					}
					else
					{
						if (shaderType == 0)
						{
							if (m_VSUserUniformBuffer == nullptr)
								m_VSUserUniformBuffer = new GLShaderUniformBufferDeclaration("", 0);
							m_VSUserUniformBuffer->PushUniform(declaration);
						}
						else if (shaderType == 1)
						{
							if (m_GSUserUniformBuffer == nullptr)
								m_GSUserUniformBuffer = new GLShaderUniformBufferDeclaration("", 1);
							m_GSUserUniformBuffer->PushUniform(declaration);
						}
						else if (shaderType == 2)
						{
							if (m_PSUserUniformBuffer == nullptr)
								m_PSUserUniformBuffer = new GLShaderUniformBufferDeclaration("", 2);
							m_PSUserUniformBuffer->PushUniform(declaration);
						}
					}
				}
			}

			void GLShader::ParseUniformStruct(const String& block, uint shaderType)
			{
				std::vector<String> tokens = Tokenize(block);

				uint index = 0;
				index++; // struct
				String name = tokens[index++];
				ShaderStruct* uniformStruct = spnew ShaderStruct(name);
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

					uint count = 1;
					const char* namestr = name.c_str();
					if (const char* s = strstr(namestr, "["))
					{
						name = String(namestr, s - namestr);

						const char* end = strstr(namestr, "]");
						String c(s + 1, end - s);
						count = atoi(c.c_str());
					}

					ShaderUniformDeclaration* field = spnew GLShaderUniformDeclaration(GLShaderUniformDeclaration::StringToType(type), name, count);
					uniformStruct->AddField(field);
				}
				m_Structs.push_back(uniformStruct);
			}

			bool GLShader::IsTypeStringResource(const String& type)
			{
				if (type == "sampler2D")		return true;
				if (type == "samplerCube")		return true;
				if (type == "sampler2DShadow")	return true;
				return false;
			}

			void GLShader::ResolveUniforms()
			{
				Bind();

				for (uint i = 0; i < m_VSUniformBuffers.size(); i++)
				{
					GLShaderUniformBufferDeclaration* decl = (GLShaderUniformBufferDeclaration*)m_VSUniformBuffers[i];
					const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
					for (uint j = 0; j < uniforms.size(); j++)
					{
						GLShaderUniformDeclaration* uniform = (GLShaderUniformDeclaration*)uniforms[j];
						if (uniform->GetType() == GLShaderUniformDeclaration::Type::STRUCT)
						{
							const ShaderStruct& s = uniform->GetShaderUniformStruct();
							const auto& fields = s.GetFields();
							for (int i = 0; i < uniform->GetCount(); i++) {
								for (uint l = 0; l < fields.size(); l++)
								{
									GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[l];
									field->m_Location = GetUniformLocation(uniform->m_Name + "[" + std::to_string(i) + "]" + "." + field->m_Name);
								}
							}
						}
						else
						{
							uniform->m_Location = GetUniformLocation(uniform->m_Name);
						}
					}
				}


				for (uint j = 0; j < m_GSUniformBuffers.size(); j++)
				{
					GLShaderUniformBufferDeclaration* decl = (GLShaderUniformBufferDeclaration*)m_GSUniformBuffers[j];
					const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
					for (uint k = 0; k < uniforms.size(); k++)
					{
						GLShaderUniformDeclaration* uniform = (GLShaderUniformDeclaration*)uniforms[k];
						if (uniform->GetType() == GLShaderUniformDeclaration::Type::STRUCT)
						{
							const ShaderStruct& s = uniform->GetShaderUniformStruct();
							const auto& fields = s.GetFields();
							if (uniform->GetCount() == 1) {
								for (uint l = 0; l < fields.size(); l++)
								{
									GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[l];
									field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
								}
							}
							else {
								for (int i = 0; i < uniform->GetCount(); i++) {
									for (uint l = 0; l < fields.size(); l++)
									{
										GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[l];
										field->m_Location = GetUniformLocation(uniform->m_Name + "[" + std::to_string(i) + "]" + "." + field->m_Name);
									}
								}
							}
						}
						else
						{
							uniform->m_Location = GetUniformLocation(uniform->m_Name);
						}
					}
				}

				for (uint j = 0; j < m_PSUniformBuffers.size(); j++)
				{
					GLShaderUniformBufferDeclaration* decl = (GLShaderUniformBufferDeclaration*)m_PSUniformBuffers[j];
					const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
					for (uint k = 0; k < uniforms.size(); k++)
					{
						GLShaderUniformDeclaration* uniform = (GLShaderUniformDeclaration*)uniforms[k];
						if (uniform->GetType() == GLShaderUniformDeclaration::Type::STRUCT)
						{
							const ShaderStruct& s = uniform->GetShaderUniformStruct();
							const auto& fields = s.GetFields();
							if (uniform->GetCount() == 1) {
								for (uint l = 0; l < fields.size(); l++)
								{
									GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[l];
									field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
								}
							}
							else {
								for (int i = 0; i < uniform->GetCount(); i++) {
									for (uint l = 0; l < fields.size(); l++)
									{
										GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[l];
										field->m_Location = GetUniformLocation(uniform->m_Name + "[" + std::to_string(i) + "]" + "." + field->m_Name);
									}
								}
							}
						}
						else
						{
							uniform->m_Location = GetUniformLocation(uniform->m_Name);
						}
					}
				}
				{
					GLShaderUniformBufferDeclaration* decl = m_VSUserUniformBuffer;
					if (decl)
					{
						const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
						for (uint j = 0; j < uniforms.size(); j++)
						{
							GLShaderUniformDeclaration* uniform = (GLShaderUniformDeclaration*)uniforms[j];
							if (uniform->GetType() == GLShaderUniformDeclaration::Type::STRUCT)
							{
								const ShaderStruct& s = uniform->GetShaderUniformStruct();
								const auto& fields = s.GetFields();
								if (uniform->GetCount() == 1) {
									for (uint k = 0; k < fields.size(); k++)
									{
										GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[k];
										field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
									}
								}
								else {
									for (int i = 0; i < uniform->GetCount(); i++) {
										for (uint k = 0; k < fields.size(); k++)
										{
											GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[k];
											field->m_Location = GetUniformLocation(uniform->m_Name + "[" + std::to_string(i) + "]" + "." + field->m_Name);
										}
									}
								}
							}
							else
							{
								uniform->m_Location = GetUniformLocation(uniform->m_Name);
							}
						}
					}
				}

				{
					GLShaderUniformBufferDeclaration* decl = m_GSUserUniformBuffer;
					if (decl)
					{
						const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
						for (uint j = 0; j < uniforms.size(); j++)
						{
							GLShaderUniformDeclaration* uniform = (GLShaderUniformDeclaration*)uniforms[j];
							if (uniform->GetType() == GLShaderUniformDeclaration::Type::STRUCT)
							{
								const ShaderStruct& s = uniform->GetShaderUniformStruct();
								const auto& fields = s.GetFields();

								if (uniform->GetCount() == 1) {
									for (uint k = 0; k < fields.size(); k++)
									{
										GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[k];
										field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
									}
								}
								else {
									for (int i = 0; i < uniform->GetCount(); i++) {
										for (uint k = 0; k < fields.size(); k++)
										{
											GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[k];
											field->m_Location = GetUniformLocation(uniform->m_Name + "[" + std::to_string(i) + "]" + "." + field->m_Name);
										}
									}
								}


							}
							else
							{
								uniform->m_Location = GetUniformLocation(uniform->m_Name);
							}
						}
					}
				}

				{
					GLShaderUniformBufferDeclaration* decl = m_PSUserUniformBuffer;
					if (decl)
					{
						const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
						for (uint j = 0; j < uniforms.size(); j++)
						{
							GLShaderUniformDeclaration* uniform = (GLShaderUniformDeclaration*)uniforms[j];
							if (uniform->GetType() == GLShaderUniformDeclaration::Type::STRUCT)
							{
								const ShaderStruct& s = uniform->GetShaderUniformStruct();
								const auto& fields = s.GetFields();

								if (uniform->GetCount() == 1) {
									for (uint k = 0; k < fields.size(); k++)
									{
										GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[k];
										field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
									}
								}
								else {
									for (int i = 0; i < uniform->GetCount(); i++) {
										for (uint k = 0; k < fields.size(); k++)
										{
											GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[k];
											field->m_Location = GetUniformLocation(uniform->m_Name + "[" + std::to_string(i) + "]" + "." + field->m_Name);
										}
									}
								}
							}
							else
							{
								uniform->m_Location = GetUniformLocation(uniform->m_Name);
							}
						}
					}
				}

				uint sampler = 0;
				for (uint i = 0; i < m_Resources.size(); i++)
				{
					GLShaderResourceDeclaration* resource = (GLShaderResourceDeclaration*)m_Resources[i];
					uint location = GetUniformLocation(resource->m_Name);
					if (resource->GetCount() == 1)
					{
						resource->m_Register = sampler;
						SetUniform1i(location, sampler++);
					}
					else if (resource->GetCount() > 1)
					{
						resource->m_Register = 0;
						uint count = resource->GetCount();
						int32* samplers = spnew int32[count];
						for (uint s = 0; s < count; s++)
							samplers[s] = s;
						SetUniform1iv(resource->GetName(), samplers, count);
						spdel[] samplers;
					}
				}
				Unbind();
			}

			void GLShader::ValidateUniforms()
			{
			}

			bool GLShader::IsSystemUniform(ShaderUniformDeclaration* uniform) const
			{
				return StartsWith(uniform->GetName(), "sys_");
			}

			GLint GLShader::GetUniformLocation(const String& name)
			{
				GLCall(GLint result = glGetUniformLocation(m_Handle, name.c_str()));
				if (result == -1)
					SP_ERROR(m_Name, ": could not find uniform ", name, " in shader!");

				return result;
			}

			void GLShader::SetUniformStruct(GLShaderUniformDeclaration* uniform, byte* data, int32 offset)
			{
				const ShaderStruct& s = uniform->GetShaderUniformStruct();

				const auto& fields = s.GetFields();

				for (uint k = 0; k < fields.size(); k++)
				{
					GLShaderUniformDeclaration* field = (GLShaderUniformDeclaration*)fields[k];
					ResolveAndSetUniformField(*field, data, offset);
					offset += field->m_Size;
				}
			}

			ShaderUniformDeclaration* GLShader::FindUniformDeclaration(const String& name, const ShaderUniformBufferDeclaration* buffer)
			{
				const ShaderUniformList& uniforms = buffer->GetUniformDeclarations();
				for (uint i = 0; i < uniforms.size(); i++)
				{
					if (uniforms[i]->GetName() == name)
						return uniforms[i];
				}
				return nullptr;
			}

			ShaderUniformDeclaration* GLShader::FindUniformDeclaration(const String& name)
			{
				ShaderUniformDeclaration* result = nullptr;
				for (uint i = 0; i < m_VSUniformBuffers.size(); i++)
				{
					result = FindUniformDeclaration(name, m_VSUniformBuffers[i]);
					if (result)
						return result;
				}

				for (uint i = 0; i < m_GSUniformBuffers.size(); i++)
				{
					result = FindUniformDeclaration(name, m_GSUniformBuffers[i]);
					if (result)
						return result;
				}

				for (uint i = 0; i < m_PSUniformBuffers.size(); i++)
				{
					result = FindUniformDeclaration(name, m_PSUniformBuffers[i]);
					if (result)
						return result;
				}

				result = FindUniformDeclaration(name, m_VSUserUniformBuffer);
				if (result)
					return result;

				result = FindUniformDeclaration(name, m_GSUserUniformBuffer);
				if (result)
					return result;

				result = FindUniformDeclaration(name, m_PSUserUniformBuffer);
				if (result)
					return result;

				return result;
			}

			void GLShader::SetVSSystemUniformBuffer(byte* data, uint size, uint slot)
			{
				Bind();
				SP_ASSERT(m_VSUniformBuffers.size() > slot);
				ShaderUniformBufferDeclaration* declaration = m_VSUniformBuffers[slot];
				ResolveAndSetUniforms(declaration, data, size);
			}

			void GLShader::SetGSSystemUniformBuffer(byte* data, uint size, uint slot)
			{
				if (size == 0) return;

				Bind();
				SP_ASSERT(m_GSUniformBuffers.size() > slot);
				ShaderUniformBufferDeclaration* declaration = m_GSUniformBuffers[slot];
				ResolveAndSetUniforms(declaration, data, size);
			}

			void GLShader::SetPSSystemUniformBuffer(byte* data, uint size, uint slot)
			{
				Bind();
				SP_ASSERT(m_PSUniformBuffers.size() > slot);
				ShaderUniformBufferDeclaration* declaration = m_PSUniformBuffers[slot];
				ResolveAndSetUniforms(declaration, data, size);
			}

			void GLShader::SetVSUserUniformBuffer(byte* data, uint size)
			{
				ResolveAndSetUniforms(m_VSUserUniformBuffer, data, size);
			}

			void GLShader::SetGSUserUniformBuffer(byte* data, uint size)
			{
				ResolveAndSetUniforms(m_GSUserUniformBuffer, data, size);
			}

			void GLShader::SetPSUserUniformBuffer(byte* data, uint size)
			{
				ResolveAndSetUniforms(m_PSUserUniformBuffer, data, size);
			}

			ShaderStruct* GLShader::FindStruct(const String& name)
			{
				for (ShaderStruct* s : m_Structs)
				{
					if (s->GetName() == name)
						return s;
				}
				return nullptr;
			}

			void GLShader::ResolveAndSetUniforms(ShaderUniformBufferDeclaration* buffer, byte* data, uint size)
			{
				const ShaderUniformList& uniforms = buffer->GetUniformDeclarations();
				for (uint i = 0; i < uniforms.size(); i++)
				{
					GLShaderUniformDeclaration* uniform = (GLShaderUniformDeclaration*)uniforms[i];
					ResolveAndSetUniform(uniform, data, size);
				}
			}

			void GLShader::ResolveAndSetUniform(GLShaderUniformDeclaration* uniform, byte* data, uint size)
			{
				if (uniform->GetLocation() == -1)
					return;

				uint offset = uniform->GetOffset();
				switch (uniform->GetType())
				{
				case GLShaderUniformDeclaration::Type::FLOAT32:
					SetUniform1f(uniform->GetLocation(), *(float*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::INT32:
					SetUniform1i(uniform->GetLocation(), *(int32*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::VEC2:
					SetUniform2f(uniform->GetLocation(), uniform->GetCount(), *(maths::vec2*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::VEC3:
					SetUniform3f(uniform->GetLocation(), uniform->GetCount(), *(maths::vec3*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::VEC4:
					SetUniform4f(uniform->GetLocation(), uniform->GetCount(), *(maths::vec4*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::MAT3:
					// TODO: SetUniformMat3(uniform->GetLocation(), *(maths::mat3*)&data[offset]);
					SP_ASSERT(false, "sp::maths::mat3 does not exist!");
					break;
				case GLShaderUniformDeclaration::Type::MAT4:
					SetUniformMat4(uniform->GetLocation(), uniform->GetCount(), *(maths::mat4*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::STRUCT:
					SetUniformStruct(uniform, data, offset);
					break;
				default:
					SP_ASSERT(false, "Unknown type!");
				}
			}

			void GLShader::SetUniform(const String& name, byte* data)
			{
				ShaderUniformDeclaration* uniform = FindUniformDeclaration(name);
				if (!uniform)
				{
					SP_ERROR("Cannot find uniform in ", m_Name, " shader with name '", name, "'");
					return;
				}
				ResolveAndSetUniform((GLShaderUniformDeclaration*)uniform, data, 0);
			}

			void GLShader::ResolveAndSetUniformField(const GLShaderUniformDeclaration& field, byte* data, int32 offset)
			{
				switch (field.GetType())
				{
				case GLShaderUniformDeclaration::Type::FLOAT32:
					SetUniform1f(field.GetLocation(), *(float*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::INT32:
					SetUniform1i(field.GetLocation(), *(int32*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::VEC2:
					SetUniform2f(field.GetLocation(), field.GetCount(), *(maths::vec2*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::VEC3:
					SetUniform3f(field.GetLocation(), field.GetCount(), *(maths::vec3*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::VEC4:
					SetUniform4f(field.GetLocation(), field.GetCount(), *(maths::vec4*)&data[offset]);
					break;
				case GLShaderUniformDeclaration::Type::MAT3:
					// TODO: SetUniformMat3(field.location, *(maths::mat3*)&data[offset]);
					SP_ASSERT(false, "sp::maths::mat3 does not exist!");
					break;
				case GLShaderUniformDeclaration::Type::MAT4:
					SetUniformMat4(field.GetLocation(), field.GetCount(), *(maths::mat4*)&data[offset]);
					break;
				default:
					SP_ASSERT(false, "Unknown type!");
				}
			}

			void GLShader::SetUniform1f(const String& name, float value)
			{
				SetUniform1f(GetUniformLocation(name), value);
			}

			void GLShader::SetUniform1fv(const String& name, float* value, int32 count)
			{
				SetUniform1fv(GetUniformLocation(name), value, count);
			}

			void GLShader::SetUniform1i(const String& name, int32 value)
			{
				SetUniform1i(GetUniformLocation(name), value);
			}

			void GLShader::SetUniform1iv(const String& name, int32* value, int32 count)
			{
				SetUniform1iv(GetUniformLocation(name), value, count);
			}

			void GLShader::SetUniform2f(const String& name, uint count, const maths::vec2& vector)
			{
				SetUniform2f(GetUniformLocation(name), count, vector);
			}

			void GLShader::SetUniform3f(const String& name, uint count, const maths::vec3& vector)
			{
				SetUniform3f(GetUniformLocation(name), count, vector);
			}

			void GLShader::SetUniform4f(const String& name, uint count, const maths::vec4& vector)
			{
				SetUniform4f(GetUniformLocation(name), count, vector);
			}

			void GLShader::SetUniformMat4(const String& name, uint count, const maths::mat4& matrix)
			{
				SetUniformMat4(GetUniformLocation(name), count, matrix);
			}

			void GLShader::SetUniform1f(uint location, float value)
			{
				GLCall(glUniform1f(location, value));
			}

			void GLShader::SetUniform1fv(uint location, float* value, int32 count)
			{
				GLCall(glUniform1fv(location, count, value));
			}

			void GLShader::SetUniform1i(uint location, int32 value)
			{
				GLCall(glUniform1i(location, value));
			}

			void GLShader::SetUniform1iv(uint location, int32* value, int32 count)
			{
				GLCall(glUniform1iv(location, count, value));
			}

			void GLShader::SetUniform2f(uint location, uint count, const maths::vec2& vector)
			{
				GLfloat data[2] = { vector.x, vector.y };
				GLCall(glUniform2fv(location, count, data));
			}

			void GLShader::SetUniform3f(uint location, uint count, const maths::vec3& vector)
			{
				GLfloat data[3] = { vector.x, vector.y, vector.z };
				GLCall(glUniform3fv(location, count, data));
			}

			void GLShader::SetUniform4f(uint location, uint count, const maths::vec4& vector)
			{
				GLfloat data[4] = { vector.x, vector.y, vector.z, vector.w };
				GLCall(glUniform4fv(location, count, data));
			}

			void GLShader::SetUniformMat4(uint location, uint count, const maths::mat4& matrix)
			{
				GLCall(glUniformMatrix4fv(location, count, GL_TRUE, matrix.elements));
			}

		}
	}
}