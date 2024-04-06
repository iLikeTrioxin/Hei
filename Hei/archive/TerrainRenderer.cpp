#include "TerrainRenderer.h"
#include "Chunk.h"
#include "glm/ext/vector_int3.hpp"

#include <Core/Components/Transform.h>
#include <Core/Components/Mesh.h>

static glm::vec3 CalcSurfaceNormal(glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3) {
    glm::vec3 u = tri2 - tri1;
    glm::vec3 v = tri3 - tri1;

    return glm::normalize(glm::cross(u, v));
}


namespace Hei {

    void TerrainManager::addChunk(){
        
    }

    void TerrainManager::renderChunks(){
        // TODO: optimize this shit
        for(int ix = 0; ix < this->chunksData.size(); ix++) {
            for(int iy = 0; iy < this->chunksData[ix].size(); iy++){
                for(int iz = 0; iz < this->chunksData[ix][iy].size(); iz++){
                    //if(this->chunksData[ix][iy][iz].sides   <       6) continue;
                    if(this->chunksData[ix][iy][iz].entity != nullptr) continue;
                    
                    renderChunkI(&this->chunksData[ix][iy][iz]);
                }
            }
        }
    }

    void TerrainManager::renderChunk(ChunkData *chunk, glm::ivec3 center){
        auto offset = chunk->offset + (chunkSize / 2) - center;

        auto& current = chunksData[offset.x][offset.y][offset.z];
        
        if(current.data != nullptr) return;

        current.data = chunk;
        
        if(offset.x < chunksData.size() - 1) chunksData[offset.x + 1][offset.y + 0][offset.z + 0].sides++;
        if(offset.y < chunksData.size() - 1) chunksData[offset.x + 0][offset.y + 1][offset.z + 0].sides++;
        if(offset.z < chunksData.size() - 1) chunksData[offset.x + 0][offset.y + 0][offset.z + 1].sides++;
        
        if(offset.x > 0) chunksData[offset.x - 1][offset.y - 0][offset.z - 0].sides++;
        if(offset.y > 0) chunksData[offset.x - 0][offset.y - 1][offset.z - 0].sides++;
        if(offset.z > 0) chunksData[offset.x - 0][offset.y - 0][offset.z - 1].sides++;

        renderChunks();
    }

    const Vector<Vector<glm::vec3>> facesv = {
        {{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0}}, // 0 front
        {{0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}}, // 1 back 1
        {{0,0,0}, {0,0,1}, {0,1,1}, {0,1,0}}, // 2 left
        {{1,0,0}, {1,0,1}, {1,1,1}, {1,1,0}}, // 3 right
        {{0,0,0}, {1,0,0}, {1,0,1}, {0,0,1}}, // 4 bottom
        {{0,1,0}, {1,1,0}, {1,1,1}, {0,1,1}}  // 5 top
    };

    const Vector<Vector<int>> facesi = {
        {2,1,0,0,3,2}, // 0 front
        {0,1,2,2,3,0}, // 1 back 1
        
        {0,1,2,2,3,0}, // 2 left
        {2,1,0,0,3,2}, // 3 right

        {0,1,2,2,3,0}, // 4 bottom
        {2,1,0,0,3,2}  // 5 top
    };

    void TerrainManager::renderChunkI(Chunk* chunk){
        using iv3 = glm::ivec3;
        using  v3 = glm:: vec3;

        //if(chunk->level == 0) return;

        //ChunkData* chunk = chunks[offset];
        
        auto offset = chunk->data->offset + iv3(chunkSize / 2);

        chunk->entity = (Entity*)parent->getScene()->createGameObject("Chunk", parent);
        auto& transform = chunk->entity->getComponent<PetrolEngine::Transform>();
        transform.setPosition(glm::vec3(chunk->data->offset * chunkSize) / 2.f);
        Mesh& mesh = chunk->entity->addComponent<Mesh>();
        mesh.meshRenderer->material = this->material;

        auto addFace = [&](int i, glm::vec3 p){
            int x = mesh.vertices.size();
           
            for(auto& i : facesi[i]) mesh.indices .push_back(i + x);
            for(auto& v : facesv[i]) mesh.vertices.push_back(v + p);
        };

        auto findBlock = [&](iv3 offset, iv3 pos) -> BlockData {
            auto x = pos.x;
            auto y = pos.y;
            auto z = pos.z;
            iv3 ts = offset;
             
            ts -= iv3(x < 0           , y < 0           , z < 0);
            ts += iv3(x == chunkSize.x, y == chunkSize.y, z == chunkSize.z);

            ChunkData* blockChunk = chunksData[ts.x][ts.y][ts.z].data;
            
            if(blockChunk == nullptr)
                return BlockData(0);
            
            x = x < 0 ? chunkSize.x - 1 : x;
            y = y < 0 ? chunkSize.y - 1 : y;
            z = z < 0 ? chunkSize.z - 1 : z;
            
            x = x >= chunkSize.x ? 0 : x;
            y = y >= chunkSize.y ? 0 : y;
            z = z >= chunkSize.z ? 0 : z;

            return blockChunk->blocks[x][y][z];
        };
        
        auto& blocks = chunk->data->blocks;
        for3D(chunkSize.x, chunkSize.y, chunkSize.z) {
            if(blocks[x][y][z].id == 0) continue;

            if(findBlock(offset, iv3(x-1, y+0, z+0)).id == 0) addFace(2, v3(x,y,z));
            if(findBlock(offset, iv3(x+0, y-1, z+0)).id == 0) addFace(4, v3(x,y,z));
            if(findBlock(offset, iv3(x+0, y+0, z-1)).id == 0) addFace(0, v3(x,y,z));
            
            if(findBlock(offset, iv3(x+1, y+0, z+0)).id == 0) addFace(3, v3(x,y,z));
            if(findBlock(offset, iv3(x-0, y+1, z+0)).id == 0) addFace(5, v3(x,y,z));
            if(findBlock(offset, iv3(x-0, y+0, z+1)).id == 0) addFace(1, v3(x,y,z));

/*
            if(y > 0 && chunk->blocks[x+0][y-1][z+0].id == 0) addFace(4, glm::vec3(x,y,z));
            if(x > 0 && chunk->blocks[x-1][y+0][z+0].id == 0) addFace(2, glm::vec3(x,y,z));
            if(z > 0 && chunk->blocks[x+0][y+0][z-1].id == 0) addFace(0, glm::vec3(x,y,z));
            
            if(x < chunkSize.x-1 && blocks[x+1][y+0][z+0].id == 0) addFace(3, glm::vec3(x,y,z));
            if(y < chunkSize.y-1 && blocks[x+0][y+1][z+0].id == 0) addFace(5, glm::vec3(x,y,z));
            if(z < chunkSize.z-1 && blocks[x+0][y+0][z+1].id == 0) addFace(1, glm::vec3(x,y,z));
        */
        }
        
        mesh.normals.resize(mesh.vertices.size());
        mesh.textureCoordinates.resize(mesh.vertices.size());

        for(int i = 0; i < mesh.indices.size(); i += 3) {
            glm::vec3 normal = CalcSurfaceNormal(
                    mesh.vertices[mesh.indices[i + 0]],
                    mesh.vertices[mesh.indices[i + 1]],
                    mesh.vertices[mesh.indices[i + 2]]
            );

            mesh.normals[mesh.indices[i + 0]] = normal;
            mesh.normals[mesh.indices[i + 1]] = normal;
            mesh.normals[mesh.indices[i + 2]] = normal;

            auto proj =[](glm::vec3 a) -> glm::vec2 {
                glm::vec2 proj;

                a = glm::normalize(a);

                proj += glm::vec2(a.y, a.z);
                proj += glm::vec2(a.x, a.z);
                proj += glm::vec2(a.x, a.y);

                return {proj.x, proj.y};
            };

            mesh.textureCoordinates.emplace_back(proj(mesh.vertices[mesh.indices[i + 0]]));
            mesh.textureCoordinates.emplace_back(proj(mesh.vertices[mesh.indices[i + 1]]));
            mesh.textureCoordinates.emplace_back(proj(mesh.vertices[mesh.indices[i + 2]]));
        }

        mesh.recalculateMesh();

        //if(mesh.vertices.size() > 0) chunk->entity->addComponent<MeshCollider>(0, false);

        //chunks[offset] = chunk;
    }

    /*void TerrainManager::generateAround(glm::vec3 position, int radius){
        glm::ivec3 centerOffset = position / chunkSize;
        
        radius += 1; // for lazy loading chunks

        int searchRadius = radius;
        
        for (int ix = -searchRadius; ix <= searchRadius; ix++) {
            for (int iy = -searchRadius; iy <= searchRadius; iy++) {
                for (int iz = -searchRadius; iz <= searchRadius; iz++) {
                    float distance = std::sqrt(ix*ix+iy*iy+iz*iz);
                    if(distance > radius) continue;
                
                    glm::ivec3 chunkOffset = glm::ivec3(ix, iy, iz) * chunkSize + centerOffset;

                    chunks[chunkOffset] = getChunk(chunkOffset);
                }
            }
        }

        for (int ix = -searchRadius; ix <= searchRadius; ix++) {
            for (int iy = -searchRadius; iy <= searchRadius; iy++) {
                for (int iz = -searchRadius; iz <= searchRadius; iz++) {
                    float distance = std::sqrt(ix*ix+iy*iy+iz*iz);
                    if(distance > radius) continue;
                
                    glm::ivec3 chunkOffset = glm::ivec3(ix, iy, iz) * chunkSize + centerOffset;

                    generateChunk(chunkOffset, (int)((float)radius - distance));
                }
            }
        }
    }*/    
}
