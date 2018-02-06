#pragma once
#include "Component.h"

#include "sp\graphics\Renderer3D.h"

namespace sp { namespace entity { namespace component {

	class SP_API AnimationComponent : public Component
	{
	public:
		maths::mat4* joints = spnew maths::mat4[NUMBONES];
	public:
		AnimationComponent(maths::mat4 jo[NUMBONES]);
		AnimationComponent();

		void SetJoint(int index, const maths::mat4& transform);
		void SetJoint(String name, const maths::mat4& transform);

		static ComponentType* GetStaticType()
		{
			static ComponentType type({ "Animation" });
			return &type;
		}

		inline virtual ComponentType* GetType() const override { return GetStaticType(); }
	};

} } }