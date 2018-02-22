#version 330 core

#define PI 3.1415926535897932384626433832795
#define GAMMA 2.2

layout(location = 0) out vec4 out_Color;

in DATA
{
	vec3 position;
	vec2 uv;
} fs_in;

struct Light
{
	vec3 color;
	vec3 position;
	vec3 vector;
};

struct Material
{
	vec4 albedo;
	float metallic;
	float roughness;
	vec3 normal;
	vec3 position;
    vec3 specular;
};

uniform Light u_LightSetup[64];

Material g_Material;

uniform sampler2D u_Position;
uniform sampler2D u_Albedo;
uniform sampler2D u_Metallic;
uniform sampler2D u_Normal;

uniform sampler2D u_PreintegratedFG;
uniform samplerCube u_EnvironmentMap;

uniform vec3 u_CameraPosition;

vec3 FinalGamma(vec3 color)
{
	return pow(color, vec3(1.0 / GAMMA));
}

float FresnelSchlick(float f0, float fd90, float view)
{
	return f0 + (fd90 - f0) * pow(max(1.0 - view, 0.1), 5.0);
}

float Disney(Light light, vec3 eye)
{
	vec3 halfVector = normalize(light.vector + eye);

	float NdotL = max(dot(g_Material.normal, light.vector), 0.0);
	float LdotH = max(dot(light.vector, halfVector), 0.0);
	float NdotV = max(dot(g_Material.normal, eye), 0.0);

	float energyBias = mix(0.0, 0.5, g_Material.roughness);
	float energyFactor = mix(1.0, 1.0 / 1.51, g_Material.roughness);
	float fd90 = energyBias + 2.0 * (LdotH * LdotH) * g_Material.roughness;
	float f0 = 1.0;

	float lightScatter = FresnelSchlick(f0, fd90, NdotL);
	float viewScatter = FresnelSchlick(f0, fd90, NdotV);

	return lightScatter * viewScatter * energyFactor;
}

vec3 GGX(Light light, vec3 eye)
{
	vec3 h = normalize(light.vector + eye);
	float NdotH = max(dot(g_Material.normal, h), 0.0);

	float rough2 = max(g_Material.roughness * g_Material.roughness, 2.0e-3); // capped so spec highlights don't disappear
	float rough4 = rough2 * rough2;

	float d = (NdotH * rough4 - NdotH) * NdotH + 1.0;
	float D = rough4 / (PI * (d * d));

	// Fresnel
	vec3 reflectivity = vec3(g_Material.specular);
	float fresnel = 1.0;
	float NdotL = clamp(dot(g_Material.normal, light.vector), 0.0, 1.0);
	float LdotH = clamp(dot(light.vector, h), 0.0, 1.0);
	float NdotV = clamp(dot(g_Material.normal, eye), 0.0, 1.0);
	vec3 F = reflectivity + (fresnel - fresnel * reflectivity) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);

	// geometric / visibility
	float k = rough2 * 0.5;
	float G_SmithL = NdotL * (1.0 - k) + k;
	float G_SmithV = NdotV * (1.0 - k) + k;
	float G = 0.25 / (G_SmithL * G_SmithV);

	return G * D * F;
}

vec3 RadianceIBLIntegration(float NdotV, float roughness, vec3 specular)
{
	vec2 preintegratedFG = texture(u_PreintegratedFG, vec2(roughness, 1.0 - NdotV)).rg;
	return specular * preintegratedFG.r + preintegratedFG.g;
}

vec3 IBL(vec3 eye)
{
	float NdotV = max(dot(g_Material.normal, eye), 0.0);

	vec3 reflectionVector = normalize(reflect(-eye, g_Material.normal));
	float smoothness = 1.0 - g_Material.roughness;
	float mipLevel = (1.0 - smoothness * smoothness) * 10.0;
	vec3 cs = textureLod(u_EnvironmentMap, reflectionVector, mipLevel).rgb;
	vec3 result = pow(cs, vec3(GAMMA)) * RadianceIBLIntegration(NdotV, g_Material.roughness, vec3(g_Material.specular));

	vec3 diffuseDominantDirection = g_Material.normal;
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 diffuseImageLighting = textureLod(u_EnvironmentMap, reflectionVector, g_Material.roughness * MAX_REFLECTION_LOD).rgb;
	diffuseImageLighting = pow(diffuseImageLighting, vec3(GAMMA));

	return result + diffuseImageLighting * g_Material.albedo.rgb;
}

float Diffuse(Light light, vec3 eye)
{
	return Disney(light, eye);
}

vec3 Specular(Light light, vec3 eye)
{
	return GGX(light, eye);
}

float GetIntensity(Light light)
{
	float dist = length(light.position - g_Material.position);
	return 1.0 / (dist + 0.01);
}

Light TestLight() {
	Light light;
	light.position = vec3(0, 20, 0);
	light.color = vec3(1.0) * 1000;
	return light;
}

void main()
{
    vec4 metallic = texture(u_Metallic, fs_in.uv);
	g_Material.albedo = texture(u_Albedo, fs_in.uv);
	g_Material.position = texture(u_Position, fs_in.uv).xyz;
	g_Material.normal = normalize(texture(u_Normal, fs_in.uv).xyz);
	g_Material.roughness = metallic.r;
    g_Material.metallic = metallic.g;

	vec3 eye = normalize(u_CameraPosition - g_Material.position);

    
    vec3 f0 = vec3(0.04);
    // g_Material.albedo = g_Material.albedo.rgb * (vec3(1.0) - f0);
    // g_Material.albedo *= 1.0 - g_Material.metallic;

    g_Material.specular = mix(f0, g_Material.albedo.rgb, g_Material.metallic);
    //float reflectance = max(max(g_Material.specular.r, g_Material.specular.g), g_Material.specular.b);
    //float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    //vec3 specularEnvironmentR0 = g_Material.specular.rgb;
    //vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

	vec4 diffuse = vec4(0.0);
	vec3 specular = vec3(0.0);

	for (int i = 0; i < 1; i++)
	{
		Light light = TestLight(); // u_LightSetup[i];

		light.color *= GetIntensity(light);
		light.vector = normalize(light.position - vec3(g_Material.position));

		float NdotL = clamp(dot(g_Material.normal, light.vector), 0.1, 1.0);
		diffuse += NdotL * Diffuse(light, eye) * vec4(light.color, 1.0);
		specular += NdotL * Specular(light, eye) * light.color;
	}

	float visibility = 1;
	vec3 diff = g_Material.albedo.rgb * diffuse.rgb * visibility;
	vec3 spec = (specular + IBL(eye)) * visibility; 
	vec3 finalColor = FinalGamma(diff + spec);

	out_Color = vec4(finalColor, g_Material.albedo.a);
};