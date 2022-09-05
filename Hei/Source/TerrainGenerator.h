#pragma once

#include "Components/Mesh.h"
#include "MarchingCubes.h"
#include "Core/Logger.h"
#include "PerlinNoise.hpp"

namespace Hei {

    class TerrainGenerator {
    public:
        TerrainGenerator(uint64 seed);

        uint64 seed = 213;
        void generateTerrain(int width, int height, int depth, PetrolEngine::Mesh* mesh);

    private:
        siv::PerlinNoise perlin;
    };
}