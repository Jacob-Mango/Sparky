#version 400 core

layout(vertices = 16) out;

in DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} tc_in[];

out DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} tc_out[];

const int AB = 2;
const int BC = 3;
const int CD = 0;
const int DA = 1;

uniform int u_TessellationFactor;
uniform float u_TessellationSlope;
uniform float u_TessellationShift;
uniform vec3 u_CameraPosition;

float lodFactor(float dist) {
	float tessellationLevel = max(0.0, u_TessellationFactor / pow(dist, u_TessellationSlope) + u_TessellationShift);
	return tessellationLevel;
}

void main()
{
	if (gl_InvocationID == 0){
	
		vec3 abMid = vec3(gl_in[0].gl_Position + gl_in[3].gl_Position) / 2.0;	
		vec3 bcMid = vec3(gl_in[3].gl_Position + gl_in[15].gl_Position) / 2.0;
		vec3 cdMid = vec3(gl_in[15].gl_Position + gl_in[12].gl_Position) / 2.0;
		vec3 daMid = vec3(gl_in[12].gl_Position + gl_in[0].gl_Position) / 2.0;		
		
		float distanceAB = distance(abMid, u_CameraPosition);
		float distanceBC = distance(bcMid, u_CameraPosition);
		float distanceCD = distance(cdMid, u_CameraPosition);
		float distanceDA = distance(daMid, u_CameraPosition);
		
		gl_TessLevelOuter[AB] = mix(1, gl_MaxTessGenLevel, lodFactor(distanceAB));
		gl_TessLevelOuter[BC] = mix(1, gl_MaxTessGenLevel, lodFactor(distanceBC));
		gl_TessLevelOuter[CD] = mix(1, gl_MaxTessGenLevel, lodFactor(distanceCD));
		gl_TessLevelOuter[DA] = mix(1, gl_MaxTessGenLevel, lodFactor(distanceDA));
		
		gl_TessLevelInner[0] = (gl_TessLevelOuter[BC] + gl_TessLevelOuter[DA]) / 4;
		gl_TessLevelInner[1] = (gl_TessLevelOuter[AB] + gl_TessLevelOuter[CD]) / 4;
	}
	
	tc_out[gl_InvocationID].uv = tc_in[gl_InvocationID].uv;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}