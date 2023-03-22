#include "TerrainGenerator.h"

#include <cmath>

#include "Bullet.h"
#include "Core/Aliases.h"
#include "Core/Components/Mesh.h"
#include "Core/Components/Transform.h"
#include "Core/Files.h"
#include "Core/Scene.h"
//#include <filesystem>
#include <ostream>
#include <fstream>
#include <math.h>

#include "PerlinNoise.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_int3.hpp"
#include "glm/fwd.hpp"

//#include <filesystem>
//#if __cplusplus >= 201703l
//#error fgdfgfdg
//#endif


glm::vec3 CalcSurfaceNormal(glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3) {
    glm::vec3 u = tri2 - tri1;
    glm::vec3 v = tri3 - tri1;

    return glm::normalize(glm::cross(u, v));
}

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

namespace Hei {

    void GameLoader::load(String path){
        String save = ReadFile("saves/" + path + "/data.txt");
        std::cout<<save<<std::endl;
    }

    void GameLoader::newSave(String name){
        String cwd = fs::current_path().string();
        String cmd = "mkdir " + cwd + "/saves";
/*
        for3D(cloudSize.x, cloudSize.y, cloudSize.z) {
            auto a = glm::vec3((offset + glm::ivec3(x, y, z))) * f;
            //a.y -= (int)a.y % 3;
            double axd = perlin.octave2D_01(a.x * 0.01f, a.z * 0.01f, 8);
            double v = 0.0;
            if (axd > (a.y / (float)cloudSize.y)) v = 1.0;
            //std::cout<<axd<< " > " << y/(double)cloudSize.y << std::endl;
            //double v = perlin.octave3D_01(a.x * 0.01f, a.y * 0.01f, a.z * 0.01f, 8);

            fieldCloud[(z * cloudSize.x * cloudSize.y) + (y * cloudSize.x) + x] = (uint8)(v * 255.0);
        }
*/
        if(!fs::exists(cwd + "/saves/" + name)){
            String cmd = "mkdir " + cwd + "/saves/" + name;
            system(cmd.c_str());
        }
        
        std::ofstream myfile;
        myfile.open (cwd + "/saves/" + name + "/" + "data.txt");
        myfile << "test run";
        myfile.close();
    }

    
    ChunkData* GameLoader::getChunk(glm::ivec3 offsets){
        String file = this->path + "/a.map";
        std::cout<< "requested chunk x: "<<offsets.x<<" y: "<<offsets.y<<" z: " <<offsets.z<<std::endl;
        auto offset = offsets * chunkSize;
        ChunkData* chunk = new ChunkData(this->chunkSize, offset);
        auto& blocks = chunk->blocks;
        
        const glm::vec3 f = glm::vec3(chunkSize) * (1.f / 8.f);
 
        for3D(chunkSize.x, chunkSize.y, chunkSize.z) {
            auto a = glm::vec3(((offset) + glm::ivec3(x, y-(0), z))) * f;
            double axd = perlin.octave2D_01(a.x * 0.01f, a.z * 0.01f, 8);
            blocks[x][y][z] = BlockData(0);
            if (axd > (a.y / (float)chunkSize.y)) blocks[x][y][z] = BlockData(1);
        }

        return chunk;
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

    void GameLoader::generateChunk(glm::ivec3 offset, int level){
        /*
        using iv3 = glm::ivec3;

        if(level == 0) return;

        ChunkData* chunk = chunks[offset];

        chunk->entity = parent->getScene()->createGameObject("Chunk", parent);
        chunk->entity->getComponent<PetrolEngine::Transform>().setPosition(glm::vec3(offset) / 2.f);
        Mesh& mesh = chunk->entity->addComponent<Mesh>();

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
            
            if(x < 0 || y < 0 || z < 0)
                ts -= iv3(x < 0 ? chunkSize.x : 0, y < 0 ? chunkSize.y : 0, z < 0 ? chunkSize.z : 0);
            if(x == chunkSize.x || y == chunkSize.y || z == chunkSize.z)
                ts += iv3(x == chunkSize.x ? chunkSize.x : 0, y == chunkSize.y ? chunkSize.y : 0, z == chunkSize.z ? chunkSize.z : 0);

            auto blockChunk = chunks.find(ts);

            if(blockChunk == chunks.end()){//std::cout<<"did not found\n";
            return BlockData(0);
        }
            x = x < 0 ? chunkSize.x - 1 : x;
            y = y < 0 ? chunkSize.y - 1 : y;
            z = z < 0 ? chunkSize.z - 1 : z;
            
            x = x >= chunkSize.x ? 0 : x;
            y = y >= chunkSize.y ? 0 : y;
            z = z >= chunkSize.z ? 0 : z;

            return blockChunk->second->blocks[x][y][z];
        };
        
        auto& blocks = chunk->blocks;
        for3D(chunkSize.x, chunkSize.y, chunkSize.z) {
            if(blocks[x][y][z].id == 0) continue;

            if(findBlock(offset, iv3(x-1, y+0, z+0)).id == 0) addFace(2, glm::vec3(x,y,z));
            if(findBlock(offset, iv3(x+0, y-1, z+0)).id == 0) addFace(4, glm::vec3(x,y,z));
            if(findBlock(offset, iv3(x+0, y+0, z-1)).id == 0) addFace(0, glm::vec3(x,y,z));
            
            if(findBlock(offset, iv3(x+1, y+0, z+0)).id == 0) addFace(3, glm::vec3(x,y,z));
            if(findBlock(offset, iv3(x-0, y+1, z+0)).id == 0) addFace(5, glm::vec3(x,y,z));
            if(findBlock(offset, iv3(x-0, y+0, z+1)).id == 0) addFace(1, glm::vec3(x,y,z));

*//*
            if(y > 0 && chunk->blocks[x+0][y-1][z+0].id == 0) addFace(4, glm::vec3(x,y,z));
            if(x > 0 && chunk->blocks[x-1][y+0][z+0].id == 0) addFace(2, glm::vec3(x,y,z));
            if(z > 0 && chunk->blocks[x+0][y+0][z-1].id == 0) addFace(0, glm::vec3(x,y,z));
            
            if(x < chunkSize.x-1 && blocks[x+1][y+0][z+0].id == 0) addFace(3, glm::vec3(x,y,z));
            if(y < chunkSize.y-1 && blocks[x+0][y+1][z+0].id == 0) addFace(5, glm::vec3(x,y,z));
            if(z < chunkSize.z-1 && blocks[x+0][y+0][z+1].id == 0) addFace(1, glm::vec3(x,y,z));
        *//*
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

        if(mesh.vertices.size() > 0) chunk->entity->addComponent<MeshCollider>(0, false);

        chunks[offset] = chunk;*/
    }

    void GameLoader::generateAround(glm::vec3 position, int radius){
        glm::ivec3 centerOffset = this->worldToOffset(position);
        
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

                    //generateChunk(chunkOffset, (int)((float)radius - distance));
                }
            }
        }
    }

}
