#pragma once

#include "sp/sp.h"
#include "sp/Common.h"
#include "sp/Types.h"

#include "sp/maths/maths.h"

#include "sp/system/VFS.h"

#include "ShaderUniform.h"
#include "ShaderResource.h"

namespace sp {
	namespace graphics {

		namespace API {

#define SHADER_VERTEX_INDEX		0
#define SHADER_UV_INDEX			1
#define SHADER_MASK_UV_INDEX	2
#define SHADER_TID_INDEX		3
#define SHADER_MID_INDEX		4
#define SHADER_COLOR_INDEX		5

#define SHADER_UNIFORM_PROJECTION_MATRIX_NAME	"sys_ProjectionMatrix"
#define SHADER_UNIFORM_VIEW_MATRIX_NAME			"sys_ViewMatrix"
#define SHADER_UNIFORM_MODEL_MATRIX_NAME		"sys_ModelMatrix"

			enum ShaderType : int
			{
				UNKNOWN = -1,
				VERTEX = 0,
				GEOMETRY = 1,
				TESSELLATION_CONTROL = 2,
				TESSELLATION_EVALUATION = 3,
				FRAGMENT = 4,
				COMPUTE = 5
			};

			class SP_API Shader
			{
			public:
				static const Shader* s_CurrentlyBound;
			public:
				virtual void Bind() const = 0;
				virtual void Unbind() const = 0;
				virtual bool Reload() = 0;

				virtual void SetSystemUniformBuffer(ShaderType type, byte* data, uint size, uint slot = 0) = 0;
				virtual void SetUserUniformBuffer(ShaderType type, byte* data, uint size) = 0;

				virtual const ShaderUniformBufferList& GetSystemUniforms(ShaderType type) const = 0;
				virtual const ShaderUniformBufferDeclaration* GetUserUniformBuffer(ShaderType type) const = 0;

				virtual const std::vector<ShaderType> GetShaderTypes() const = 0;

				virtual const ShaderResourceList& GetResources() const = 0;

				virtual const String& GetName() const = 0;
				virtual const String& GetFilePath() const = 0;

				// virtual void SetData(byte* data, uint size) = 0;

				// bool HasUniform(const String& name) const = 0;


			public:
				static Shader* CreateFromFile(const String& name, const String& filepath, void* address = nullptr); // TODO: Temp, implement properly
				static Shader* CreateFromName(const String& name, void* address = nullptr);
				static Shader* CreateFromSource(const String& name, const String& source);

				static bool TryCompile(const String& source, String& error);
				static bool TryCompileFromFile(const String& filepath, String& error);
			};

		}
	}
}