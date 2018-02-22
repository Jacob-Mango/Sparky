#version 330 core

layout(location = 0) out vec4 out_Screen;
layout(location = 1) out vec4 out_Normal;

in DATA
{
	vec3 position;
	vec2 uv;
} fs_in;

uniform sampler2D u_Screen;

uniform vec3 u_inverseFilterTextureSize = vec3(1, 1, 1);
uniform float u_SpanMax = 8.0;
uniform float u_ReduceMin = 1.0 / 128.0;
uniform float u_ReduceMul = 1.0 / 8.0;

vec4 FXAA(sampler2D tex, vec2 texCoord) {
	vec2 texCoordOffset = u_inverseFilterTextureSize.xy;
	
	vec3 luma = vec3(0.299, 0.587, 0.114);	
	float lumaTL = dot(luma, texture2D(tex, texCoord.xy + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
	float lumaTR = dot(luma, texture2D(tex, texCoord.xy + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
	float lumaBL = dot(luma, texture2D(tex, texCoord.xy + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
	float lumaBR = dot(luma, texture2D(tex, texCoord.xy + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
	float lumaM  = dot(luma, texture2D(tex, texCoord.xy).xyz);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
	
	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (u_ReduceMul * 0.25), u_ReduceMin);
	float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = min(vec2(u_SpanMax, u_SpanMax), 
		max(vec2(-u_SpanMax, -u_SpanMax), dir * inverseDirAdjustment)) * texCoordOffset;

	vec3 result1 = (1.0/2.0) * (
		texture2D(tex, texCoord.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
		texture2D(tex, texCoord.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

	vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
		texture2D(tex, texCoord.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
		texture2D(tex, texCoord.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);

	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		return vec4(result1, 1.0);
	else
		return vec4(result2, 1.0);
}

void main()
{
    out_Screen = FXAA(u_Screen, fs_in.uv);
	out_Screen.g = 0.1;
}