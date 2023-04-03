#include "../Server/GameServer.h"
#include <cstring>
#include <iostream>
#include <thread>

using namespace PetrolEngine;

int main(int argc, char* argv[]) {
    const char* ip = "localhost";
    int port = 55555;

    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "--ip"  ) == 0 && argc > i) ip = argv[i+1];
        if(strcmp(argv[i], "--port") == 0 && argc > i) port = std::stoi(argv[i+1]);
    }

    std::cout<< "ip " << ip << " port " << port << std::endl;

    auto serverFunc = [](int port){
        auto server = GameServer(port);
        server.init();
        server.run();
    };

    std::thread* serverThread = nullptr;

    if(strcmp(ip, "localhost") == 0)
        serverThread = new std::thread(serverFunc, port);

    if(strcmp(ip, "localhost") == 0)
        serverThread->join();

    delete serverThread;

    return 0;
}
