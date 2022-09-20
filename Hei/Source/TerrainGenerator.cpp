#include "TerrainGenerator.h"

#include <cmath>

#include "PerlinNoise.hpp"

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
        this->seed = seed;
        this->perlin = siv::PerlinNoise(seed);
    }

    void TerrainGenerator::generateTerrain(glm::ivec3 dimensions, glm::ivec3 offset, PetrolEngine::Mesh* mesh) {
        uint8 chunk[dimensions.x][dimensions.y][dimensions.z];
        UnorderedMap<glm::vec3, uint, Vec3Hash, Vec3Hash> vertexMap;

        const glm::vec3 f = glm::vec3(dimensions) * (1.f / 8.f);

        glm::ivec3 pos = {0, 0, 0};
        for(; pos.x < dimensions.x; pos.x++){
            for(; pos.y < dimensions.y; pos.y++){
                for(; pos.z < dimensions.z; pos.z++){

                    auto a = glm::vec3(pos + offset) * f;
                    double v = perlin.octave3D_01(a.x, a.y, a.z, 8);
                    //v += 5.0f;
                    v += a.y * a.y;
                    v = fmax(v, 0.0f);
                    chunk[pos.x][pos.y][pos.z] = (uint8)(v * 255.0);


                }
            }
        }

        Vector<glm::vec3> vertices;
        Vector<     uint> indices;
        pos = {0, 0, 0};
        for (; pos.x < dimensions.x; pos.x++) {
            for(; pos.y < dimensions.y; pos.y++){
                for(; pos.z < dimensions.z; pos.z++){
                    MarchingCube cube{};

                    for(int i = 0; i < 8; i++){
                        glm::ivec3 edge = pos + edgePos[i];
                        cube.edges[i] = chunk[edge.x][edge.y][edge.z];
                    }

                    MarchingCubes::getCubeMesh(cube, glm::vec3(pos), &vertices);

                    for(int i = 0; i < vertices.size(); i++){
                        if(vertexMap.find(vertices[i]) == vertexMap.end())
                            vertexMap[vertices[i]] = vertexMap.size();

                        vertices.push_back(vertices[i]);
                        indices.push_back(vertexMap[vertices[i]]);


                        /*
                        PetrolEngine::Vertex vertex1;
                        PetrolEngine::Vertex vertex2;
                        PetrolEngine::Vertex vertex3;

                        glm::vec3 normal = CalcSurfaceNormal(vertices[i], vertices[i + 1], vertices[i + 2]);

                        vertex1.position = vertices[i    ] * 5.0f;
                        vertex2.position = vertices[i + 1] * 5.0f;
                        vertex3.position = vertices[i + 2] * 5.0f;

                        vertex1.normal = normal;
                        vertex2.normal = normal;
                        vertex3.normal = normal;

                        meshVertices.push_back(vertex1);
                        meshVertices.push_back(vertex2);
                        meshVertices.push_back(vertex3);

                        indices.push_back(indices.size());
                        indices.push_back(indices.size());
                        indices.push_back(indices.size());
                        */
                    }
                }
            }
        }

        mesh->vertexBuffer->setData(meshVertices.data(), meshVertices.size() * sizeof(PetrolEngine::Vertex));
        mesh->indexBuffer->setData(indices.data(), indices.size() * sizeof(unsigned int));
    }
}