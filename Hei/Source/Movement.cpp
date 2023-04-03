#include <PCH.h>

#include <Core/EventStack.h>
#include "Events.h"
#include "Movement.h"
#include <Core/Components/Camera.h>
#include <Static/Window/Window.h>

using namespace PetrolEngine;

namespace Hei {
    // Movement
    Movement::Movement(Camera* camera) :
            camera (camera) {};

    void Movement::onUpdate() { LOG_FUNCTION();
        float distance = (Window::isPressed(Keys::KeyLeftShift) ? runSpeed : walkSpeed) * (float)deltaTime;

        if (Window::isPressed(Keys::KeyW)) transform->position += camera->transform->forward * distance;
        if (Window::isPressed(Keys::KeyS)) transform->position -= camera->transform->forward * distance;
        if (Window::isPressed(Keys::KeyA)) transform->position -= camera->transform->right   * distance;
        if (Window::isPressed(Keys::KeyD)) transform->position += camera->transform->right   * distance;

                transform->rotateY((float) -(deltaXMousePos * 0.005));
        camera->transform->rotateX((float) -(deltaYMousePos * 0.005));

        glm::vec3 angles = eulerAngles(camera->transform->rotation);

        if (angles.x >  1.5f) camera->transform->setRotationX( 1.5f);
        if (angles.x < -1.5f) camera->transform->setRotationX(-1.5f);

        EventStack::addEvent(new PlayerMovedNetwork(transform));
    }
}
