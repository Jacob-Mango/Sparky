#pragma once

#include <Sparky.h>

#ifndef TERRAINNODE_H_
#define TERRAINNODE_H_
namespace sp {
	namespace scene {
		namespace terrain {

			struct TerrainVertex
			{
				maths::vec2 position;
			};

			class TerrainNode : public sp::scene::Object
			{
			public:
				TerrainNode();
				~TerrainNode();

				void OnInit() override;
				void OnUpdate(const sp::Timestep& ts) override;

			private:
				TerrainVertex CreateVertex(maths::vec2 pos);
			};
		}
	}
}
#endif