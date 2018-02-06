#pragma once

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "sp/sp.h"
#include "sp/entity/Entity.h"

namespace sp { namespace maths {

	struct vec2;
	struct vec3;
	struct vec4;
	struct mat4;

} }

namespace sp { namespace world { 

class HeightMap : public sp::entity::Entity
{
private:
	float AMPLITUDE = 75.0f;
	int OCTAVES = 6;
	float ROUGHNESS = 0.05f;
public:
	HeightMap(sp::maths::vec3 offset);

	void OnInit() override;
	void OnUpdate(const sp::Timestep& ts) override;
private:
	float generateHeight(int x, int z);
	float getInterpolatedNoise(float x, float z);
	float interpolate(float a, float b, float blend);
	float getSmoothNoise(int x, int z);
	float getNoise(int x, int z);
};

} }

#endif // !1