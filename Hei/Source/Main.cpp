#define PETROL_ENGINE_DEBUG
#define DEBUG

#include <PCH.h>
#include "TerrainGenerator.h"
#include <Core/Files.h>
#include <modelLoader.h>
#include <DebugTools.h>

#include "Renderer/OpenGL/OpenGL.h"

#include "Renderer/Renderer/Renderer.h"
#include "Renderer/Renderer/Shader.h"
#include "Renderer/Renderer/Texture.h"
#include "Renderer/Renderer/VertexArray.h"
#include "Renderer/Renderer/Text.h"

#include "Core/GLFW/GLFW.h"

#include "Core/Window/Window.h"

#include "Components/Material.h"
#include "Components/Movement.h"
#include "Components/TestRotationScript.h"

#include <Scene.h>
#include "Components/Entity.h"
#include <Utils/Benchmarker.h>
#include <Sound/Sound.h>
#include <Renderer/OpenGL/OpenGL.h>
#include "Core/GLFW/GLFW.h"

using namespace PetrolEngine;

class Game {
public:
    Vector<Scene> scenes;

    Game();

    void gameLoop();
};

class Player {
public:
    Entity* entity;
    Entity* camera;
};
Player player;

Game::Game() {
    Renderer::setContext(OpenGL);
    Window  ::setContext(GLFW);

    Window  ::init(800, 600, "Hei");
    Renderer::init(true);
    Sound   ::init();
    Text    ::init();

    Window::setIcon("../Hei/Resources/fuel_distributor64.png");

    Image::flipImages(true);

    Text::loadFont("../Hei/Resources/Fonts/xd.ttf");
    //Text::loadFont("Arial")

    Window::setVSync(false);

    String basicVertexShader   = ReadFile("../Hei/Resources/Shaders/shader.vert");
    String basicFragmentShader = ReadFile("../Hei/Resources/Shaders/shader.frag");
    Ref<ShaderI> basic = Shader::load("default", basicVertexShader.c_str(), basicFragmentShader.c_str(), nullptr);

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

    player.entity = scene.createGameObject("player");
    player.camera = scene.createGameObject("Camera", player.entity);

    auto a = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", &scene);
    auto c = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", &scene);
    auto b = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", &scene);

    auto& kc = a->getComponent<Transform>();
    auto& ka = b->getComponent<Transform>();
    ka.position.z -= 10;
    kc.position.x -= 10;

    ka.rotation = glm::quat(glm::radians(glm::vec3(0.0f, 180.0f, 0.0f)));

    b->addComponent<TestRotationScript>();

    gameLoop();
}
#include <Renderer/Renderer/Framebuffer.h>

void Game::gameLoop() { LOG_FUNCTION();
    uint   iter = 0;
    double time = 0;

    auto& camTra = player.camera->getComponent<Transform>();
    auto& camera = player.camera->addComponent<  Camera >();

    auto& camMov = player.entity->addComponent<Movement>(&camera);

    Renderer::setCamera(&camera);

    double previousXCursorPos = cursorXPos;
    double previousYCursorPos = cursorYPos;
    double previousFrame      = glfwGetTime();

    Ref<TextureI> texture = Texture::create(800, 600, TextureFormat::RGBA8);
    Ref<FramebufferI> framebuffer = Framebuffer::create(FramebufferSpecification{ 800, 600});
    //framebuffer->addAttachment(texture);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getID());

    for(auto& scene : scenes) scene.start();
    bool cursor = true;

    auto terrain = scenes[0].createGameObject("terrain");

    String terrainVertex   = ReadFile("../Hei/Resources/Shaders/terrain.vert");
    String terrainFragment = ReadFile("../Hei/Resources/Shaders/terrain.frag");
    Ref<ShaderI> terrainShader = Shader::load("terrain", terrainVertex.c_str(), terrainFragment.c_str(), nullptr);

    auto terrainGenerator = Hei::TerrainGenerator(213, terrain);
    terrainGenerator.setMaterial(Material(terrainShader));
    terrainGenerator.generateTerrainAround(glm::vec3(0, 0, 0), 2);

    Benchmarker benchmarker = Benchmarker();
    while (!Window::shouldClose())
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if( Window::isPressed(GLFW_KEY_ESCAPE) )
            Window::close();

        if (Window::isPressed(GLFW_KEY_R)) {
            auto shad1 = ReadFile("../Hei/Resources/Shaders/shader.vert");
            auto shad2 = ReadFile("../Hei/Resources/Shaders/shader.frag");

            auto a = Shader::load("default");
            a->recompileShader(shad1.c_str(), shad2.c_str() );

            auto shad11 = ReadFile("../Hei/Resources/Shaders/textShader.vert");
            auto shad12 = ReadFile("../Hei/Resources/Shaders/textShader.frag");

            auto b = Shader::load("textShader");
            b->recompileShader(shad11.c_str(), shad12.c_str() );
        }

        auto& kr = EventStack::getEvents<WindowI::KeyPressedEvent>();
        for (auto* w : kr) {
            if(w->key == GLFW_KEY_M && !w->repeat) Sound::playSound("/home/samuel/Downloads/marcin.wav");
            if(w->key == GLFW_KEY_C && !w->repeat) Window::showCursor(cursor = !cursor);
            if(w->key == GLFW_KEY_P && !w->repeat) {
                {
                    glm::quat r = camTra.rotation;
                    LOG("X : " + toString(r.x) + " Y : " + toString(r.y) + " Z : " + toString(r.z) + " W : " + toString(r.w), 1);
                }
                glm::vec3 r = glm::degrees(glm::eulerAngles(camTra.rotation));
                LOG("X : " + toString(r.x) + " Y : " + toString(r.y) + " Z : " + toString(r.z), 1);
            }
            EventStack::popFront<WindowI::KeyPressedEvent>();
        }

        auto& wr = EventStack::getEvents<WindowI::WindowResizedEvent>();
        for (auto* w : wr) {
            camera.resolution = glm::vec2(w->data.width, w->data.height);
            camera.updatePerspective();
            Renderer::setViewport(0, 0, w->data.width, w->data.height);
            //Renderer::setViewport(w->data.width, w->data.height);

            EventStack::popFront<WindowI::WindowResizedEvent>();
        }

        //camMov.update(mainCamera);

        for (auto& scene : scenes) scene.update();

        {
            LOG_SCOPE("D-F-R");

            Transform a = Transform();

            //a.position = { 10.f, 475.f, 0.f };

            a.scale = { 1.8f, 1.8f, 1.8f };

            Renderer::renderText(".1% LOW FPS: " + toString(1.0 / deltaTime), a, "arial");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Renderer::renderFramebuffer(framebuffer);

        EventStack::clear();

        Window::swapBuffers();
        Window::pollEvents ();
        benchmarker.frameDone();
    }

    Sound::destroy();
    Text ::destroy();
}


int main() {
    Game a = Game();
    return 0;
}