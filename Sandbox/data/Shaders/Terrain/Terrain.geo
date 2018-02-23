#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} gs_in[];

out DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} gs_out;

uniform mat4 sys_ProjectionMatrix;
uniform mat4 sys_ViewMatrix;
uniform mat4 sys_ModelMatrix;

uniform vec3 u_CameraPosition;
uniform int u_TBN_range;

vec3 tangent;

void calcTangent()
{	
	vec3 v0 = gl_in[0].gl_Position.xyz;
	vec3 v1 = gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[2].gl_Position.xyz;

	// edges of the face/triangle
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
	
	vec2 uv0 = gs_in[0].uv;
	vec2 uv1 = gs_in[1].uv;
	vec2 uv2 = gs_in[2].uv;

    vec2 deltaUV1 = uv1 - uv0;
	vec2 deltaUV2 = uv2 - uv0;
	
	float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	
	tangent = normalize((e1 * deltaUV2.y - e2 * deltaUV1.y)*r);
}

vec3 displacement[3];

void main() {

	for (int i = 0; i < 3; ++i) {
		displacement[i] = vec3(0, 0, 0);
	}

	float dist = (distance(gl_in[0].gl_Position.xyz, u_CameraPosition)
			    + distance(gl_in[1].gl_Position.xyz, u_CameraPosition) 
				+ distance(gl_in[2].gl_Position.xyz, u_CameraPosition)) / 3;
	
	if (dist < u_TBN_range) {
		calcTangent();
		
		for(int k = 0; k < gl_in.length(); k++) {
			displacement[k] = vec3(0, 1, 0);
			
			float height = gl_in[k].gl_Position.y;
			
			float scale = 0;
			//for (int i = 0; i < 3; i++) {
				// scale += texture(materials[i].heightmap, gs_in[k].uv * materials[i].horizontalScaling).r * materials[i].heightScaling;
			//}
						
			float attenuation = clamp(-distance(gl_in[k].gl_Position.xyz, u_CameraPosition) / (u_TBN_range - 50) + 1, 0.0, 1.0);
			scale *= attenuation;

			displacement[k] *= scale;
		}	
	}
	
	for (int i = 0; i < gl_in.length(); ++i)
	{
		vec4 worldPos = gl_in[i].gl_Position + vec4(displacement[i], 0);
		gl_Position = sys_ProjectionMatrix * sys_ViewMatrix * worldPos;
		gs_out.uv = gs_in[i].uv;
		gs_out.position = worldPos;
		gs_out.tangent = tangent;
		EmitVertex();
	}
	
	EndPrimitive();
}