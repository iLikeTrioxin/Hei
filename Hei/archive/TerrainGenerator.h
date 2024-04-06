#pragma once

#include "Core/Components/Material.h"
#include "Core/Components/Mesh.h"
#include "Core/Components/Entity.h"
#include "Core/Logger.h"
#include "Chunk.h"

//#include "MarchingCubes.h"
#include "PerlinNoise.hpp"

namespace Hei {
    using namespace PetrolEngine; 

    class GameLoader {
    public:
        UnorderedMap<glm::ivec3, ChunkData*, Vec3Hash, Vec3Hash> chunks;
        glm::ivec3 chunkSize = {16, 16, 16};
        glm::ivec3 chunksInFile = {16, 16, 16};
        int seed = 2137;
        //Entity* parent;
        siv::PerlinNoise perlin;
        String path;
        void load(String path);
        void newSave(String name);

        glm::ivec3 worldToOffset(glm::vec3 pos) { return glm::ivec3(pos) / chunkSize; } ;
        ChunkData* getChunk(glm::ivec3 offset);
        void generateChunk(glm::ivec3 offset, int level);

        void generateAround(glm::vec3 pos, int radius);    

        GameLoader(){
            //this->parent = parent;
            perlin = siv::PerlinNoise(seed);
        }

        //ChunkData loadChunk(glm::ivec3 offset);
    };

}
