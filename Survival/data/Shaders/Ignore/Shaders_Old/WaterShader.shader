#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;

#include "/shaders/PBRShader/PBRData.shader"

uniform mat4 u_DepthBiasMVP;

uniform float u_Time;

const float PI = 3.1415926535897932384626433832795;
const float spread = 80;
const float amplitude = 0.1;

float generateHeight() {
	float time2 = u_Time;
	float component1 = sin((2.0 * PI * time2 + ((position.x + position.z) * 16.0)) / spread) * amplitude;
	float component2 = sin((2.0 * PI * time2 + (position.x * 8.0)) / spread) * amplitude;
	float component3 = cos((2.5 * PI * time2 * 10 + (position.x * 32.0)) / (spread)) * amplitude * 5;
	return (component1 + component3) + component2;
}

void main()
{
	vec4 pos = sys_ModelMatrix * vec4(position.x, generateHeight(), position.z, 1.0);
	vs_out.position = pos;
	gl_Position = sys_ProjectionMatrix * sys_ViewMatrix * pos;
	
	//vs_out.normal = normal;
	//vs_out.binormal = binormal;
	//vs_out.tangent = tangent;

	vs_out.normal = normalize(vec3(sys_ModelMatrix * vec4(normal, 0)));
	vs_out.binormal = normalize(vec3(sys_ModelMatrix * vec4(binormal, 0)));
	vs_out.tangent = normalize(vec3(sys_ModelMatrix * vec4(tangent, 0)));

	vs_out.uv = uv + vec2(u_Time * 0.1, 0);
	vs_out.color = vec3(1, 1, 1);
	vs_out.cameraPos = sys_CameraPosition;

	vs_out.shadowCoord = u_DepthBiasMVP * pos;
}

#shader geometry
#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VSDATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
	vec3 color;
	vec4 shadowCoord;
	vec3 cameraPos;
} gs_in[];

out DATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
	vec3 color;
	vec4 shadowCoord;
	vec3 cameraPos;
} gs_out;

void main()
{
	int i;

	vec3 tangent = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 binormal = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 normal = normalize(cross(tangent, binormal));

	for (i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;

		gs_out.position = gs_in[i].position;
		gs_out.normal = normalize(normal + gs_in[i].normal);
		gs_out.uv = gs_in[i].uv;
		gs_out.binormal = normalize(binormal + gs_in[i].binormal);
		gs_out.tangent = normalize(tangent + gs_in[i].tangent);
		gs_out.color = gs_in[i].color;
		gs_out.shadowCoord = gs_in[i].shadowCoord;
		gs_out.cameraPos = gs_in[i].cameraPos;
		EmitVertex();
	}
	EndPrimitive();
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

#include "/shaders/PBRShader/PBRFragment.shader"

uniform Light sys_Light;

void main()
{
	g_Attributes.position = fs_in.position.xyz;
	// Normalize inputs
	g_Attributes.normal = normalize(fs_in.normal);
	g_Attributes.tangent = normalize(fs_in.tangent);
	g_Attributes.binormal = normalize(fs_in.binormal);

	if (u_UsingNormalMap > 0.0)
		g_Attributes.normal = NormalMap();

	vec3 eye = normalize(fs_in.cameraPos - g_Attributes.position);

	Light light = sys_Light;
	light.intensity = Attenuate(light);
	light.lightVector = normalize(light.position - vec3(fs_in.position));

	Material material;
	material.albedo = GetAlbedo();
	material.specular = GetSpecular();
	material.roughness = GetRoughness();

	vec4 diffuse = vec4(0.0);
	vec3 specular = vec3(0.0);

	for (int i = 0; i < 1; i++)
	{
		float NdotL = clamp(dot(g_Attributes.normal, light.lightVector), 0.1, 1.0);
		diffuse += NdotL * Diffuse(light, material, eye) * light.color * light.intensity;
		specular += NdotL * Specular(light, material, eye) * light.color.xyz * light.intensity;
		light.intensity /= 2.0;
		light.lightVector = -light.lightVector;
	}

	// Shadows
	float bias = 0.005;
	float visibility = 1.0;
	for (int i = 0; i < 1; i++)
	{
		int index = int(16.0 * random(floor(fs_in.position.xyz * 1000.0), i)) % 16;
		visibility -= (1.0 / 4.0) * (1.0 - texture(u_ShadowMap, vec3(fs_in.shadowCoord.xy + poissonDisk[index] / 700.0, (fs_in.shadowCoord.z - bias) / fs_in.shadowCoord.w)));
	}

	vec3 finalColor = material.albedo.rgb * diffuse.rgb * visibility + (specular + IBL(light, material, eye)) * visibility;
	finalColor = FinalGamma(finalColor);

	color = vec4(finalColor, material.albedo.a);// *vec4(fs_in.color, 1.0);
}