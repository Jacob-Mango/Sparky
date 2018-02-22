#pragma once

#include <Sparky.h>

#include "sp/Common.h"
#include "sp/Types.h"

#include "sp/maths/maths.h"

#include "sp/graphics/Mesh.h"
#include "sp/graphics/API/VertexArray.h"

#include "sp/graphics/material/Material.h"

#ifndef TERRAINNODE_H_
#define TERRAINNODE_H_
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
				
				// void CreatePatch();
			};
		}
	}
}
#endif