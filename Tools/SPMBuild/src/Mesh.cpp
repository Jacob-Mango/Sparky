#include "Mesh.h"

bool Mesh::LoadMesh(const std::string& Filename)
{
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (pScene) {
		m_Entries.resize(pScene->mNumMeshes);
		m_Textures.resize(pScene->mNumMaterials);

		for (unsigned int i = 0; i < m_Entries.size(); i++) 
			InitMesh(i, pScene);

		InitMaterials(pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}

	return true;
}

aiNode* FindNode(aiNode* Node, String name)
{
	for (int i = 0; i < Node->mNumChildren; i++) {
		if (StringEquals(Node->mChildren[i]->mName.C_Str(), name)) return Node->mChildren[i];
		aiNode* out = FindNode(Node->mChildren[i], name);
		if (out != nullptr) return out;
	}
	return nullptr;
}

uint Mesh::FindBone(String name)
{
	for (int i = 0; i < m_NumBones; i++) {
		if (StringEquals(m_BoneInfo[i]->Name, name)) return i;
	}
	return -1;
}

void Mesh::InitMesh(unsigned int Index, const aiScene* pScene)
{
	const aiMesh* paiMesh = pScene->mMeshes[Index];
	std::vector<Vertex>* Vertices = new std::vector<Vertex>();
	std::vector<uint>* Indices = new std::vector<uint>();
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]) ? &(paiMesh->mNormals[i]) : &Zero3D;
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Vertex v;

		v.position = vec3(pPos->x, pPos->y, pPos->z);
		v.uv = vec2(pTexCoord->x, pTexCoord->y);
		v.normal = vec3(pNormal->x, pNormal->y, pNormal->z);

		Vertices->push_back(v);
	}

	if (paiMesh->HasBones()) {
		int boneArraysSize = paiMesh->mNumVertices * 4;
		
		for (unsigned int i = 0; i < paiMesh->mNumBones; i++) {
			unsigned int BoneIndex = 0;
			aiBone* asBone = paiMesh->mBones[i];

			std::string BoneName(asBone->mName.data);
			
			if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
				BoneIndex = m_NumBones;
				m_NumBones++;

				Bone* bi = new Bone();
				m_BoneInfo.push_back(bi);

				m_BoneInfo[BoneIndex]->Name = BoneName;
				m_BoneInfo[BoneIndex]->Index = BoneIndex;

				aiMatrix4x4 off = asBone->mOffsetMatrix;
				vec4 row1 = { off.a1, off.a2, off.a3, off.a4 };
				vec4 row2 = { off.b1, off.b2, off.b3, off.b4 };
				vec4 row3 = { off.c1, off.c2, off.c3, off.c4 };
				vec4 row4 = { off.d1, off.d2, off.d3, off.d4 };

				m_BoneInfo[BoneIndex]->BoneOffset = mat4::Transpose(mat4(row1, row2, row3, row4));
				m_BoneMapping[BoneName] = BoneIndex;
			}
			else {
				BoneIndex = m_BoneMapping[BoneName];
			}

			const int numWeights = asBone->mNumWeights;

			for (int j = 0; j < numWeights; j++)
			{
				aiVertexWeight weight = asBone->mWeights[j];
				Vertices->at(weight.mVertexId).jointIndices = vec4(BoneIndex);
				Vertices->at(weight.mVertexId).jointWeights = vec4(weight.mWeight);
			}
		}
	}
	else {
		for (unsigned int j = 0; j < Vertices->size(); j++) {
			Vertices->at(j).jointIndices = vec4(0);
			Vertices->at(j).jointWeights = vec4(1);
		}
	}

	for (int i = 0; i < m_NumBones; i++) {
		aiNode* node = FindNode(pScene->mRootNode, m_BoneInfo[i]->Name);
		if (node->mParent) {
			m_BoneInfo[i]->ParentIndex = FindBone(node->mParent->mName.C_Str());
		}
		else {
			m_BoneInfo[i]->ParentIndex = -1;
		}
	}

	for (int i = 0; i < m_NumBones; i++) {
		if (m_BoneInfo[i]->ParentIndex == -1) continue;

		m_BoneInfo[i]->Parent = m_BoneInfo[m_BoneInfo[i]->ParentIndex];
		m_BoneInfo[m_BoneInfo[i]->ParentIndex]->Children.push_back(m_BoneInfo[i]);
	}

	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& face = paiMesh->mFaces[i];
		
		for (int j = 0; j < face.mNumIndices; j++)
			Indices->push_back(face.mIndices[j]);
	}

	m_Entries[Index].Set(Vertices, Indices);
	m_Entries[Index].materialIndex = paiMesh->mMaterialIndex;
	m_Entries[Index].Optimise();

}

void MeshEntry::Optimise()
{
	std::vector<Vertex>* tVertices = new std::vector<Vertex>();
	std::vector<uint>* tIndices = new std::vector<uint>();

	for (int i = 0; i < Indices->size(); i++)
	{
		uint nIndex = Indices->at(i);
		for (int j = 0; j < tVertices->size(); j++) {
			if (tVertices->at(j).Equals(Vertices->at(nIndex)))
				nIndex = j;
		}
		tIndices->push_back(nIndex);
		tVertices->push_back(Vertices->at(nIndex));
	}

	Vertices = tVertices;
	Indices = tIndices;
}

void Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				m_Textures[i] = Path.data;
			}
		}
	}
}