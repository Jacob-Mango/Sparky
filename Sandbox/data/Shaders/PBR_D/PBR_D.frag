#version 330 core

#define PI 3.14159265359
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
    vec3 direction;
	vec3 position;
    int type;
};

struct Material
{
	vec4 albedo;
	vec3 normal;
    float metallic;
	float roughness;
};

uniform Light u_LightSetup[64];

Material g_Material;

uniform sampler2D u_Position;
uniform sampler2D u_Albedo;
uniform sampler2D u_Metallic;
uniform sampler2D u_Normal;

uniform samplerCube u_EnvironmentMap;
uniform sampler2D u_PreintegratedFG;

uniform vec3 u_CameraPosition;

struct PBRInfo
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};

vec3 Diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / PI;
}

vec3 IBL(PBRInfo pbrInputs, vec3 n, vec3 reflection)
{
    float mipCount = 9.0; 
    float lod = (pbrInputs.perceptualRoughness * mipCount);
    vec3 brdf = vec3(0); // texture(u_PreintegratedFG, vec2(pbrInputs.NdotV, 1.0 - pbrInputs.perceptualRoughness)).rgb;
    vec3 diffuseLight = vec3(0.2, 0.3, 0.8); // SRGBtoLINEAR(textureCube(u_EnvironmentMap, n)).rgb;

    vec3 specularLight = vec3(0.04); //textureLod(u_EnvironmentMap, reflection, lod).rgb;

    vec3 diffuse = diffuseLight * pbrInputs.diffuseColor;
    vec3 specular = specularLight * (pbrInputs.specularColor * brdf.x + brdf.y);

    return diffuse + specular;
}


vec3 SpecularReflection(PBRInfo pbrInputs)
{
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

float GeometricOcclusion(PBRInfo pbrInputs)
{
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

float MicrofacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (PI * f * f);
}

void main()
{
	vec3 position = texture(u_Position, fs_in.uv).xyz;

	vec4 metallic = texture(u_Metallic, fs_in.uv);
	g_Material.albedo = texture(u_Albedo, fs_in.uv);
	g_Material.normal = texture(u_Normal, fs_in.uv).xyz;
	g_Material.roughness = metallic.r;
    g_Material.metallic = metallic.g;
    
    float perceptualRoughness = g_Material.roughness;
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    vec3 f0 = vec3(0.04);
    vec3 diffuseColor = g_Material.albedo.rgb * (vec3(1.0) - f0);
    diffuseColor *= 1.0 - g_Material.metallic;

    vec3 specularColor = mix(f0, g_Material.albedo.rgb, g_Material.metallic);
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
    vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

    vec3 n = g_Material.normal;
    vec3 v = normalize(u_CameraPosition - position);
    vec3 reflection = -normalize(reflect(v, n));

    Light lights[2];
    lights[0] = Light(vec3(0.0001), vec3(0.5, 1, 0), vec3(0), 1);
    lights[1] = Light(vec3(100), vec3(0), vec3(0, 20, -80), 0);

    PBRInfo pbrInputs;

    vec3 color = vec3(0.0);
    for (int i = 0; i < 2; i++) {
        Light light = lights[i];

        if (light.type == 0) {
            light.direction = normalize(light.position - position);
        } else if (light.type == 1) {
            light.position = position;
        }

        vec3 l = normalize(light.direction);
        vec3 h = normalize(l + v);

        float NdotL = clamp(dot(n, l), 0.001, 1.0);
        float NdotV = abs(dot(n, v)) + 0.001;
        float NdotH = clamp(dot(n, h), 0.0, 1.0);
        float LdotH = clamp(dot(l, h), 0.0, 1.0);
        float VdotH = clamp(dot(v, h), 0.0, 1.0);

        pbrInputs = PBRInfo(
            NdotL,
            NdotV,
            NdotH,
            LdotH,
            VdotH,
            perceptualRoughness,
            g_Material.metallic,
            specularEnvironmentR0,
            specularEnvironmentR90,
            alphaRoughness,
            diffuseColor,
            specularColor
        ); 

        vec3 F = SpecularReflection(pbrInputs);
        float G = GeometricOcclusion(pbrInputs);
        float D = MicrofacetDistribution(pbrInputs);

        vec3 diffuseContrib = ((1.0 - F) * Diffuse(pbrInputs)) * light.color;
        vec3 specContrib = (F * G * D / (4.0 * NdotL * NdotV)) * light.color;

        float dist = length(light.position - position) + 0.001;
        float attenuation = 1.0 / dist;

        color += NdotL * (diffuseContrib + specContrib) * attenuation;
    }

    color += IBL(pbrInputs, n, reflection);

    color = vec3(color / (color + vec3(1.0)));
    color = pow(color, vec3(1.0 / GAMMA)); 

    out_Color = vec4(color, g_Material.albedo.a);
}