#include "GameClient.h"
#include "Chunk.h"

#include <Core/Aliases.h>
#include <Core/Components/Mesh.h>
#include <Core/EventStack.h>
#include <Static/Renderer/Renderer.h>
#include <Core/Components/Entity.h>

#include "Events.h"
#include "Movement.h"
#include <cmath>

using namespace PetrolEngine;

namespace Hei {
    /*
    UnorderedMap<String, Entity*> players;

    void GameClient::onConnect(){
        String cmd = "wj;";
        cmd += this->name + ";";
        send(cmd);
    }

    void GameClient::dispatchEvents(){
        auto& toSend = EventStack::getEvents<RequestNetwork>();

        for(auto& event : toSend) send(event->cmd);

        auto& events = EventStack::getEvents<PlayerMovedNetwork>();

        for(auto& event : events) movePlayer(*event->player);
    }

    void GameClient::movePlayer(Transform& t){
        String cmd = "pm;" + this->name + ";";

        cmd += toString(t.position.x) + ";";
        cmd += toString(t.position.y) + ";";
        cmd += toString(t.position.z) + ";";

        auto& rotation = t.rotation;

        cmd += toString(rotation.x) + ";";
        cmd += toString(rotation.y) + ";";
        cmd += toString(rotation.z) + ";";
        cmd += toString(rotation.w) + ";";

        send(cmd);
    }

    void GameClient::generateTerrain(glm::vec3 pos, int radius){
        radius += 1;

        Vector<Pair<float, glm::ivec3>> vec;
        
        auto& chunkSize = terrainManager->chunkSize;
        for(int ix = -radius; ix <= radius; ix++){
        for(int iy = -radius; iy <= radius; iy++){
        for(int iz = -radius; iz <= radius; iz++){
            float distance = std::sqrt(ix*ix + iy*iy + iz*iz);
            if(distance > radius) continue;
            vec.emplace_back(distance, glm::ivec3(ix,iy,iz));
        }}}

        std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });
        
        String cmd;
        for(auto& e : vec) {
            cmd = "gc;";

            cmd += toString(pos.x) + ";";
            cmd += toString(pos.y) + ";";
            cmd += toString(pos.z) + ";";

            cmd += toString(e.second.x) + ";";
            cmd += toString(e.second.y) + ";";
            cmd += toString(e.second.z) + ";";
            
            send(cmd);
        }
    }

    void GameClient::onRecive(String& msg){
        Vector<String> parts = split(msg, ';');

        if(parts[0] == "np"){
            String name = parts[1];
            
            if(this->name == name) return;

            Image* stoneImg = Image::create("Resources/Wood.png");
            Ref<Texture> stoneTex = Renderer::createTexture(stoneImg);
    
            players[name] = terrainManager->parent->getScene()->createGameObject(name.c_str());
            auto& mesh = players[name]->addComponent<Mesh>(createCube());
            mesh.material.shader = Renderer::loadShader("default");
            mesh.material.textures.push_back(stoneTex);
        }

        if(parts[0] == "pm"){
            String name = parts[1];
        
            if(this->name == name) return;
            if(players.find(name) == players.end()) return;

            auto& transform = players[name]->getComponent<Transform>();
            
            glm::vec3 pos;
            auto&     rot = transform.rotation;
            
            pos.x = stof(parts[2]);
            pos.y = stof(parts[3]);
            pos.z = stof(parts[4]);
            
            transform.setPosition(pos);

            rot.x = stof(parts[5]);
            rot.y = stof(parts[6]);
            rot.z = stof(parts[7]);
            rot.w = stof(parts[8]);
        
            transform.updateOrientation();
            transform.updateTransformMatrix();
        }

        if(parts[0] == "gcr"){
            int offsetX = stoi(parts[1]);
            int offsetY = stoi(parts[2]);
            int offsetZ = stoi(parts[3]);
            
            auto& chunkSize = terrainManager->chunkSize;
            ChunkData*data=new ChunkData(chunkSize, glm::ivec3(offsetX, offsetY, offsetZ));

            int i = 0;

            for(int ix=0;ix< chunkSize.x; ix++){
                for(int iy = 0; iy < chunkSize.y; iy++){
                    for(int iz = 0; iz < chunkSize.z; iz++){
                        data->blocks[ix][iy][iz] = BlockData(parts[4][i++] - '0');
                    }
                }
            }
            
            EventStack::addEvent(new GotChunkNetwork(data));
        }
    }
*/
}
