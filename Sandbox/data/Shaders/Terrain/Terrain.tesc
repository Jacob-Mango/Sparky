#version 400 core

#define ID gl_InvocationID

layout(vertices = 3) out;

in DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tridistance;
	vec3 patchdistance;
} tc_in[];

out DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tridistance;
	vec3 patchdistance;
} tc_out[];

uniform float TessLevelInner = 3;
uniform float TessLevelOuter = 2;

void main()
{
    tc_out[ID].position = tc_in[ID].position;
    if (ID == 0) {
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    }
}
