#version 330 core

layout(location = 0) out vec4 out_Color;

in DATA
{
	vec3 position;
	vec2 uv;
} fs_in;

uniform sampler2D u_Screen;

const float exposure = 1;

void main()
{
	vec3 screen = texture(u_Screen, fs_in.uv).rgb;

	vec3 result = vec3(1.0) - exp(-screen * exposure);

    out_Color = vec4(result.r, result.g, result.b, 1.0);
}