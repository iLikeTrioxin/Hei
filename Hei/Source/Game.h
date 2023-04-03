#pragma once

#include <ENet/Client.h>
#include "Core/Components/Transform.h"
#include "TerrainRenderer.h"

using namespace PetrolEngine;

namespace Hei{
    class World {
    public:
        TerrainManager* terrain = nullptr;
        Scene*          scene   = nullptr;
        
        void update();
    };
    
    class Player {
    public:
        GameObject* entity;
        GameObject* camera;
    };

    class GameClient: public Client {
    public:
        const  String version = "0.0.1";
        static String username;
        static Player player;
    
    public:
        static UnorderedMap<String, World*> worlds;
        static World* world;
        
                

        Game(String username, const char* ip, int port);
    
        void onConnect();
        void onRecive(String& msg);
        void dispatchEvents(); 
        void generateTerrain(glm::vec3 pos, int radius);
        void movePlayer(Transform& tran);
        void gameLoop();
    };
    
}
