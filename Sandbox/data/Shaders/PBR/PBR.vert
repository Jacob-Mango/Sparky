#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelMatrix;

out DATA
{
	vec3 position;
	vec2 uv;
} vs_out;

void main()
{
	gl_Position = u_ProjectionMatrix * u_ModelMatrix * vec4(position, 1.0);
	vs_out.position = position;
	vs_out.uv = uv;
};