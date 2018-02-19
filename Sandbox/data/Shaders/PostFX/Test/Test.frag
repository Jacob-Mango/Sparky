#version 330 core

layout(location = 0) out vec4 out_Color;

in DATA
{
	vec3 position;
	vec2 uv;
} fs_in;

uniform sampler2D u_Screen;
uniform sampler2D u_Normal;

void main()
{
	vec3 screen = texture(u_Screen, fs_in.uv).rgb;
	vec3 normal = texture(u_Normal, fs_in.uv).xyz;

    out_Color = vec4(screen.r, screen.g, screen.b, 1.0);
}