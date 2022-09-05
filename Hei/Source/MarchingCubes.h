#pragma once

#include "Aliases.h"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "MarchingCubesConstants.h"
#include "Core/Logger.h"
namespace Hei {

    class MarchingCubes {
    public:
        static uint8 getCubeTriangulationIndex(uint64 cube) {
            uint8 index = 0;

            for (uint8 i = 0; i < 8; i++)
                index |= ((getCubeIndex(cube, i) > 128) ? 1 : 0) << i;

            return index;
        }

        static glm::vec3 interpolateVerts(glm::vec4 v1, glm::vec4 v2, float t = 128) {
            return v1 + (t - v1.w) / (v2.w - v1.w) * (v2 - v1);
        }

        static inline uint8 getCubeIndex(uint64 cube, uint8 i) {
            return ((cube & (0xFF << (i*8))) >> (i*8));
        }

        static void getCubeMesh(uint64 cube, glm::vec3 coords, Vector<glm::vec3>* vertices){
            static const glm::vec3 cubeCorners[8] = {
                    {0, 0, 0},
                    {1, 0, 0},
                    {1, 0, 1},
                    {0, 0, 1},
                    {0, 1, 0},
                    {1, 1, 0},
                    {1, 1, 1},
                    {0, 1, 1}
            };

            uint8 cubeIndex = getCubeTriangulationIndex(cube);
            if (cubeIndex != 0 && cubeIndex != 255)
                LOG("cud nad wisla", 2);

            for(uint8 i = 0; i < 5; i++){
                if (triangulation[cubeIndex][i * 3] == -1) return;

                for (uint8 vertexNumber = 0; vertexNumber < 3; vertexNumber++) {
                    uint8 index = i * 3 + vertexNumber;

                    uint8 cornerA = cornerIndexAFromEdge[triangulation[cubeIndex][index]];
                    uint8 cornerB = cornerIndexBFromEdge[triangulation[cubeIndex][index]];

                    glm::vec3 VertexPosition = interpolateVerts(
                            glm::vec4(cubeCorners[cornerA] + coords, getCubeIndex(cube, cornerA)),
                            glm::vec4(cubeCorners[cornerB] + coords, getCubeIndex(cube, cornerB))
                    );

                    vertices->push_back(VertexPosition);
                }
            }

        }
    };
}