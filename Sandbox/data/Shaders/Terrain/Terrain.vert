#version 330 core

layout(location = 0) in vec3 position;

out DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tridistance;
	vec3 patchdistance;
} vs_out;

void main()
{
    vs_out.position = vec4(position, 1.0);
}