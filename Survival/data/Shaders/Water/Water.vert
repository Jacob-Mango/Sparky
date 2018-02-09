#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in vec4 jointIndices;

uniform mat4 sys_ProjectionMatrix;
uniform mat4 sys_ViewMatrix;
uniform mat4 sys_ModelMatrix;

out VSDATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} vs_out;

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
	
	vs_out.normal = normal;
	vs_out.binormal = binormal;
	vs_out.tangent = tangent;

	vs_out.normal = normalize(vec3(sys_ModelMatrix * vec4(normal, 0)));
	vs_out.binormal = normalize(vec3(sys_ModelMatrix * vec4(binormal, 0)));
	vs_out.tangent = normalize(vec3(sys_ModelMatrix * vec4(tangent, 0)));

	vs_out.uv = uv + vec2(u_Time * 0.1, 0);
}