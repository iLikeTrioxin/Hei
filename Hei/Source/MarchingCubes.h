#pragma once

#include "Aliases.h"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "MarchingCubesConstants.h"
#include "Core/Logger.h"
namespace Hei {

    struct MarchingCube {
        uint8 edges[8];
    };

    class MarchingCubes {
    public:
        static uint8 getCubeTriangulationIndex(MarchingCube cube) {
            uint8 index = 0;

            for (uint8 i = 0; i < 8; i++)
                index |= (cube.edges[i] > 128) << i;

            return index;
        }

        static glm::vec3 interpolatePoints(glm::vec4 p1, glm::vec4 p2, float center = 128.f) {
            return p1 + (center - p1.w) / (p2.w - p1.w) * (p2 - p1);
        }

        static void getCubeMesh(MarchingCube cube, glm::vec3 coords, Vector<glm::vec3>* vertices){
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

            for(uint8 i = 0; i < 5; i++){
                if (triangulation[cubeIndex][i * 3] == -1) return;

                for (uint8 vertexNumber = 0; vertexNumber < 3; vertexNumber++) {
                    uint8 index = i * 3 + vertexNumber;

                    uint8 cornerA = cornerIndexAFromEdge[triangulation[cubeIndex][index]];
                    uint8 cornerB = cornerIndexBFromEdge[triangulation[cubeIndex][index]];

                    glm::vec3 VertexPosition = interpolatePoints(
                            glm::vec4(cubeCorners[cornerA] + coords, cube.edges[cornerA]),
                            glm::vec4(cubeCorners[cornerB] + coords, cube.edges[cornerB])
                    );

                    vertices->push_back(VertexPosition);
                }
            }

        }
    };
}