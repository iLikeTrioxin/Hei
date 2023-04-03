#pragma once

#include <Core/Aliases.h>
#include "Chunk.h"
#include <Core/Components/Entity.h>
#include <Core/Components/Material.h>

using namespace PetrolEngine;

namespace Hei {
    class TerrainManager {
        class Chunk{
        public:
            ChunkData* data   = nullptr;
            Entity*    entity = nullptr;
            uint8      sides  = 0;
            uint8      level  = 0;
        };
    public:
        //UnorderedMap<glm::ivec3, ChunkData*, Vec3Hash, Vec3Hash>chunksData;
        //UnorderedMap<glm::ivec3, Entity*, Vec3Hash, Vec3Hash> chunks;
        void addChunk();
        void renderChunks();
        TerrainManager(Entity* parent, Material material){
            this->parent = parent;
            this->material = material;
            chunksData.resize(chunkSize.x);

            for(int ix =0 ;ix < chunkSize.x; ix++){
                chunksData[ix].resize(chunkSize.y);
                for(int iy = 0; iy < chunkSize.y; iy++){
                    chunksData[ix][iy].resize(chunkSize.z);
                }
            }
        }
        Material material;
        Vector<Vector<Vector<Chunk>>> chunksData;
        glm::ivec3 chunkSize = {16,16,16};
        void renderChunk(ChunkData* chunk, glm::ivec3 center);
        void renderChunkI(Chunk* chunk);
        Entity* parent;
    };
}
