#version 400 core

layout(triangles, equal_spacing, cw) in;

in DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tridistance;
	vec3 patchdistance;
} te_in[];

out DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tridistance;
	vec3 patchdistance;
} te_out;

uniform mat4 sys_ProjectionMatrix;
uniform mat4 sys_ViewMatrix;
uniform mat4 sys_ModelMatrix;

void main()
{
    vec3 p0 = gl_TessCoord.x * te_in[0].position.xyz;
    vec3 p1 = gl_TessCoord.y * te_in[1].position.xyz;
    vec3 p2 = gl_TessCoord.z * te_in[2].position.xyz;

    te_out.patchdistance = gl_TessCoord;
    te_out.position = vec4(normalize(p0 + p1 + p2), 1.0);
	
    gl_Position = sys_ProjectionMatrix * sys_ViewMatrix * te_out.position;
}