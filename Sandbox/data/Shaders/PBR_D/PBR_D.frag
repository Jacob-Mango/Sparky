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
	float intensity;
};

struct Material
{
	vec4 albedo;
	float specular;
    float metallic;
	float roughness;
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
Material g_Material;

uniform sampler2D u_Position;
uniform sampler2D u_Albedo;
uniform sampler2D u_SpecularRoughness;
uniform sampler2D u_Normal;

uniform samplerCube u_EnvironmentMap;
uniform sampler2D u_PreintegratedFG;

uniform vec3 u_CameraPosition;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

Light TestLight() {
	Light light;	
	light.position = vec3(0, 100, -75);
	light.intensity = 300;
	light.color = vec3(1.0);
	return light;
}

vec3 Radiance(Light light, vec3 N, vec3 V, vec3 F0) 
{
    vec3 L = normalize(light.position - g_Attributes.position);
    vec3 H = normalize(V + L);
    float dist = length(light.position - g_Attributes.position);
    float attenuation = 1.0 / (dist / light.intensity);
    vec3 radiance = light.color * attenuation;

    float NDF = DistributionGGX(N, H, g_Material.roughness);   
    float G = GeometrySmith(N, V, L, g_Material.roughness);    
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);        
        
    vec3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.001;
    vec3 specular = nominator / denominator;
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - vec3(g_Material.metallic);

    float NdotL = max(dot(N, L), 0.0);        
    return ((kD * g_Material.albedo.rgb / PI + specular) * radiance * NdotL);
}

void main()
{
	g_Attributes.position = texture(u_Position, fs_in.uv).xyz;
	g_Attributes.normal = texture(u_Normal, fs_in.uv).xyz;

	vec4 specRoughness = texture(u_SpecularRoughness, fs_in.uv);
	g_Material.albedo = texture(u_Albedo, fs_in.uv);
	g_Material.specular = specRoughness.r;
	g_Material.roughness = specRoughness.g;
    g_Material.metallic = specRoughness.b;

    vec3 N = normalize(g_Attributes.normal);
    vec3 V = normalize(u_CameraPosition - g_Attributes.position);
    vec3 R = reflect(-V, N); 

    vec3 F0 = vec3(0.04); 
    // F0 = mix(F0, g_Material.albedo.rgb, vec3(g_Material.metallic));

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 1; ++i) 
    {
        Light light = TestLight();
        Lo += Radiance(light, N, V, F0); 
    }
    
    vec3 color = Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / GAMMA)); 

    out_Color = vec4(color, g_Material.albedo.a);
}