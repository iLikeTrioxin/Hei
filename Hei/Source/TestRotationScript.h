#pragma once

#include "Static/Window/Window.h"

#include "Core/Components/Component.h"
#include "Core/Components/Transform.h"

namespace PetrolEngine {
    class TestRotationScript : public Component {
    public:
        void onUpdate() override {
            transform->rotateY( (float) deltaTime );
        }
    };
}