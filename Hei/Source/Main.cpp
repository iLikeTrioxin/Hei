#define PETROL_ENGINE_DEBUG
#include <Core/Files.h>
#include <Components.h>
#include <Renderer/Renderer.h>
#include <modelLoader.h>
#include <DebugTools.h>
#include <Renderer/Shader.h>
#include <Renderer/Text.h>
#include <Scene.h>
#include <Core/Window.h>
#include <Entity.h>
#include <GLFW/glfw3.h>
#include <Utils/Benchmarker.h>

using namespace PetrolEngine;

class Game {
public:
    std::shared_ptr<Window> window;

    Vector<Scene> scenes;

    Game();

    void gameLoop();
};

Entity mainCamera;

Game::Game() {
    DEBUG_LOG("Size of int8  is " << sizeof(int8 ));
    DEBUG_LOG("Size of int16 is " << sizeof(int16));
    DEBUG_LOG("Size of int32 is " << sizeof(int32));
    DEBUG_LOG("Size of int64 is " << sizeof(int64));

    this->window = Window::create(800, 600, "Hei");

    Image::flipImages(true);

    window->init();
    //window->setIcon(Image("../Hei/Resources/cobweb.png"));

    Renderer::init(RendererAPI::API::OpenGL);
    Text::init("../Hei/Resources/Fonts/Poppins/Poppins-Black.ttf");

    window->setVSync(false);

    auto basic = Shader::load("default",ReadFile("../Hei/Resources/Shaders/shader.vert"),ReadFile("../Hei/Resources/Shaders/shader.frag")
    );

    basic->setInt  ("material.diffuse"  , 0  );
    basic->setInt  ("material.specular" , 0  );
    basic->setFloat("material.shininess", 1. );
    basic->setInt  ("light[0].lightType", 1  );

    basic->setVec3 ("light[0].direction", 1.0f, 0.0f, 1.0f);
    basic->setVec3 ("light[0].ambient"  , 0.2f, 0.2f, 0.2f);
    basic->setVec3 ("light[0].diffuse"  , 3.0f, 3.0f, 3.0f);
    basic->setVec3 ("light[0].specular" , 0.0f, 0.0f, 0.0f);

    ModelLoader::Get().shader = basic;

    Scene& scene = scenes.emplace_back<>();

    mainCamera = scene.createEntity("Camera");

    auto a = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", &scene);
    auto b = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", &scene);

    auto& ka = b.getComponent<Transform>();
    ka.position.z -= 10;
    ka.rotation = glm::quat(glm::radians(glm::vec3(0.0f, 180.0f, 0.0f)));

    ExternalScript* s = new rotatee(&ka);
    b.addComponent<ExternalScript*>(s);

    gameLoop();
}

void Game::gameLoop() { LOG_FUNCTION();
    uint   iter = 0;
    double time = 0;

    auto& camTra = mainCamera.getComponent<Transform>(       );
    auto& camera = mainCamera.addComponent<  Camera >(       );
    auto& camMov = mainCamera.addComponent< Movement>(&camTra);

    camera.updatePerspectiveMatrix(window->getAspectRatio());

    uint idk = 0;
    Renderer::getDeviceConstantValue(DeviceConstant::MAX_TEXTURE_IMAGE_UNITS, (void*) &idk);
    DEBUG_LOG("aaa:"<<std::to_string(idk));

    double previousXCursorPos = cursorXPos;
    double previousYCursorPos = cursorYPos;
    double previousFrame      = glfwGetTime();

    Benchmarker benchmarker = Benchmarker( (void*) glfwGetTime );

    while (!window->shouldClose())
    {
        deltaXMousePos     = cursorXPos - previousXCursorPos;
        deltaYMousePos     = cursorYPos - previousYCursorPos;
        previousXCursorPos = cursorXPos;
        previousYCursorPos = cursorYPos;

        double currentFrame = glfwGetTime();

        deltaTime     = currentFrame - previousFrame;
        previousFrame = currentFrame;

        LOG_SCOPE("Frame");
        Renderer::clear();

        if( window->isPressed(GLFW_KEY_ESCAPE) )
            window->close();

        if (window->isPressed(GLFW_KEY_R)) {
            auto shad1 = ReadFile("../Hei/Resources/Shaders/shader.vert");
            auto shad2 = ReadFile("../Hei/Resources/Shaders/shader.frag");

            Shader::load("default", "")->recompileShader(shad1.c_str(), shad2.c_str() );
        }

        auto& wr = EventStack::getEvents<Window::WindowResizedEvent>();
        for (auto* w : wr) {
            camera.updatePerspectiveMatrix(window->getAspectRatio());
            Renderer::OnWindowResize(w->data.width, w->data.height);
            EventStack::popFront<Window::WindowResizedEvent>();
        }

        camMov.update(window, mainCamera);

        for (auto& scene : scenes) scene.update(window->getAspectRatio());

        // display frame rate
        {
            LOG_SCOPE("D-F-R");

            Transform a = Transform();

            a.position = { 10.f, 475.f, 0.f };
            Renderer::renderText(".1% LOW FPS: " + std::to_string(1.0 / benchmarker.getAVG()), a);
            
            a.position = { 10.f, 450.f, 0.f };
            Renderer::renderText("1.% LOW FPS: " + std::to_string(1.0 / benchmarker.getDotOnePercentLow()), a);

            a.position = { 10.f, 425.f, 0.f };
            //Renderer2D::renderText( std::to_string(1.0 / average), a, camera, .4f);
        }

        EventStack::clear();

        window->swapBuffers();
        window->pollEvents ();
        benchmarker.frameDone();
    }
}


int main() {
    Game a = Game();
    return 0;
}