#pragma once

#include <Core/Event.h>
#include <Core/Components/Transform.h>
#include "Chunk.h"

using namespace PetrolEngine;

namespace Hei {
    struct PlayerMovedNetwork: Event { Transform* player; PlayerMovedNetwork(Transform* p): player(p){ } };
    struct     RequestNetwork: Event { String cmd; RequestNetwork(String s): cmd(s){ } };
    struct    GotChunkNetwork: Event { ChunkData* data; GotChunkNetwork(ChunkData* d): data(d){ } };
}
