#shader vertex
#include "/shaders/Terrain/Terrain.vert"
#shader end

#shader tess_cont
#include "/shaders/Terrain/Terrain.tesc"
#shader end

#shader tess_eval
#include "/shaders/Terrain/Terrain.tese"
#shader end

#shader geometry
#include "/shaders/Terrain/Terrain.geo"
#shader end

#shader fragment
#include "/shaders/Terrain/Terrain.frag"
#shader end