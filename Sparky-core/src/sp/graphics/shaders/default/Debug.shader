R"(
#shader vertex
#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

uniform mat4 sys_Projectionmatrix;
uniform mat4 sys_ViewMatrix;

out DATA
{
	vec4 color;
} vs_out;

void main()
{
	gl_Position = sys_Projectionmatrix * sys_ViewMatrix * position;
	vs_out.color = color;
};

#shader fragment
#version 330 core

layout (location = 0) out vec4 color;

in DATA
{
	vec4 color;
} fs_in;

void main()
{
	color = fs_in.color;
};
)"