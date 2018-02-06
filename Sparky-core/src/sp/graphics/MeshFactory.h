#pragma once

#include "sp/Common.h"
#include "sp/Types.h"

#include "sp/maths/maths.h"

#include "sp/graphics/Mesh.h"
#include "sp/graphics/API/VertexArray.h"

#include "sp/graphics/material/Material.h"

namespace sp { namespace graphics { namespace MeshFactory {

	SP_API Mesh* CreatePoint(MaterialInstance* material);

	SP_API Mesh* CreateQuad(float x, float y, float width, float height, MaterialInstance* material);
	SP_API Mesh* CreateQuad(const maths::vec2& position, const maths::vec2& size, MaterialInstance* material);

	SP_API Mesh* CreateCube(float size, MaterialInstance* material);
	SP_API Mesh* CreatePlane(float size, const maths::vec3& normal, MaterialInstance* material);
	SP_API Mesh* CreatePlane2(float size, float density, MaterialInstance* material);

	SP_API Mesh* CreateSphere(float size, MaterialInstance* material);

} } }