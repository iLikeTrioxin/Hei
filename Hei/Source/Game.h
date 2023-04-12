#pragma once

#include "GameClient.h"

using namespace PetrolEngine;

namespace Hei{
    class Game {
    public:
        GameClient* game = nullptr;

        Game();
        ~Game();
    };
}
