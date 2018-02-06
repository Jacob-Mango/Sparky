#include "FBXExporter.h"
#include "Utilities.h"
#include <fstream>
#include <sstream>
#include <iomanip>

#include <stdlib.h>

enum VertexAttribute
{
	POSITION = BIT(0),
	NORMAL = BIT(1),
	UV = BIT(2),
	BINORMAL = BIT(3),
	TANGENT = BIT(4),
	JOINTWEIGHT = BIT(5),
	JOINTINDEX = BIT(6)
};

struct SPMFormat
{
	char* header = "SPMF";
	byte nameLength;
	char* name;
	uint attributes; // Bitfield
	uint vertexBufferSize;
	byte* vertexData;
	uint indexBufferSize;
	byte* indexData;
	// TODO: Materials
	char* footer = "1234";
};

void WriteBytes(FILE* file, const byte* data, uint size)
{
	fwrite(data, 1, size, file);
}

FBXExporter::FBXExporter()
{
	mFBXManager = nullptr;
	mFBXScene = nullptr;
	mTriangleCount = 0;
	mHasAnimation = true;
}

bool FBXExporter::Initialize()
{
	mFBXManager = FbxManager::Create();
	if (!mFBXManager)
	{
		return false;
	}

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(mFBXManager, IOSROOT);
	mFBXManager->SetIOSettings(fbxIOSettings);

	mFBXScene = FbxScene::Create(mFBXManager, "myScene");

	return true;
}

bool FBXExporter::LoadScene(const char* inFileName, const char* inOutputPath)
{
	
	mInputFilePath = inFileName;
	mOutputFilePath = inOutputPath;

	FbxImporter* fbxImporter = FbxImporter::Create(mFBXManager, "myImporter");

	if (!fbxImporter)
	{
		std::cout << "Importer is null!" << std::endl;
		return false;
	}

	if (!fbxImporter->Initialize(inFileName, -1, mFBXManager->GetIOSettings()))
	{
		std::cout << "Could not initialize Importer!" << std::endl;
		return false;
	}

	if (!fbxImporter->Import(mFBXScene))
	{
		std::cout << "Could not import scene!" << std::endl;
		return false;
	}
	fbxImporter->Destroy();
	std::cout << "Loading FBX File \n";

	return true;
}

void FBXExporter::ExportFBX()
{

	ProcessSkeletonHierarchy(mFBXScene->GetRootNode());
	if (mSkeleton.mJoints.empty())
	{
		mHasAnimation = false;
	}

	ProcessGeometry(mFBXScene->GetRootNode());


	String name = "name";

	SPMFormat format;

	format.nameLength = name.length();
	format.name = &name[0];
	format.attributes = POSITION | NORMAL | UV | BINORMAL | TANGENT;
	format.vertexBufferSize = mVertices.size() * sizeof(Vertex);
	format.vertexData = (byte*)&mVertices[0];
	format.indexBufferSize = mIndices.size() * sizeof(uint);
	format.indexData = (byte*)&mIndices[0];

	FILE* f = fopen(mOutputFilePath.c_str(), "wb");
	WriteBytes(f, (byte*)format.header, 4);
	WriteBytes(f, &format.nameLength, 1);
	WriteBytes(f, (byte*)format.name, format.nameLength);
	WriteBytes(f, (byte*)&format.attributes, sizeof(uint));
	WriteBytes(f, (byte*)&format.vertexBufferSize, sizeof(uint));
	WriteBytes(f, format.vertexData, format.vertexBufferSize);
	WriteBytes(f, (byte*)&format.indexBufferSize, sizeof(uint));
	WriteBytes(f, format.indexData, format.indexBufferSize);
	WriteBytes(f, (byte*)format.footer, 4);
	fclose(f);
}

void FBXExporter::ProcessGeometry(FbxNode* inNode)
{
	if (inNode->GetNodeAttribute())
	{
		switch (inNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			ProcessControlPoints(inNode);

			if(mHasAnimation)
			{
				ProcessJointsAndAnimations(inNode);
			}

			ProcessMesh(inNode);
			break;
		}
	}

	for (int i = 0; i < inNode->GetChildCount(); ++i)
	{
		ProcessGeometry(inNode->GetChild(i));
	}
}

void FBXExporter::ProcessSkeletonHierarchy(FbxNode* inRootNode)
{

	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = inRootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1);
	}
}

void FBXExporter::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex)
{
	if(inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currJoint;
		currJoint.mParentIndex = inParentIndex;
		currJoint.mName = inNode->GetName();
		mSkeleton.mJoints.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), inDepth + 1, mSkeleton.mJoints.size(), myIndex);
	}
}

void FBXExporter::ProcessControlPoints(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	for(unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CtrlPoint* currCtrlPoint = new CtrlPoint();
		vec3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint->mPosition = currPosition;
		mControlPoints[i] = currCtrlPoint;
	}
}

void FBXExporter::ProcessJointsAndAnimations(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();
	// This geometry transform is something I cannot understand
	// I think it is from MotionBuilder
	// If you are using Maya for your models, 99% this is just an
	// identity matrix
	// But I am taking it into account anyways......
	FbxAMatrix geometryTransform = Utilities::GetGeometryTransformation(inNode);

	// A deformer is a FBX thing, which contains some clusters
	// A cluster contains a link, which is basically a joint
	// Normally, there is only one deformer in a mesh
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin)
		{
			continue;
		}

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName);
			FbxAMatrix transformMatrix;						
			FbxAMatrix transformLinkMatrix;					
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix);	// The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			// Update the information in mSkeleton 
			mSkeleton.mJoints[currJointIndex].mGlobalBindposeInverse = globalBindposeInverseMatrix;
			mSkeleton.mJoints[currJointIndex].mNode = currCluster->GetLink();

			// Associate each joint with the control points it affects
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.mBlendingIndex = currJointIndex;
				currBlendingIndexWeightPair.mBlendingWeight = currCluster->GetControlPointWeights()[i];
				mControlPoints[currCluster->GetControlPointIndices()[i]]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
			}

			// Get animation information
			// Now only supports one take
			FbxAnimStack* currAnimStack = mFBXScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			mAnimationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = mFBXScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			Keyframe** currAnim = &mSkeleton.mJoints[currJointIndex].mAnimation;

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new Keyframe();
				(*currAnim)->mFrameNum = i;
				FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
				(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				currAnim = &((*currAnim)->mNext);
			}
		}
	}

	// Some of the control points only have less than 4 joints
	// affecting them.
	// For a normal renderer, there are usually 4 joints
	// I am adding more dummy joints if there isn't enough
	BlendingIndexWeightPair currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.mBlendingIndex = 0;
	currBlendingIndexWeightPair.mBlendingWeight = 0;
	for(auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	{
		for(unsigned int i = itr->second->mBlendingInfo.size(); i <= 4; ++i)
		{
			itr->second->mBlendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}

unsigned int FBXExporter::FindJointIndexUsingName(const std::string& inJointName)
{
	for(unsigned int i = 0; i < mSkeleton.mJoints.size(); ++i)
	{
		if (mSkeleton.mJoints[i].mName == inJointName)
		{
			return i;
		}
	}

	throw std::exception("Skeleton information in FBX file is corrupted.");
}

void FBXExporter::InsertVertex(const vec3& position, const vec3& normal, const vec2& uv, const vec3& binormal, const vec3& tangent, const vec4& weight, const vec4& jIndex)
{
	Vertex vertex = { position, normal, uv, binormal, tangent, weight, jIndex };
	auto lookup = mIndexMapping.find(vertex);
	if (lookup != mIndexMapping.end())
	{
		mIndices.push_back(lookup->second);
	}
	else
	{
		uint index = mVertices.size();
		mIndexMapping[vertex] = index;
		mIndices.push_back(index);
		mVertices.push_back(vertex);
	}
}

void FBXExporter::ProcessMesh(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();

	if (currMesh && !currMesh->GetUserDataPtr()) {
		std::cout << "test";

		mTriangleCount = currMesh->GetPolygonCount();

		int vertexCounter = 0;
		mIndices.resize(mTriangleCount);
		mVertices.resize(mTriangleCount * 3);

		try {

			for (unsigned int i = 0; i < mTriangleCount; ++i)
			{
				vec3 normal[3] = { vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f) };
				vec3 tangent[3];
				vec3 binormal[3];
				vec2 UV[3];
				vec4 jointIndices[3];
				vec4 jointWeights[3] = { vec4(1.0f), vec4(1.0f), vec4(1.0f) };

				for (unsigned int j = 0; j < 3; j++)
				{
					int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);
					CtrlPoint* currCtrlPoint = mControlPoints[ctrlPointIndex];

					//ReadTangent(currMesh, ctrlPointIndex, vertexCounter, tangent[j]);
					//ReadBinormal(currMesh, ctrlPointIndex, vertexCounter, binormal[j]);
					//ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(i, j), 0, UV[j]);
					//ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normal[j]);


					int index[4];
					float weights[4];

					if (mHasAnimation) {
						for (unsigned int k = 0; k < 4; ++k)
						{
							BlendingIndexWeightPair blend = currCtrlPoint->mBlendingInfo[k];

							index[k] = blend.mBlendingIndex;
							weights[k] = blend.mBlendingWeight;
						}

						jointWeights[j] = vec4(index[0], index[1], index[2], index[3]);
						jointWeights[j] = vec4(weights[0], weights[1], weights[2], weights[3]);
					}

					InsertVertex(currCtrlPoint->mPosition, normal[j], UV[j], tangent[j], binormal[j], jointWeights[j], jointIndices[j]);

					++vertexCounter;
				}
			}

		}
		catch (std::exception e) {
			std::cout << "Errror" << std::endl;
		}
	}

	for(auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	{
		delete itr->second;
	}
	mControlPoints.clear();
}

void FBXExporter::ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, vec2& outUV)
{
	if (inUVLayer >= 2 || inMesh->GetElementUVCount() <= inUVLayer)
	{
		throw std::exception("Invalid UV Layer Number");
	}

	FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(inUVLayer);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FBXExporter::ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, vec3& outNormal)
{
	if(inMesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
	switch(vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FBXExporter::ReadBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, vec3& outBinormal)
{
	if(inMesh->GetElementBinormalCount() < 1)
	{
		throw std::exception("Invalid Binormal Number");
	}

	FbxGeometryElementBinormal* vertexBinormal = inMesh->GetElementBinormal(0);
	switch(vertexBinormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBinormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBinormal->GetIndexArray().GetAt(inVertexCounter);
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FBXExporter::ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, vec3& outTangent)
{
	if(inMesh->GetElementTangentCount() < 1)
	{
		throw std::exception("Invalid Tangent Number");
	}

	FbxGeometryElementTangent* vertexTangent = inMesh->GetElementTangent(0);
	switch(vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inCtrlPointIndex);
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inVertexCounter);
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

/*
void FBXExporter::WriteAnimationToStream(std::ostream& inStream)
{
	inStream << "<?xml version='1.0' encoding='UTF-8' ?>" << std::endl;
	inStream << "<itpanim>" << std::endl;
	inStream << "\t<skeleton count='" << mSkeleton.mJoints.size() << "'>" << std::endl;
	for (unsigned int i = 0; i < mSkeleton.mJoints.size(); ++i)
	{
		inStream << "\t\t<joint id='" << i << "' name='" << mSkeleton.mJoints[i].mName << "' parent='" << mSkeleton.mJoints[i].mParentIndex << "'>\n";
		inStream << "\t\t\t";
		FbxVector4 translation = mSkeleton.mJoints[i].mGlobalBindposeInverse.GetT();
		FbxVector4 rotation = mSkeleton.mJoints[i].mGlobalBindposeInverse.GetR();
		translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
		rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);
		mSkeleton.mJoints[i].mGlobalBindposeInverse.SetT(translation);
		mSkeleton.mJoints[i].mGlobalBindposeInverse.SetR(rotation);
		FbxMatrix out = mSkeleton.mJoints[i].mGlobalBindposeInverse;

		Utilities::WriteMatrix(inStream, out.Transpose(), true);
		inStream << "\t\t</joint>\n";
	}
	inStream << "\t</skeleton>\n";
	inStream << "\t<animations>\n";
	inStream << "\t\t<animation name='" << mAnimationName << "' length='" << mAnimationLength << "'>\n";
	for (unsigned int i = 0; i < mSkeleton.mJoints.size(); ++i)
	{
		inStream << "\t\t\t" << "<track id = '" << i << "' name='" << mSkeleton.mJoints[i].mName << "'>\n";
		Keyframe* walker = mSkeleton.mJoints[i].mAnimation;
		while(walker)
		{
			inStream << "\t\t\t\t" << "<frame num='" << walker->mFrameNum - 1 << "'>\n";
			inStream << "\t\t\t\t\t";
			FbxVector4 translation = walker->mGlobalTransform.GetT();
			FbxVector4 rotation = walker->mGlobalTransform.GetR();
			translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
			rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);
			walker->mGlobalTransform.SetT(translation);
			walker->mGlobalTransform.SetR(rotation);
			FbxMatrix out = walker->mGlobalTransform;
			Utilities::WriteMatrix(inStream, out.Transpose(), true);
			inStream << "\t\t\t\t" << "</frame>\n";
			walker = walker->mNext;
		}
		inStream << "\t\t\t" << "</track>\n";
	}
	inStream << "\t\t</animation>\n";
	inStream << "</animations>\n";
	inStream << "</itpanim>";
}
*/