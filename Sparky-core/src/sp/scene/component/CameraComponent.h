#pragma once

#include "Component.h"

#include "sp/Common.h"

#include "sp\graphics\camera\Camera.h"

namespace sp { namespace scene { namespace component {

	class SP_API CameraComponent : public Component
	{
	public:
		graphics::Camera* camera;
	public:
		CameraComponent(graphics::Camera* camera);

		static ComponentType* GetStaticType()
		{
			static ComponentType type({ "Camera" });
			return &type;
		}

		inline virtual ComponentType* GetType() const override { return GetStaticType(); }

		void OnUpdate(const sp::Timestep& ts) override;

		void SetRotation(maths::Quaternion& rot);
		void SetPosition(maths::vec3& pos);
	};

} } }

