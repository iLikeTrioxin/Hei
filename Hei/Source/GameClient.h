#pragma once

#include <ENet/Client.h>
#include <Core/Aliases.h>
#include "Core/Components/Entity.h"
#include "TerrainRenderer.h"

using namespace PetrolEngine;

namespace Hei {
    class World: public Scene {
    public:
        TerrainManager* terrain = nullptr;
        
    };
    
    class Player {
    public:
        GameObject* entity;
        GameObject* camera;
    };

    class GameClient: public Client {
    public:
        const String version = "0.0.1";
        String username;
        Player player;
    
    public:
        UnorderedMap<String, World*> worlds;
        World* world;
        
        GameClient(String username, const char* ip, int port);
    
        void onConnect();
        void onRecive(String& msg);
        void dispatchEvents(); 
        void generateTerrain(glm::vec3 pos, int radius);
        void movePlayer(Transform& tran);
        void gameLoop();
    };
}
