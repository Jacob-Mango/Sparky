uniform mat4 sys_ProjectionMatrix;
uniform mat4 sys_ViewMatrix;
uniform mat4 sys_ModelMatrix;
uniform vec3 sys_CameraPosition;

out VSDATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
	vec3 color;
	vec4 shadowCoord;
	vec3 cameraPos;
} vs_out;