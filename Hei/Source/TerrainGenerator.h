#pragma once

#include "Components/Mesh.h"
#include "MarchingCubes.h"
#include "Core/Logger.h"
#include "PerlinNoise.hpp"
#include "Components/Entity.h"

struct Vec3Hash {
    size_t operator()(const glm::vec3& k) const {
        return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
    }

    bool operator()(const glm::vec3& a, const glm::vec3& b) const {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

namespace Hei {
    using PetrolEngine::Entity;

    class TerrainGenerator {
    public:
        TerrainGenerator(uint64 seed, Entity* parent);

        uint64 seed = 213;
        Entity* generateChunk(glm::ivec3 offset);
        void generateTerrainAround(glm::vec3 position, int radius);

        PetrolEngine::Material material;
        void setMaterial(PetrolEngine::Material material) {
            this->material = material;
        }

    private:
        Entity* parent = nullptr;
        UnorderedMap<glm::vec3, Entity*, Vec3Hash, Vec3Hash> chunks;

        glm::ivec3 chunkSize = {16, 16, 16};
        siv::PerlinNoise perlin;
    };
}