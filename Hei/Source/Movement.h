#pragma once

#include "Core/Aliases.h"
#include <Core/Components/Component.h>
#include <Core/Components/Camera.h>

using namespace PetrolEngine;

namespace Hei {
    class Movement: public Component {
    public:
        float walkSpeed =  2.0f;
        float  runSpeed = 10.0f;

        Movement(Camera* camera);

        Movement& operator=(const Movement&) { return *this; }

        void onStart() override {};
        void onUpdate() override;
        const InspectorTypes inspectorTypes() override {
            return InspectorTypes{
                {String("walk Speed"), Pair<InspectorType, uint>(InspectorType::Float, offsetOf(&Movement::walkSpeed))},
                {String("run Speed"), Pair<InspectorType, uint>(InspectorType::Float, offsetOf(&Movement::runSpeed))},
            };
        }
    private:
        Camera* camera;
    };
}
