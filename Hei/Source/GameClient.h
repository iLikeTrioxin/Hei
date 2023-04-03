#pragma once

#include <ENet/Client.h>
#include <Core/Aliases.h>
#include "Core/Components/Entity.h"
#include "TerrainRenderer.h"

using namespace PetrolEngine;

namespace Hei {
/*
    class GameClient: public Client {
    public:
        String version;
        String name;
        TerrainManager* terrainManager = nullptr;
        //Entity* parent;
        GameClient(String name, Entity* parent, Material material, const char* ip, int port): Client(ip, port) {
            terrainManager = new TerrainManager(parent, material);
            this->name = name;
        }
        
        void generateTerrain(glm::vec3 pos, int radius);
        
        void dispatchEvents();
        void movePlayer(Transform& t);
        void onConnect();
        void onRecive(String& msg);
    };
    */
}
