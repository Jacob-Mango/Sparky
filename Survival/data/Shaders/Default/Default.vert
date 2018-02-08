#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in vec4 jointIndices;

#define MAX_JOINTS 64

uniform mat4 sys_ProjectionMatrix;
uniform mat4 sys_ViewMatrix;
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_JointMatrix[MAX_JOINTS];

out DATA
{
	vec4 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
} vs_out;

void main()
{
	mat4 boneTransform = mat4(0.0);

	boneTransform += sys_JointMatrix[int(jointIndices.x)] * jointWeights.x;
	boneTransform += sys_JointMatrix[int(jointIndices.y)] * jointWeights.y;
	boneTransform += sys_JointMatrix[int(jointIndices.z)] * jointWeights.z;
	boneTransform += sys_JointMatrix[int(jointIndices.w)] * jointWeights.w;

	mat4 model = sys_ModelMatrix;
	mat4 modelAndBone = model;// * boneTransform;
	vec4 pos = modelAndBone * vec4(position.xyz, 1.0);

	gl_Position = sys_ProjectionMatrix * sys_ViewMatrix * pos;

	vs_out.position = pos;
	vs_out.normal = normalize(vec3(modelAndBone * vec4(normal, 0)));
	vs_out.binormal = normalize(vec3(model * vec4(binormal, 0)));
	vs_out.tangent = normalize(vec3(model * vec4(tangent, 0)));
	vs_out.uv = uv;
}