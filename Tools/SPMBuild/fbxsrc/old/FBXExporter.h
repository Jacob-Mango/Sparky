#pragma once
#include "Utilities.h"
#include <unordered_map>

using namespace fbxsdk;

class FBXExporter
{
public:
	FBXExporter();
	bool Initialize();
	bool LoadScene(const char* inFileName, const char* inOutputPath);
	
	void ExportFBX();

private:
	FbxManager* mFBXManager;
	FbxScene* mFBXScene;

	std::string mInputFilePath;
	std::string mOutputFilePath;

	bool mHasAnimation;
	std::unordered_map<unsigned int, CtrlPoint*> mControlPoints; 
	unsigned int mTriangleCount;

	std::vector<uint> mIndices;
	std::vector<Vertex> mVertices;
	std::unordered_map<Vertex, uint> mIndexMapping;

	Skeleton mSkeleton;
	FbxLongLong mAnimationLength;
	std::string mAnimationName;
	

private:
	void ProcessGeometry(FbxNode* inNode);

	void ProcessSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
	void ProcessControlPoints(FbxNode* inNode);
	void ProcessJointsAndAnimations(FbxNode* inNode);
	unsigned int FindJointIndexUsingName(const std::string& inJointName);

	void ProcessMesh(FbxNode* inNode);

	void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, vec2& outUV);
	void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, vec3& outNormal);
	void ReadBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, vec3& outBinormal);
	void ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, vec3& outTangent);

	void InsertVertex(const vec3& position, const vec3& normal, const vec2& uv, const vec3& binormal, const vec3& tangent, const vec4& weight, const vec4& jIndex);
};