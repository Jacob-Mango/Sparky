#pragma once

#include <Sparky.h>

namespace sp {
	namespace scene {
		namespace terrain {
			class TerrainNode : public sp::scene::Object
			{
			public:
				TerrainNode();
				~TerrainNode();

				void OnInit() override;
				void OnUpdate(const sp::Timestep& ts) override;
			};
		}
	}
}