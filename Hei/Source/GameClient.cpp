#include "GameClient.h"
#include "Chunk.h"
#include "Core/Aliases.h"
#include "Core/Components/Mesh.h"
#include "Static/Renderer/Renderer.h"
#include <Core/Components/Entity.h>
#include <cmath>

namespace PetrolEngine{
    UnorderedMap<String, Entity*> players;

    void GameClient::onConnect(){
        String cmd = "wj;";
        cmd += this->name + ";";
        send(cmd);
    }

    void GameClient::movePlayer(Transform& t){
        String cmd = "pm;";
        cmd += this->name + ";";

        cmd += toString(t.position.x) + ";";
        cmd += toString(t.position.y) + ";";
        cmd += toString(t.position.z) + ";";

        auto rotation = t.getRotation();

        cmd += toString(rotation.x) + ";";
        cmd += toString(rotation.y) + ";";
        cmd += toString(rotation.z) + ";";

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

            cmd += toString(e.second.x) + ";";
            cmd += toString(e.second.y) + ";";
            cmd += toString(e.second.z) + ";";
            std::cout<<cmd<<std::endl;
            send(cmd);
        }
    }

    void GameClient::onRecive(String& msg){
        Vector<String> parts = split(msg, ';');
        
        if(parts[0] == "npi"){
             
        }

        if(parts[0] == "np"){
            String name = parts[1];
            
            if(this->name == name) return;

            Image* stoneImg = Image::create("../Hei/Resources/Wood.png");
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

            glm::vec3 pos;
            glm::vec3 rot;

            pos.x = stof(parts[2]);
            pos.y = stof(parts[3]);
            pos.z = stof(parts[4]);

            rot.x = stof(parts[5]);
            rot.y = stof(parts[6]);
            rot.z = stof(parts[7]);
            
            auto& transform = players[name]->getComponent<Transform>();
            
            transform.setPosition(pos);
            transform.setRotation(rot);
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

            terrainManager->renderChunk(data);
        }
    }
}
