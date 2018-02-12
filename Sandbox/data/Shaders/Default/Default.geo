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
} gs_in[];

out FSDATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} gs_out;

void main()
{
	int i;
	for (i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		gs_out.position = gs_in[i].position;
		gs_out.normal = gs_in[i].normal;
		gs_out.uv = gs_in[i].uv;
		gs_out.binormal = gs_in[i].binormal;
		gs_out.tangent = gs_in[i].tangent;
		EmitVertex();
	}
	EndPrimitive();
}