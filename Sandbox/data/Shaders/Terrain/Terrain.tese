#version 400 core

layout(quads, fractional_odd_spacing, cw) in;

in DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} te_in[];

out DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} te_out;

void main()
{
	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
	
	// world position
	vec4 position =
	((1 - u) * (1 - v) * gl_in[12].gl_Position +
	u * (1 - v) * gl_in[0].gl_Position +
	u * v * gl_in[3].gl_Position +
	(1 - u) * v * gl_in[15].gl_Position);
	
	vec2 uv =
	((1 - u) * (1 - v) * te_in[12].uv +
	u * (1 - v) * te_in[0].uv +
	u * v * te_in[3].uv +
	(1 - u) * v * te_in[15].uv);
	
	//float height = texture(heightmap, uv).r;
	//height *= 600;		
	//position.y = height;
	
	te_out.uv = uv;
	
	gl_Position = position;
}