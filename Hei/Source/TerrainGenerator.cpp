#include "TerrainGenerator.h"

#include <PCH.h>

#include <algorithm>
#include <cmath>

#include "Core/Aliases.h"
#include "Core/Components/Component.h"
#include "PerlinNoise.hpp"
#include <Core/Components/Mesh.h>
#include <Core/Components/Entity.h>
#include <Core/GameObject.h>
#include <Static/Renderer/Renderer.h>
#include "Movement.h"
#include "glm/ext/vector_int3.hpp"
#include <algorithm>

static const glm::ivec3 edgePos[8] = {
        {0, 0, 0},
        {1, 0, 0},
        {1, 0, 1},
        {0, 0, 1},
        {0, 1, 0},
        {1, 1, 0},
        {1, 1, 1},
        {0, 1, 1}
};


glm::vec3 CalcSurfaceNormal(glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3)
{
    //tri1 tri2 and tri3 are the triangles 3 vertices
    //Simple algorithm followed from another stack overflow article
    glm::vec3 u = tri2 - tri1;
    glm::vec3 v = tri3 - tri1;

    glm::vec3 nrmcross = glm::cross(u, v);
    nrmcross = glm::normalize(nrmcross);
    return nrmcross;
}

struct Vec3Hash {
    size_t operator()(const glm::vec3& k)const
    {
        return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
    }

    bool operator()(const glm::vec3& a, const glm::vec3& b)const
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

namespace Hei{

    TerrainGenerator::TerrainGenerator(uint64 seed) {
        auto frag = PetrolEngine::ReadFile("Resources/Shaders/terrain.frag");
        auto vert = PetrolEngine::ReadFile("Resources/Shaders/terrain.vert");
        material.shader = PetrolEngine::Renderer::loadShader("terrain", vert, frag);
        material.textures.push_back(PetrolEngine::Renderer::createTexture("Resources/Stone.png"));
        
        this->seed = seed;
        this->perlin = siv::PerlinNoise(seed);
    }

    void TerrainGenerator::onStart(){
        
    }

    UnorderedMap<glm::vec3, Entity*, Vec3Hash, Vec3Hash> chunkMap;

    int radius = 12;
    glm::ivec3 lastChunkOffset = glm::ivec3(2<<30, 2<<30, 2<<30);
    void TerrainGenerator::onUpdate(){
        auto playerTransform = entity->getScene()->getEntityByComponent<Movement>()->getComponent<Transform>();
        glm::ivec3 chunkOffset = glm::ivec3(playerTransform.position / glm::vec3(chunkSize));
        if(chunkOffset != lastChunkOffset){
            lastChunkOffset = chunkOffset;
            
            for(int x = -radius*2; x <= radius*2; x++)
            for(int y = -radius*2; y <= radius*2; y++)
            for(int z = -radius*2; z <= radius*2; z++){
                glm::ivec3 pos = {x, y, z}; 
                pos += lastChunkOffset;

                if(glm::distance2(glm::vec3(pos) + 0.5f, playerTransform.position / glm::vec3(chunkSize)) <= radius){
                    generateTerrain(pos);
                }else{
                    if(chunkMap.find(pos) != chunkMap.end()){
                        chunkMap[pos]->destroy();
                        chunkMap.erase(pos);
                    }
                }
            }
        }
    }
    String posToStr(glm::ivec3 pos){
        return "x: " + toString(pos.x) + " y: " + toString(pos.y) + " z: " + toString(pos.z);
    }

    
    void TerrainGenerator::generateTerrain(glm::ivec3 offset) {
        if(chunkMap.find(offset) != chunkMap.end()){
            return;
        }

        glm::ivec3 cloudSize = chunkSize + glm::ivec3(1, 1, 1);
        uint8 fieldCloud[cloudSize.x][cloudSize.y][cloudSize.z];
        UnorderedMap<glm::vec3, uint, Vec3Hash, Vec3Hash> vertexMap;

        auto chunkEntity = entity->getScene()->createGameObject("chunk", entity);
        chunkEntity->getComponent<PetrolEngine::Transform>().position = offset * chunkSize;
        auto& mesh = chunkEntity->addComponent<PetrolEngine::Mesh>();

        chunkMap[offset] = chunkEntity;

        for(int x = 0; x < cloudSize.x; x++){
            for(int y = 0; y < cloudSize.y; y++){
                for(int z = 0; z < cloudSize.z; z++){
                    auto a = glm::vec3(glm::ivec3(x, y, z) + (offset*chunkSize));
                    uint8 v = perlin.normalizedOctave3D_01(a.x * 0.1f, a.y *0.1f, a.z * 0.1f, 8) * 255.0;

                    v += 128 - a.y;
                    v -= a.y*2;
                    //v += (a.y > 32) ? a.y / 64 : -a.y * (1./64.);

                    fieldCloud[x][y][z] = std::clamp(v, (uint8)0, (uint8)255);
                }
            }
        }

        auto& vertices = mesh.vertices; vertices.clear();
        auto& indices  = mesh.indices;  indices .clear();
        for (int x=0; x < chunkSize.x; x++) {
            for(int y=0; y < chunkSize.y; y++){
                for(int z=0; z < chunkSize.z; z++){
                    MarchingCube cube{};

                    for(int i = 0; i < 8; i++){
                        glm::ivec3 edge = glm::ivec3(x, y, z) + edgePos[i];
                        cube.edges[i] = fieldCloud[edge.x][edge.y][edge.z];
                    }
                    
                    Vector<glm::vec3> cubeVertices;

                    MarchingCubes::getCubeMesh(cube, glm::vec3(x, y, z), &cubeVertices);

                    for(auto& vertex : cubeVertices) {
                        if (vertexMap.find(vertex) == vertexMap.end()) {
                            vertexMap[vertex] = vertexMap.size();
                            mesh.vertices.push_back(vertex);
                        }

                        mesh.indices.push_back(vertexMap[vertex]);
                    }
                }
            }
        }
        mesh.meshRenderer->material = this->material;
        /*
        mesh.normals           .clear();
        mesh.textureCoordinates.clear();

        mesh.normals           .reserve(mesh.vertices.size());
        mesh.textureCoordinates.reserve(mesh.vertices.size());

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

                return {a.x, a.z};
            };

            mesh.textureCoordinates.emplace_back(proj(mesh.vertices[mesh.indices[i + 0]]));
            mesh.textureCoordinates.emplace_back(proj(mesh.vertices[mesh.indices[i + 1]]));
            mesh.textureCoordinates.emplace_back(proj(mesh.vertices[mesh.indices[i + 2]]));
        }*/

        mesh.recalculateMesh();

        //mesh->vertices->setData(vertices.data(), vertices.size() * sizeof(PetrolEngine::Vertex));
        //mesh->indices->setData(indices.data(), indices.size() * sizeof(unsigned int));
    }
}
