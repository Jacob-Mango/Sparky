#pragma once

#include <Sparky.h>

#ifndef TERRAINNODE_H_
#define TERRAINNODE_H_
namespace sp {
	namespace scene {
		namespace terrain {

			struct TerrainVertex
			{
				maths::vec3 position;
			};

			class TerrainNode : public sp::scene::Object
			{
			private:
				sp::graphics::MaterialInstance* m_Instance;
				sp::graphics::Camera* m_Camera;
			public:
				TerrainNode(sp::graphics::Camera* camera);
				~TerrainNode();

				void OnInit() override;
				void OnUpdate(const sp::Timestep& ts) override;
			private:
				TerrainVertex CreateVertex(maths::vec3 pos);
			};
		}
	}
}
#endif