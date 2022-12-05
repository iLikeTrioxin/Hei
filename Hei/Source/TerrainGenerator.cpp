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
                    glm::vec3 offset = glm::vec3(glm::ivec3(i, j, k) + x) * glm::vec3(chunkSize);
                    chunks[offset] = generateChunk(offset);
                }
            }
        }
    }

    Entity* TerrainGenerator::generateChunk(glm::ivec3 offset) {
        Entity* chunk = this->parent->getScene()->createGameObject("Chunk", parent);
        chunk->getComponent<PetrolEngine::Transform>().setPosition(glm::vec3(offset) / 2.f);
        Mesh& mesh = chunk->addComponent<Mesh>();

        mesh.material = this->material;

        glm::ivec3 cloudSize = chunkSize + glm::ivec3(1, 1, 1);
        uint8* fieldCloud = new uint8[cloudSize.x * cloudSize.y * cloudSize.z];
        UnorderedMap<glm::vec3, uint, Vec3Hash, Vec3Hash> vertexMap;

        const glm::vec3 f = glm::vec3(chunkSize) * (1.f / 8.f);

        for3D(cloudSize.x, cloudSize.y, cloudSize.z) {
            auto a = glm::vec3((offset + glm::ivec3(x, y, z))) * f;
            double v = perlin.octave3D_01(a.x * 0.01f, a.y * 0.01f, a.z * 0.01f, 8);

            fieldCloud[(z * cloudSize.x * cloudSize.y) + (y * cloudSize.x) + x] = (uint8)(v * 255.0);
        }

        mesh.vertices.clear();
        mesh.indices .clear();

        for3D(chunkSize.x, chunkSize.y, chunkSize.z){
            MarchingCube cube{};

            for(int i = 0; i < 8; i++){
                glm::ivec3 edge = glm::ivec3(x,y,z) + edgePos[i];
                cube.edges[i] = fieldCloud[(edge.z * cloudSize.x * cloudSize.y) + (edge.y * cloudSize.x) + edge.x];
            }

            Vector<glm::vec3> cubeVertices;
            MarchingCubes::getCubeMesh(cube, glm::vec3(x,y,z), &cubeVertices);

            for(auto& vertex : cubeVertices) {
                if (vertexMap.find(vertex) == vertexMap.end()) {
                    vertexMap[vertex] = vertexMap.size();
                    mesh.vertices.push_back( {vertex} );
                }

                mesh.indices.push_back(vertexMap[vertex]);
            }

        }

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
        }

        mesh.recalculateMesh();

        // mesh.vertexBuffer->setData(vertices.data(), vertices.size() * sizeof(CMVertex    ));
        // mesh. indexBuffer->setData( indices.data(),  indices.size() * sizeof(unsigned int));

        return chunk;
    }
}