#pragma once

#include "Core/Aliases.h"
#include "../TerrainGenerator.h"
#include "../TerrainRenderer.h"
#include <ENet/Server.h>
#include "../Chunk.h"

namespace Hei {
    class GameServer: public Server {
    public:
        struct Player {
            glm::vec3 position;
            glm::quat rotation;
        };
    public:
        Entity* worldParent = nullptr; // TODO: remove it i think?
        Hei::GameLoader* world;
        UnorderedMap<String, Player> players;
        
        GameServer(int port): PetrolEngine::Server(port) {
            world = new Hei::GameLoader();
        }
        
        virtual void onRecive(Peer& client, String& msg){
            Vector<String> parts = split(msg, ';');
                LOG(msg, 3); 
    
            if(parts[0] == "wj"){
                String resp = "np;"; // new player
                resp += parts[1] + ";"; // username
                
                this->broadcast(resp);

                for(auto& player : players){
                    resp = "np;";
                    resp += player.first + ";";
                    client.send(resp);
                }

                players[parts[1]] = Player();
            }

            if(parts[0] == "pm"){
                this->broadcast(msg);
            }
    
            if(parts[0] == "gc"){ 
                int cx = stoi(parts[1]);
                int cy = stoi(parts[2]);
                int cz = stoi(parts[3]);
                
                int x = stoi(parts[4]);
                int y = stoi(parts[5]);
                int z = stoi(parts[6]);

                String resp = "gcr;";
                std::cout<<"chunk request recived!;\n";

                resp += toString(x) + ";";
                resp += toString(y) + ";";
                resp += toString(z) + ";";
                
                ChunkData* data = this->world->getChunk({x,y,z});

                int xSize = data->blocks      .size();
                int ySize = data->blocks[0]   .size();
                int zSize = data->blocks[0][0].size();

                resp.reserve(xSize*ySize*zSize + 32);
                for(int ix=0;ix< data->blocks.size(); ix++){
                    for(int iy = 0; iy < data->blocks[ix].size(); iy++){
                        for(int iz = 0; iz < data->blocks[ix][iy].size(); iz++){
                            resp += toString(data->blocks[ix][iy][iz].id);
                        }
                    }
                }
                resp += ";";

                client.send(resp);
            }
        }
    };

}
