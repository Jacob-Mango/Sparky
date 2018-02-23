#version 330 core

layout(location = 0) in vec2 position;

uniform mat4 sys_ProjectionMatrix;
uniform mat4 sys_ViewMatrix;
uniform mat4 sys_ModelMatrix;

out DataBlock
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} vs_out;

uniform vec3 u_CameraPosition;
uniform int u_LOD;
uniform vec2 u_Index;
uniform float u_Gap;
uniform vec2 u_Location;
uniform int u_LOD_Morph_Area[8];

float morphLatitude(vec2 position)
{
	vec2 frac = position - u_Location;
	
	if (u_Index == vec2(0, 0))
	{
		float morph = frac.x - frac.y;
		if (morph > 0)
			return morph;
	}
	if (u_Index == vec2(1, 0))
	{
		float morph = u_Gap - frac.x - frac.y;
		if (morph > 0)
			return morph;
	}
	if (u_Index == vec2(0, 1))
	{
		float morph = frac.x + frac.y - u_Gap;
		if (morph > 0)
			return -morph;
	}
	if (u_Index == vec2(1, 1))
	{
		float morph = frac.y - frac.x;
		if (morph > 0)
			return -morph;
	}
	return 0;
}

float morphLongitude(vec2 position)
{
	vec2 frac = position - u_Location;
	
	if (u_Index == vec2(0, 0))
	{
		float morph = frac.y - frac.x;
		if (morph > 0)
			return -morph;
	}
	if (u_Index == vec2(1, 0))
	{
		float morph = frac.y - (u_Gap - frac.x);
		if (morph > 0)
			return morph;
	}
	if (u_Index == vec2(0, 1))
	{
		float morph = u_Gap - frac.y - frac.x;
		if (morph > 0)
			return -morph;
	}
	if (u_Index == vec2(1, 1))
	{
		float morph = frac.x - frac.y;
		if (morph > 0)
			return morph;
	}
	return 0;
}

vec2 morph(vec2 localPosition, int morph_area)
{
	vec2 morphing = vec2(0, 0);
	
	vec2 fixPointLatitude = vec2(0, 0);
	vec2 fixPointLongitude = vec2(0, 0);
	float distLatitude;
	float distLongitude;
	
	if (u_Index == vec2(0, 0))
	{
		fixPointLatitude = u_Location + vec2(u_Gap, 0);
		fixPointLongitude = u_Location + vec2(0, u_Gap);
	}
	else if (u_Index == vec2(1, 0))
	{
		fixPointLatitude = u_Location;
		fixPointLongitude = u_Location + vec2(u_Gap, u_Gap);
	}
	else if (u_Index == vec2(0, 1))
	{
		fixPointLatitude = u_Location + vec2(u_Gap, u_Gap);
		fixPointLongitude = u_Location;
	}
	else if (u_Index == vec2(1, 1))
	{
		fixPointLatitude = u_Location + vec2(0, u_Gap);
		fixPointLongitude = u_Location + vec2(u_Gap, 0);
	}
	
	float planarFactor = 0;

	float scaleY = 1;

	if (u_CameraPosition.y > abs(scaleY))
		planarFactor = 1;
	else
		planarFactor = u_CameraPosition.y / abs(scaleY);
		
	distLatitude = length(u_CameraPosition - (sys_ModelMatrix * 
					vec4(fixPointLatitude.x, planarFactor, fixPointLatitude.y, 1)).xyz);
	distLongitude = length(u_CameraPosition - (sys_ModelMatrix * 
					vec4(fixPointLongitude.x, planarFactor, fixPointLongitude.y, 1)).xyz);
					
	if (distLatitude > morph_area)
		morphing.x = morphLatitude(localPosition.xy);
	if (distLongitude > morph_area)
		morphing.y = morphLongitude(localPosition.xy);
		
	return morphing;
}

void main()
{
	vec2 localPosition = (vec4(position.x, 0, position.y, 1.0)).xz;
	
	if (u_LOD > 0)
		localPosition += morph(localPosition, u_LOD_Morph_Area[u_LOD - 1]);

	vec4 pos = sys_ModelMatrix * vec4(localPosition.x, 0, localPosition.y, 1.0);

	gl_Position = sys_ProjectionMatrix * sys_ViewMatrix * pos;

	vs_out.position = pos;
	vs_out.uv = localPosition;
}