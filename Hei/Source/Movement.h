#pragma once

#include "Core/Aliases.h"
#include "Core/Components/InspectorTypes.h"
#include <Core/Components/Component.h>
#include <Core/Components/Camera.h>

using namespace PetrolEngine;

namespace Hei {
    class Movement: public Component {
    public:
        [[Expose]] float walkSpeed =  2.0f;
        [[Expose]] float  runSpeed = 10.0f;
        [[Expose]] Vector<Vector<int>> jes;

        Movement(Camera* camera);

        Movement& operator=(const Movement&) { return *this; }

        void onStart() override {};
        void onUpdate() override;
    private:
        Camera* camera;
    };
}
