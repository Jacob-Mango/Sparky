#pragma once

#include <map>
#include <vector>

#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#include "maths/maths.h"
#include "Utils.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

using namespace sp;
using namespace maths;

struct Vertex
{
	maths::vec3 position;
	maths::vec3 normal;
	maths::vec2 uv;
	maths::vec3 binormal;
	maths::vec3 tangent;
	maths::vec4 jointWeights;
	maths::vec4 jointIndices;

	bool Equals(Vertex v)
	{
		return (position == v.position) && (normal == v.normal) && (uv == v.uv) && (binormal == v.binormal) && (tangent == v.tangent) && (jointWeights == v.jointWeights) && (jointIndices == v.jointIndices);
	}
};

class MeshEntry {
public:
	std::vector<Vertex>* Vertices;
	std::vector<uint>* Indices;


	int materialIndex;
	MeshEntry() {

	}

	void Set(std::vector<Vertex>* V, std::vector<uint>* I) {
		Vertices = V;
		Indices = I;
	}

	void Optimise();
};

struct Bone
{
public:
	int Index;
	int ParentIndex;
	Bone* Parent;
	std::vector<Bone*> Children;
	String Name;
	mat4 BoneOffset;
};

class Mesh
{
public:
	Mesh() {

	}

	~Mesh() {

	}

	bool LoadMesh(const std::string& Filename);


	std::vector<MeshEntry> m_Entries;
	std::vector<std::string> m_Textures;
	std::vector<Bone*> m_BoneInfo;
private:

	uint FindBone(String name);

	void InitMesh(unsigned int Index, const aiScene* pScene);
	void InitMaterials(const aiScene* pScene, const std::string& Filename);

	std::map<std::string, unsigned int> m_BoneMapping;
	unsigned int m_NumBones;

#define INVALID_MATERIAL 0xFFFFFFFF

};