#include "sp/sp.h"
#include "GLShader.h"

#include <GL/glew.h>
#include "sp/system/Memory.h"

#include "sp/system/FileSystem.h"

namespace sp {
	namespace graphics {
		namespace API {

			bool IGNORE_LINES = false;
			ShaderType type = ShaderType::UNKNOWN;

			bool GLShader::TryCompile(const String& source, String& error)
			{
				std::map<ShaderType, String>* sources = spnew std::map<ShaderType, String>();
				GLShader::PreProcess(source, sources);

				GLShaderErrorInfo info;
				if (!GLShader::Compile(sources, info))
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

				std::map<ShaderType, String>* sources = spnew std::map<ShaderType, String>();

				PreProcess(m_Source, sources);
				Parse(sources);

				for (auto source : *sources) {
					m_ShaderTypes.push_back(source.first);
				}

				GLShaderErrorInfo error;

				m_Handle = Compile(sources, error);
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

			void GLShader::PreProcess(const String& source, std::map<ShaderType, String>* sources)
			{
				type = ShaderType::UNKNOWN;
				std::vector<String> lines = GetLines(source);
				ReadShaderFile(lines, sources);
			}

			// NEED TO MAKE THIS MORE ABSTRACT AND PRETTY AND EASIER TO READ AND OTHER STUFF!!!
			void GLShader::ReadShaderFile(std::vector<String> lines, std::map<ShaderType, String>* shaders) {
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
							std::map<ShaderType, String>::iterator it = shaders->begin();
							shaders->insert(it, std::pair<ShaderType, String>(type, ""));
						}
						else if (StringContains(str, "geometry")) {
							type = ShaderType::GEOMETRY;
							std::map<ShaderType, String>::iterator it = shaders->begin();
							shaders->insert(it, std::pair<ShaderType, String>(type, ""));
						}
						else if (StringContains(str, "fragment")) {
							type = ShaderType::FRAGMENT;
							std::map<ShaderType, String>::iterator it = shaders->begin();
							shaders->insert(it, std::pair<ShaderType, String>(type, ""));
						}
						else if (StringContains(str, "tess_cont")) {
							type = ShaderType::TESSELLATION_CONTROL;
							std::map<ShaderType, String>::iterator it = shaders->begin();
							shaders->insert(it, std::pair<ShaderType, String>(type, ""));
						}
						else if (StringContains(str, "tess_eval")) {
							type = ShaderType::TESSELLATION_EVALUATION;
							std::map<ShaderType, String>::iterator it = shaders->begin();
							shaders->insert(it, std::pair<ShaderType, String>(type, ""));
						}
						else if (StringContains(str, "compute")) {
							type = ShaderType::COMPUTE;
							std::map<ShaderType, String>::iterator it = shaders->begin();
							shaders->insert(it, std::pair<ShaderType, String>(type, ""));
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
							VFS::Get()->ReadTextFile(file);
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

							if (def == "0") {
								IGNORE_LINES = true;
							}
						}
					}
					else if (type != ShaderType::UNKNOWN)
					{
						shaders->at(type).append(lines[i].c_str());
						shaders->at(type).append("\n");
					}
				}
			}

			uint GLShader::Compile(std::map<ShaderType, String>* sources, GLShaderErrorInfo& info)
			{
				GLCall(uint program = glCreateProgram());

				std::vector<GLuint> shaders;

				for (auto source : *sources) {
					shaders.push_back(CompileShader(source.first, source.second, program, info));
				}

				for (int z = 0; z < shaders.size(); z++) {
					GLCall(glAttachShader(program, shaders[z]));
				}

				GLCall(glLinkProgram(program));

				GLint result;
				GLCall(glGetProgramiv(program, GL_LINK_STATUS, &result));
				if (result == GL_FALSE)
				{
					GLint length;
					GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
					std::vector<char> error(length);
					GLCall(glGetProgramInfoLog(program, length, &length, error.data()));
					String errorMessage(error.data(), length);
					int32 lineNumber = -1;
					sscanf(error.data(), "%*s %*d:%d", &lineNumber);
					info.shader = 3;
					info.message[info.shader] += "Failed to link shader!\n";
					info.line[info.shader] = 0;
					info.message[info.shader] += errorMessage;

					SP_ERROR(info.message[info.shader]);
					return 0;
				}

				GLCall(glValidateProgram(program));


				for (int z = 0; z < shaders.size(); z++) {
					GLCall(glDetachShader(program, shaders[z]));
				}

				for (int z = 0; z < shaders.size(); z++) {
					GLCall(glDeleteShader(shaders[z]));
				}

				return program;
			}

			GLenum TypeToGL(ShaderType type) {
				switch (type) {
				case VERTEX:
					return GL_VERTEX_SHADER;
				case GEOMETRY:
					return GL_GEOMETRY_SHADER;
				case FRAGMENT:
					return GL_FRAGMENT_SHADER;
				case TESSELLATION_CONTROL:
					return GL_TESS_CONTROL_SHADER;
				case TESSELLATION_EVALUATION:
					return GL_TESS_EVALUATION_SHADER;
				case COMPUTE:
					return GL_COMPUTE_SHADER;
				}
				return -1;
			}

			String TypeToString(ShaderType type) {
				switch (type) {
				case VERTEX:
					return "GL_VERTEX_SHADER";
				case GEOMETRY:
					return "GL_GEOMETRY_SHADER";
				case FRAGMENT:
					return "GL_FRAGMENT_SHADER";
				case TESSELLATION_CONTROL:
					return "GL_TESS_CONTROL_SHADER";
				case TESSELLATION_EVALUATION:
					return "GL_TESS_EVALUATION_SHADER";
				case COMPUTE:
					return "GL_COMPUTE_SHADER";
				}
				return "N/A";
			}

			GLuint GLShader::CompileShader(ShaderType type, String source, uint program, GLShaderErrorInfo& info) {
				const char* cstr = source.c_str();

				GLCall(GLuint shader = glCreateShader(TypeToGL(type)));
				GLCall(glShaderSource(shader, 1, &cstr, NULL));
				GLCall(glCompileShader(shader));

				GLint result;
				GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
				if (result == GL_FALSE)
				{
					GLint length;
					GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
					std::vector<char> error(length);
					GLCall(glGetShaderInfoLog(shader, length, &length, error.data()));
					String errorMessage(error.data(), length);
					int32 lineNumber;
					sscanf(error.data(), "%*s %*d:%d", &lineNumber);
					info.shader = type;
					info.message[info.shader] += "Failed to compile " + TypeToString(type) + " shader!\n";

					info.line[info.shader] = lineNumber;
					info.message[info.shader] += errorMessage;
					GLCall(glDeleteShader(shader));

					SP_INFO(source);

					SP_ERROR(info.message[info.shader]);
					return -1;
				}
				return shader;
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

			void GLShader::Parse(std::map<ShaderType, String>* sources)
			{
				for (auto source : *sources) {
					m_UniformBuffers[source.first].push_back(spnew GLShaderUniformBufferDeclaration("Global", source.first));

					const char* token;
					const char* str;

					str = source.second.c_str();
					while (token = FindToken(str, "struct"))
						ParseUniformStruct(GetBlock(token, &str), source.first);

					str = source.second.c_str();
					while (token = FindToken(str, "uniform"))
						ParseUniform(GetStatement(token, &str), source.first);
				}
			}

			void GLShader::ParseUniform(const String& statement, ShaderType type)
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
						((GLShaderUniformBufferDeclaration*)m_UniformBuffers[type].front())->PushUniform(declaration);
					}
					else
					{
						if (m_UserUniformBuffers[type] == nullptr)
							m_UserUniformBuffers[type] = new GLShaderUniformBufferDeclaration("", 0);
						m_UserUniformBuffers[type]->PushUniform(declaration);
					}
				}
			}

			void GLShader::ParseUniformStruct(const String& block, ShaderType type)
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

				for (auto shader : m_UniformBuffers) {
					for (uint j = 0; j < shader.second.size(); j++)
					{
						GLShaderUniformBufferDeclaration* decl = (GLShaderUniformBufferDeclaration*)shader.second[j];
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
				}


				for (auto shader : m_UserUniformBuffers) {
					GLShaderUniformBufferDeclaration* decl = shader.second;
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

				for (auto shader : m_UniformBuffers) {
					for (uint i = 0; i < shader.second.size(); i++) {
						result = FindUniformDeclaration(name, shader.second[i]);
						if (result)
							return result;
					}
				}

				for (auto shader : m_UserUniformBuffers) {
					result = FindUniformDeclaration(name, shader.second);
					if (result)
						return result;
				}

				return result;
			}

			void GLShader::SetSystemUniformBuffer(ShaderType type, byte* data, uint size, uint slot)
			{
				Bind();
				SP_ASSERT(m_UniformBuffers.size() > slot);
				if (m_UniformBuffers[type].size() > 0) {
					ShaderUniformBufferDeclaration* declaration = m_UniformBuffers[type][slot];
					if (declaration != nullptr)
						ResolveAndSetUniforms(declaration, data, size);
				}
			}

			void GLShader::SetUserUniformBuffer(ShaderType type, byte* data, uint size)
			{
				ResolveAndSetUniforms(m_UserUniformBuffers[type], data, size);
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