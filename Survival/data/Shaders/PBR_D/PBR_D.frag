#version 330 core

#define PI 3.1415926535897932384626433832795
#define GAMMA 2.2

layout(location = 0) out vec4 color;

in DATA
{
	vec3 position;
	vec2 uv;
} fs_in;

struct Light
{
	vec3 color;
	vec3 position;
	float p0;
	vec3 direction;
	float p1;
	vec3 lightVector;
	float intensity;
};

struct Material
{
	vec4 albedo;
	vec3 specular;
	float roughness;
	vec3 normal;
};

struct Attributes
{
	vec3 position;
	vec3 normal;
	vec3 binormal;
	vec3 tangent;
};

uniform Light u_LightSetup[64];

Attributes g_Attributes;

uniform sampler2D u_Position;
uniform sampler2D u_Albedo;
uniform sampler2D u_SpecularRoughness;
uniform sampler2D u_Normal;
//uniform sampler2D u_Tangent;
//uniform sampler2D u_Binormal;

uniform sampler2D u_PreintegratedFG;
uniform samplerCube u_EnvironmentMap;

uniform vec3 u_CameraPosition;

float nothingness;

vec3 FinalGamma(vec3 color)
{
	return pow(color, vec3(1.0 / GAMMA));
}

float FresnelSchlick(float f0, float fd90, float view)
{
	return f0 + (fd90 - f0) * pow(max(1.0 - view, 0.1), 5.0);
}

float Disney(Light light, Material material, vec3 eye)
{
	vec3 halfVector = normalize(light.lightVector + eye);

	float NdotL = max(dot(g_Attributes.normal, light.lightVector), 0.0);
	float LdotH = max(dot(light.lightVector, halfVector), 0.0);
	float NdotV = max(dot(g_Attributes.normal, eye), 0.0);

	float energyBias = mix(0.0, 0.5, material.roughness);
	float energyFactor = mix(1.0, 1.0 / 1.51, material.roughness);
	float fd90 = energyBias + 2.0 * (LdotH * LdotH) * material.roughness;
	float f0 = 1.0;

	float lightScatter = FresnelSchlick(f0, fd90, NdotL);
	float viewScatter = FresnelSchlick(f0, fd90, NdotV);

	return lightScatter * viewScatter * energyFactor;
}

vec3 GGX(Light light, Material material, vec3 eye)
{
	vec3 h = normalize(light.lightVector + eye);
	float NdotH = max(dot(g_Attributes.normal, h), 0.0);

	float rough2 = max(material.roughness * material.roughness, 2.0e-3); // capped so spec highlights don't disappear
	float rough4 = rough2 * rough2;

	float d = (NdotH * rough4 - NdotH) * NdotH + 1.0;
	float D = rough4 / (PI * (d * d));

	// Fresnel
	vec3 reflectivity = material.specular;
	float fresnel = 1.0;
	float NdotL = clamp(dot(g_Attributes.normal, light.lightVector), 0.0, 1.0);
	float LdotH = clamp(dot(light.lightVector, h), 0.0, 1.0);
	float NdotV = clamp(dot(g_Attributes.normal, eye), 0.0, 1.0);
	vec3 F = reflectivity + (fresnel - fresnel * reflectivity) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);

	// geometric / visibility
	float k = rough2 * 0.5;
	float G_SmithL = NdotL * (1.0 - k) + k;
	float G_SmithV = NdotV * (1.0 - k) + k;
	float G = 0.25 / (G_SmithL * G_SmithV);

	return G * D * F * nothingness;
}

vec3 RadianceIBLIntegration(float NdotV, float roughness, vec3 specular)
{
	vec2 preintegratedFG = texture(u_PreintegratedFG, vec2(roughness, 1.0 - NdotV)).rg;
	return specular * preintegratedFG.r + preintegratedFG.g;
}

vec3 IBL(Material material, vec3 eye)
{
	float NdotV = max(dot(g_Attributes.normal, eye), 0.0);

	vec3 reflectionVector = normalize(reflect(-eye, g_Attributes.normal));
	float smoothness = 1.0 - material.roughness;
	float mipLevel = (1.0 - smoothness * smoothness) * 10.0;
	vec4 cs = textureLod(u_EnvironmentMap, reflectionVector, mipLevel);
	vec3 result = pow(cs.xyz, vec3(GAMMA)) * RadianceIBLIntegration(NdotV, material.roughness, material.specular);

	vec3 diffuseDominantDirection = g_Attributes.normal;
	float diffuseLowMip = 9.6;
	vec3 diffuseImageLighting = textureLod(u_EnvironmentMap, diffuseDominantDirection, diffuseLowMip).rgb;
	diffuseImageLighting = pow(diffuseImageLighting, vec3(GAMMA));

	return result + diffuseImageLighting * material.albedo.rgb;
}

float Diffuse(Light light, Material material, vec3 eye)
{
	return Disney(light, material, eye);
}

vec3 Specular(Light light, Material material, vec3 eye)
{
	return GGX(light, material, eye);
}

float Attenuate(Light light)
{
	vec3 direction = normalize(light.position - g_Attributes.position);
	return light.intensity * 1.0 / (dot(direction, direction) + 0.01);
}

void main()
{
	vec4 albedo = texture(u_Albedo, fs_in.uv);
	vec4 specRoughness = texture(u_SpecularRoughness, fs_in.uv);

	g_Attributes.position = texture(u_Position, fs_in.uv).xyz;
	g_Attributes.normal = normalize(texture(u_Normal, fs_in.uv).xyz * vec3(10, 10, 10));
	//g_Attributes.tangent = texture(u_Tangent, fs_in.uv).xyz;
	//g_Attributes.binormal = texture(u_Binormal, fs_in.uv).xyz;

	if (g_Attributes.normal == vec3(0, 0, 0) && g_Attributes.position == vec3(0, 0, 0)) {
		nothingness = 0;
	}
	else {
		nothingness = 1;
	}

	vec3 eye = normalize(u_CameraPosition - g_Attributes.position);

	Material material;
	material.albedo = albedo;
	material.specular = specRoughness.rgb;
	material.roughness = specRoughness.a;

	vec4 diffuse = vec4(0.0);
	vec3 specular = vec3(0.0);

	for (int i = 0; i < 64; i++)
	{
		Light light = u_LightSetup[i];
		light.intensity = Attenuate(light);
		light.lightVector = normalize(light.position - vec3(g_Attributes.position));

		float NdotL = clamp(dot(g_Attributes.normal, light.lightVector), 0.1, 1.0);
		diffuse += NdotL * Diffuse(light, material, eye) * vec4(light.color, 1.0) * light.intensity;
		specular += NdotL * Specular(light, material, eye) * light.color * light.intensity;
	}

	float visibility = 1;
	vec3 diff = material.albedo.rgb * diffuse.rgb * visibility;
	vec3 spec = (specular + IBL(material, eye)) * visibility;
	vec3 finalColor = FinalGamma(diff + spec);

	color = vec4(finalColor, 1.0);
};