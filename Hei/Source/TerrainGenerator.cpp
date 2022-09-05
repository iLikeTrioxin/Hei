#include "TerrainGenerator.h"

#include <cmath>

#include "PerlinNoise.hpp"

static const glm::vec3 edgePos[8] = {
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

namespace Hei{

    TerrainGenerator::TerrainGenerator(uint64 seed) {
        this->seed = seed;
        this->perlin = siv::PerlinNoise(seed);
    }
    void TerrainGenerator::generateTerrain(int width, int height, int depth, PetrolEngine::Mesh* mesh) {
        Vector<glm::vec3> vertices;
        Vector<PetrolEngine::Vertex> meshVertices;
        Vector<unsigned int> indices;

        const glm::vec3 f = {
                8.0f / (float) width,
                8.0f / (float) height,
                8.0f / (float) depth
        };

        for(int x = 0; x < width; x++){
            for(int y = 0; y < height; y++){
                for(int z = 0; z < depth; z++){
                    uint64 cube = 0;

                    for(int i = 0; i < 8; i++) {
                        auto a = (edgePos[i] + glm::vec3(x, y, z)) * f;
                        float v = perlin.octave3D_01(a.x, a.y, a.z, 8);
                        cube += (unsigned char) std::floor((v - a.y) * 256.0f);

                        if (i != 7) cube <<= 8;
                    }

                    MarchingCubes::getCubeMesh(cube, glm::vec3(x, y, z), &vertices);

                    for(int i = 0; i < vertices.size(); i += 3){
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
                    }
                }
            }
        }

        mesh->vertexBuffer->setData(meshVertices.data(), meshVertices.size() * sizeof(PetrolEngine::Vertex));
        mesh->indexBuffer->setData(indices.data(), indices.size() * sizeof(unsigned int));
    }
}