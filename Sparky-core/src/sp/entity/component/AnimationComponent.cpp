#include "sp/sp.h"
#include "AnimationComponent.h"

#include "../Entity.h"

namespace sp { namespace entity { namespace component {

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

bool FindBone(graphics::Bone* bone, String name, int& index)
{
	if (StringEquals(bone->Name, name)) {
		index = bone->Index;
		return true;
	}
	for (int i = 0; i < bone->Children.size(); i++) {
		if (FindBone(bone->Children[i], name, index)) return true;
	}
	return false;
}

void AnimationComponent::SetJoint(String name, const maths::mat4& transform)
{
	graphics::Bone* rootBone = GetEntity()->GetComponent<MeshComponent>()->mesh->m_RootBone;
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