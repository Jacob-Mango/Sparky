#version 330 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gAlebdo;
layout(location = 2) out vec4 gSpecularRoughness;
layout(location = 3) out vec4 gNormal;

in FSDATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} fs_in;

struct Material
{
	vec4 albedo;
	vec3 specular;
	float roughness;
};

struct Attributes
{
	vec3 position;
	vec3 normal;
	vec3 binormal;
	vec3 tangent;
};

Attributes g_Attributes;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_SpecularMap;
uniform sampler2D u_GlossMap;
uniform sampler2D u_NormalMap;

uniform vec4 u_AlbedoColor;
uniform vec3 u_SpecularColor;
uniform float u_GlossColor;
uniform vec3 u_NormalColor;

uniform float u_UsingAlbedoMap;
uniform float u_UsingSpecularMap;
uniform float u_UsingGlossMap;
uniform float u_UsingNormalMap;

#define PI 3.1415926535897932384626433832795
#define GAMMA 1

vec4 GammaCorrectTexture(sampler2D tex, vec2 uv)
{
	vec4 samp = texture(tex, uv);
	return vec4(pow(samp.rgb, vec3(GAMMA)), samp.a);
}

vec3 GammaCorrectTextureRGB(sampler2D tex, vec2 uv)
{
	vec4 samp = texture(tex, uv);
	return vec3(pow(samp.rgb, vec3(GAMMA)));
}

vec4 GetAlbedoMap()
{
	return (1.0 - u_UsingAlbedoMap) * u_AlbedoColor + u_UsingAlbedoMap * GammaCorrectTexture(u_AlbedoMap, fs_in.uv);
}

vec3 GetSpecularMap()
{
	return (1.0 - u_UsingSpecularMap) * u_SpecularColor + u_UsingSpecularMap * GammaCorrectTextureRGB(u_SpecularMap, fs_in.uv);
}

float GetGlossMap()
{
	return (1.0 - u_UsingGlossMap) * u_GlossColor + u_UsingGlossMap * GammaCorrectTextureRGB(u_GlossMap, fs_in.uv).r;
}

float GetRoughnessMap()
{
	return 1.0 - GetGlossMap();
}

vec3 GetNormalMap()
{
	return (1.0 - u_UsingNormalMap) * u_NormalColor + u_UsingNormalMap * texture(u_NormalMap, fs_in.uv).rgb * 2.0 - 1.0;
}

vec3 GetNormal()
{
	mat3 toWorld = mat3(g_Attributes.binormal, g_Attributes.tangent, g_Attributes.normal);
	vec3 normalMap = GetNormalMap();
	normalMap = toWorld * normalMap.rgb;
	normalMap = normalize(normalMap);
	return normalMap;
}

void main()
{
	g_Attributes.position = fs_in.position.xyz;
	g_Attributes.normal = normalize(fs_in.normal);
	g_Attributes.normal = GetNormal();

	Material material;
	material.albedo = GetAlbedoMap();
	material.specular = GetSpecularMap();
	material.roughness = GetRoughnessMap();
	
	gPosition = vec4(g_Attributes.position, 1.0);
	gAlebdo = material.albedo;
	gSpecularRoughness = vec4(material.specular, material.roughness);
	gNormal = vec4(g_Attributes.normal, 1.0);
}