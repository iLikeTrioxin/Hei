#include "Game.h"

#include <Static/Renderer/defaultShaders.h>
#include <Static/Renderer/Renderer.h>
#include <Static/Window/Window.h>

#include <Assimp/modelLoader.h>

#include <OpenGL/OpenGL.h>

#include <GLFW/GLFW.h>

#include <OpenAL/Sound/Sound.h>
#include "Movement.h"

#include "Events.h"

namespace Hei {
    Game::Game(){
        std::string username = "iLikeTrioxin";
        //std::cout<<"Username: ";
        //std::cin>>username;
        std::cout<<std::endl;
        this->game = new GameClient(username, "localhost", 55555);
    }

    Game::~Game(){
        delete this->game;
    }
}
