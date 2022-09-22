#include "TerrainGenerator.h"

#include <cmath>
#include "Scene.h"
#include "PerlinNoise.hpp"
#include "Components/Mesh.h"

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

namespace Hei {

    TerrainGenerator::TerrainGenerator(uint64 seed, PetrolEngine::Entity* parent) {
        this->parent = parent;
        this->seed = seed;
        this->perlin = siv::PerlinNoise(seed);
    }

    void TerrainGenerator::generateTerrainAround(glm::vec3 position, int radius) {
        glm::ivec3 x = glm::ivec3(position) / chunkSize;
        for (int i = -radius; i < radius; i++) {
            for (int j = -radius; j < radius; j++) {
                for (int k = -radius; k < radius; k++) {
                    glm::vec3 offset = glm::vec3(i, j, k) * glm::vec3(chunkSize);
                    offset += x;
                    chunks[offset] = generateChunk(offset);
                }
            }
        }
    }

    Entity* TerrainGenerator::generateChunk(glm::ivec3 offset) {
        using PetrolEngine::Mesh;
        using PetrolEngine::VertexLayout;

        struct CMVertex {
            glm::vec3 position;
            //glm::vec2 texCords;
            //glm::vec3 normal;
        };

        Entity* chunk = this->parent->getScene()->createGameObject("Chunk", parent);
        chunk->getComponent<PetrolEngine::Transform>().setPosition(glm::vec3(offset));
        Mesh& mesh = chunk->addComponent<Mesh>(VertexLayout({
            { "position", PetrolEngine::ShaderDataType::Float3},
            //{ "normal"  , PetrolEngine::ShaderDataType::Float3 },
        }));

        mesh.material = this->material;

        auto* fieldCloud = new uint8[chunkSize.x * chunkSize.y * chunkSize.z];
        UnorderedMap<glm::vec3, uint, Vec3Hash, Vec3Hash> vertexMap;

        const glm::vec3 f = glm::vec3(chunkSize) * (1.f / 8.f);

        for3D(chunkSize.x, chunkSize.y, chunkSize.z){
            auto a = glm::vec3((offset + glm::ivec3(x, y, z))) * f;
            double v = perlin.octave3D_01(a.x, a.y, a.z, 8);
            fieldCloud[(z * chunkSize.x * chunkSize.y) + (y * chunkSize.x) + x] = (uint8)(v * 255.0);
        }

        Vector<CMVertex> vertices;
        Vector<    uint> indices;

        for3D(chunkSize.x, chunkSize.y, chunkSize.z){
            MarchingCube cube{};

            for(int i = 0; i < 8; i++){
                glm::ivec3 edge = glm::ivec3(x,y,z) + edgePos[i];
                cube.edges[i] = fieldCloud[(edge.z * chunkSize.x * chunkSize.y) + (edge.y * chunkSize.x) + edge.x];
            }

            Vector<glm::vec3> v;
            MarchingCubes::getCubeMesh(cube, glm::vec3(x,y,z), &v);

            for(int i = 0; i < v.size(); i++) {
                if (vertexMap.find(v[i]) == vertexMap.end()) {
                    vertexMap[v[i]] = vertexMap.size();
                    vertices.push_back( {v[i]} );
                }

                indices.push_back(vertexMap[v[i]]);
            }

            for(int i = 0; i < indices.size(); i += 3) {
                glm::vec3 normal = CalcSurfaceNormal(
                    vertices[indices[i + 0]].position,
                    vertices[indices[i + 1]].position,
                    vertices[indices[i + 2]].position
                );

                //vertices[indices[i + 0]].normal = normal;
                //vertices[indices[i + 1]].normal = normal;
                //vertices[indices[i + 2]].normal = normal;
            }
        }

        mesh.vertexBuffer->setData(vertices.data(), vertices.size() * sizeof(CMVertex    ));
        mesh. indexBuffer->setData( indices.data(),  indices.size() * sizeof(unsigned int));

        return chunk;
    }
}