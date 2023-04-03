#pragma once

#include <Core/Aliases.h>
#include <glm/vec3.hpp>

struct Vec3Hash {
    size_t operator()(const glm::vec3& k) const {
        return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
    }

    bool operator()(const glm::vec3& a, const glm::vec3& b) const {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

namespace Hei {
    struct BlockData {
        uint8 id;

        BlockData(){}
        BlockData(uint8 id) : id(id){}
    };

    struct ChunkData {
        glm::ivec3 offset;
        Vector<Vector<Vector<BlockData>>> blocks;
        
        ChunkData(glm::ivec3 size, glm::ivec3 offset){
            this->offset = offset;
            blocks.resize(size.x);
            
            for(int x = 0; x < size.x; x++){
                blocks[x].resize(size.y);

                for(int y = 0; y < size.y; y++){
                    blocks[x][y].resize(size.z);
                }
            }
        }
    };
}
