R"(
#shader vertex
#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec2 mask_uv;
layout (location = 3) in float tid;
layout (location = 4) in float mid;
layout (location = 5) in vec4 color;

uniform mat4 sys_ProjectionMatrix;
uniform mat4 sys_ViewMatrix;
uniform mat4 sys_ModelMatrix;

uniform mat4 sys_MaskMatrix;

out GSDATA
{
	vec4 position;
	vec2 uv;
	vec2 mask_uv;
	float tid;
	float mid;
	vec4 color;
} vs_out;

void main()
{
	gl_Position = sys_ProjectionMatrix * position;
	vs_out.position = position;
	vs_out.uv = uv;
	vs_out.tid = tid;
	vs_out.mid = mid;
	vs_out.color = color;
	vs_out.mask_uv = mask_uv;
};

#shader geometry
#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in GSDATA
{
	vec4 position;
	vec2 uv;
	vec2 mask_uv;
	float tid;
	float mid;
	vec4 color;
} gs_in[];

out DATA
{
	vec4 position;
	vec2 uv;
	vec2 mask_uv;
	float tid;
	float mid;
	vec4 color;
} gs_out;

void main()
{
	int i;
	for (i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		gs_out.position = gs_in[i].position;
		gs_out.uv = gs_in[i].uv;
		gs_out.mask_uv = gs_in[i].mask_uv;
		gs_out.tid = gs_in[i].tid;
		gs_out.mid = gs_in[i].mid;
		gs_out.color = gs_in[i].color;
		EmitVertex();
	}
	EndPrimitive();
}

#shader fragment
#version 330 core

layout (location = 0) out vec4 color;

in DATA
{
	vec4 position;
	vec2 uv;
	vec2 mask_uv;
	float tid;
	float mid;
	vec4 color;
} fs_in;

uniform sampler2D textures[32];

void main()
{
	vec4 texColor = fs_in.color;
	vec4 maskColor = vec4(1.0, 1.0, 1.0, 1.0);
	if (fs_in.tid > 0.0)
	{
		int tid = int(fs_in.tid - 0.5);
		texColor = fs_in.color * texture(textures[tid], fs_in.uv);
	}
	if (fs_in.mid > 0.0)
	{
		int mid = int(fs_in.mid - 0.5);
		maskColor = texture(textures[mid], fs_in.mask_uv);
	}
	color = texColor * maskColor; // vec4(1.0 - maskColor.x, 1.0 - maskColor.y, 1.0 - maskColor.z, 1.0);
};
)"