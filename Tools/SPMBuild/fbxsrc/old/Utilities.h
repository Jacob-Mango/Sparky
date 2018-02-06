#pragma once

#include <fbxsdk.h>
#include <iostream>
#include <string>
#include <unordered_map>

typedef unsigned char byte;
typedef unsigned int uint;
typedef std::string String;

#define BIT(x) (1 << x)


struct vec3
{
	float x, y, z;

	vec3()
		: x(0.0f), y(0.0f), z(0.0f)
	{}
	vec3(float x, float y, float z)
		: x(x), y(y), z(z)
	{}

	bool operator==(const vec3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool operator!=(const vec3& other) const
	{
		return !(*this == other);
	}

	uint GetHash() const
	{
		return (*(uint*)&x) ^ ((*(uint*)&y) << 14) ^ ((*(uint*)&z) << 23);
	}

};

struct vec2
{
	float x, y;

	vec2()
		: x(0.0f), y(0.0f)
	{}
	vec2(float x, float y)
		: x(x), y(y)
	{}

	bool operator==(const vec2& other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const vec2& other) const
	{
		return !(*this == other);
	}

	uint GetHash() const
	{
		return (*(uint*)&x) ^ ((*(uint*)&y) << 14);
	}

};

struct vec4
{
	float x, y, z, w;

	vec4()
		: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{}
	vec4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{}

	vec4(vec3 r, float w)
		: x(r.x), y(r.y), z(r.z), w(w)
	{}

	vec4(float r)
		: x(r), y(r), z(r), w(r)
	{}

	bool operator==(const vec4& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool operator!=(const vec4& other) const
	{
		return !(*this == other);
	}

	uint GetHash() const
	{
		return (*(uint*)&x) ^ ((*(uint*)&y) << 14) ^ ((*(uint*)&z) << 23) ^ ((*(uint*)&w) << 31);
	}
};


struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
	vec4 jointWeights;
	vec4 jointIndices;

	bool operator==(const Vertex& other) const
	{
		return position == other.position && normal == other.normal && uv == other.uv && binormal == other.binormal && tangent == other.tangent && jointWeights == other.jointWeights && jointIndices == other.jointIndices;
	}
};

template<>
struct std::hash<Vertex>
{
	const size_t operator()(const Vertex& key) const
	{
		return key.position.GetHash() ^ key.normal.GetHash() ^ key.uv.GetHash() ^ key.binormal.GetHash() ^ key.tangent.GetHash();
	}
};

struct BlendingIndexWeightPair
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	BlendingIndexWeightPair() : 
		mBlendingIndex(0),
		mBlendingWeight(0)
	{}
};

// Each Control Point in FBX is basically a vertex
// in the physical world. For example, a cube has 8
// vertices(Control Points) in FBX
// Joints are associated with Control Points in FBX
// The mapping is one joint corresponding to 4
// Control Points(Reverse of what is done in a game engine)
// As a result, this struct stores a XMFLOAT3 and a 
// vector of joint indices
struct CtrlPoint
{
	vec3 mPosition;
	std::vector<BlendingIndexWeightPair> mBlendingInfo;

	CtrlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};

// This stores the information of each key frame of each joint
// This is a linked list and each node is a snapshot of the
// global transformation of the joint at a certain frame
struct Keyframe
{
	FbxLongLong mFrameNum;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	Keyframe() :
		mNext(nullptr)
	{}
};

// This is the actual representation of a joint in a game engine
struct Joint
{
	std::string mName;
	int mParentIndex;
	FbxAMatrix mGlobalBindposeInverse;
	Keyframe* mAnimation;
	FbxNode* mNode;

	Joint() :
		mNode(nullptr),
		mAnimation(nullptr)
	{
		mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
	}

	~Joint()
	{
		while(mAnimation)
		{
			Keyframe* temp = mAnimation->mNext;
			delete mAnimation;
			mAnimation = temp;
		}
	}
};

struct Skeleton
{
	std::vector<Joint> mJoints;
};

class Utilities
{
public:

	// This function should be changed if exporting to another format
	static void WriteMatrix(std::ostream& inStream, FbxMatrix& inMatrix, bool inIsRoot);

	static void PrintMatrix(FbxMatrix& inMatrix);
	
	static FbxAMatrix GetGeometryTransformation(FbxNode* inNode);

	static std::string GetFileName(const std::string& inInput);

	static std::string RemoveSuffix(const std::string& inInput);
};

