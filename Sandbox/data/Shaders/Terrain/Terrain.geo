#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tridistance;
	vec3 patchdistance;
} gs_in[];

out DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 tridistance;
	vec3 patchdistance;
} gs_out;

void main()
{
	vec3 A = gs_in[2].position.xyz - gs_in[0].position.xyz;
    vec3 B = gs_in[1].position.xyz - gs_in[0].position.xyz;
    gs_out.normal = vec3(0, 1, 0) * normalize(cross(A, B));
    
    gs_out.patchdistance = gs_in[0].patchdistance;
    gs_out.tridistance = vec3(1, 0, 0);
    gs_out.position = gs_in[0].position;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gs_out.patchdistance = gs_in[1].patchdistance;
    gs_out.tridistance = vec3(0, 1, 0);
    gs_out.position = gs_in[1].position;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gs_out.patchdistance = gs_in[2].patchdistance;
    gs_out.tridistance = vec3(0, 0, 1);
    gs_out.position = gs_in[2].position;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}