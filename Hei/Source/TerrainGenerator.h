#pragma once

#include "Core/Components/Mesh.h"
#include "MarchingCubes.h"
#include "Core/Logger.h"
#include "PerlinNoise.hpp"
#include "Core/Components/Material.h"

namespace Hei {

    class TerrainGenerator: public PetrolEngine::Component {
    public:
        PetrolEngine::Material material;
        const glm::ivec3 chunkSize = {16, 16, 16};
        TerrainGenerator(uint64 seed);

        uint64 seed = 2137;
        void generateTerrain(glm::ivec3 offset);

        void onStart();
        void onUpdate();

    private:
        siv::PerlinNoise perlin;
    };
}
