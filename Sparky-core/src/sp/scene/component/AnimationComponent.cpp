#include "sp/sp.h"
#include "AnimationComponent.h"

#include "../Object.h"

namespace sp { namespace scene { namespace component {

AnimationComponent::AnimationComponent(maths::mat4 jo[NUMBONES])
{
	for (int i = 0; i < NUMBONES; i++) {
		joints[i] = jo[i];
	}
}

AnimationComponent::AnimationComponent()
{
	for (int i = 0; i < NUMBONES; i++) {
		joints[i] = maths::mat4(1.0);
	}
}

void AnimationComponent::SetJoint(int index, const maths::mat4& transform)
{
	joints[index] = transform;
}

bool FindBone(graphics::Bone* bone, String name, int& ID)
{
	if (StringEquals(bone->Name, name)) {
		ID = bone->ID;
		return true;
	}
	for (int i = 0; i < bone->Children.size(); i++) {
		if (FindBone(bone->Children[i], name, ID)) return true;
	}
	return false;
}

void AnimationComponent::SetJoint(String name, const maths::mat4& transform)
{
	graphics::Bone* rootBone = m_Parent->GetComponent<MeshComponent>()->mesh->m_RootBone;
	graphics::Bone* currentBone = nullptr;
	int index = -1;
	FindBone(rootBone, name, index);
	if (index == -1) {
		SP_ERROR("Error, can't find bone!");
		return;
	}
	SetJoint(index, transform);
}

} } }