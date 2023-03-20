#include "Core/Aliases.h"
#include "Core/Components/Mesh.h"
#include "Core/Components/Transform.h"
#include "Core/Renderer/Shader.h"
#include "Static/Window/Window.h"
#include <cstring>
#include <string>
#define PETROL_ENGINE_DEBUG
#define DEBUG

#include <Core/Files.h>
#include <Assimp/modelLoader.h>
#include <Core/DebugTools.h>

#include "OpenGL/OpenGL.h"
#include <Bullet.h>
#include "Freetype/Renderer/Text.h"

#include <GLFW/GLFW.h>

#include "TestRotationScript.h"
#include <Core/Components/Material.h>
#include <Core/Components/Movement.h>
#include <Core/Components/Entity.h>
#include <Core/Renderer/Renderer.h>
#include <Core/Window/Window.h>
#include <Core/Scene.h>

#include "TerrainGenerator.h"
#include <OpenAL/Sound/Sound.h>

#include <Static/Renderer/defaultShaders.h>
#include <Static/Renderer/Renderer.h>

#include <thread>
#include <ENet/Server.h>
#include <ENet/Client.h>

#include "GameClient.h"
#include "GameServer.h"

using namespace PetrolEngine;

class Game {
public:
    const char* ip;
    int port;

    UnorderedMap<String, Scene*> scenes;
    
    Game(const char* ip, int port);

    void gameLoop();
};

class Player {
public:
    Entity* entity;
    Entity* camera;
};

Player player;

Game::Game(const char* ip, int port) {
    this->ip = ip;
    this->port = port;

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
    Ref<Shader> basic = Renderer::loadShader("default", defaultVertexShader, defaultFragmentShader);

    basic->setInt  ("material.diffuse"  , 0  );
    basic->setInt  ("material.specular" , 0  );
    basic->setFloat("material.shininess", 1. );
    basic->setInt  ("light[0].lightType", 1  );

    basic->setVec3 ("light[0].direction", 1.0f, 0.0f, 1.0f);
    basic->setVec3 ("light[0].ambient"  , 0.2f, 0.2f, 0.2f);
    basic->setVec3 ("light[0].diffuse"  , 3.0f, 3.0f, 3.0f);
    basic->setVec3 ("light[0].specular" , 0.0f, 0.0f, 0.0f);

    ModelLoader::Get().shader = basic;

    this->scenes["Overworld"] = new Scene();

    player.entity = scenes["Overworld"]->createGameObject("player");
    player.camera = scenes["Overworld"]->createGameObject("Camera", player.entity);

//auto a = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", scenes["Overworld"]);
    auto a = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", scenes["Overworld"]);
    auto c = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", scenes["Overworld"]);
    auto b = ModelLoader::loadModel("../Hei/Resources/Models/Devildom girl/girl.fbx", scenes["Overworld"]);

    auto& kc = a->getComponent<Transform>();
    auto& ka = b->getComponent<Transform>();
    ka.position.z -= 10;
    kc.position.x -= 10;

    ka.rotation = glm::quat(glm::radians(glm::vec3(0.0f, 180.0f, 0.0f)));

    b->addComponent<TestRotationScript>();

    gameLoop();
    //gameServer.join();
}

void Game::gameLoop() { LOG_FUNCTION();
    auto& camTra = player.camera->getComponent<Transform>();
    auto& camera = player.camera->addComponent<  Camera >();

    auto& camMov = player.entity->addComponent<Movement>(&camera);
    
    // bpx
    Image* stoneImg = Image::create("../Hei/Resources/Wood.png");
    Ref<Texture> stoneTex = Renderer::createTexture(stoneImg);
    auto  box    = scenes["Overworld"]->createGameObject("falling");
    box->getComponent<Transform>().position = glm::vec3(0, 100, 0);
    box->addComponent<BoxCollider>(1, false); //true
    auto& boxMesh = box->addComponent<Mesh>(createCube());
    boxMesh.material.shader = Renderer::loadShader("default");
    boxMesh.material.textures.push_back(stoneTex);

    Renderer::setCamera(&camera);

    double previousXCursorPos = cursorXPos;
    double previousYCursorPos = cursorYPos;
    double previousFrame      = glfwGetTime();

    Ref<Texture> texture = Renderer::createTexture(800, 600, TextureFormat::RGBA8);
//    Ref<Framebuffer> framebuffer = Renderer::createFramebuffer(FramebufferSpecification{800, 600});
    //framebuffer->addAttachment(texture);
//    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getID());

    bool cursor = true;

    // terrain
    auto terrain = scenes["Overworld"]->createGameObject("terrain");
/*
    Image* tex = Image::create("../Hei/Resources/Stone.png");

    Material material;

    auto frag = ReadFile("../Hei/Resources/Shaders/terrain.frag");
    auto vert = ReadFile("../Hei/Resources/Shaders/terrain.vert");
    material.shader = Renderer::loadShader("terrain", vert, frag);
    material.textures.push_back(Renderer::createTexture(tex));

    delete tex;

    auto terrainGenerator = Hei::TerrainGenerator(213, terrain);
    terrainGenerator.setMaterial(material);
    terrainGenerator.generateTerrainAround(glm::vec3(0, 0, 0), 2);
*/
    // skybox

    auto skybox = scenes["Overworld"]->createGameObject("skybox");
    auto& skyMesh = skybox->addComponent<Mesh>(createCube());

    skyMesh.invertFaces();
    skyMesh.recalculateMesh();

    auto skyboxTexture = Renderer::createTexture(0, 0, TextureFormat::RGBA8, TextureType::TextureCube);
    skyboxTexture->hejka("../Hei/Resources/FS000_Night_02_Moonless.png");

    skyMesh.material.textures.push_back(skyboxTexture);;
    skyMesh.material.shader = Renderer::loadShader("skybox",
        ReadFile("../Hei/Resources/Shaders/skybox.vert"),
        ReadFile("../Hei/Resources/Shaders/skybox.frag")
    );

    auto terraina = scenes["Overworld"]->createGameObject("terrain1");

    Image* tex = Image::create("../Hei/Resources/Stone.png");

    Material material;

    auto frag = ReadFile("../Hei/Resources/Shaders/terrain.frag");
    auto vert = ReadFile("../Hei/Resources/Shaders/terrain.vert");
    material.shader = Renderer::loadShader("terrain", vert, frag);
    material.textures.push_back(Renderer::createTexture(tex));


    //Hei::GameLoader anie = Hei::GameLoader(terraina, material);
    //anie.load("xd");
    //anie.generateAround(glm::vec3(0,0,0), 2);
    String username;
    std::cin>>username;
    GameClient client = GameClient(username, terrain, material, this->ip, this->port);
    client.init();
    bool gotChunk = false;
    for(auto& scene : scenes) scene.second->start();
//    Benchmarker benchmarker = Benchmarker();
    /*
    String adac;

            for(int ix = -2; ix <= 2; ix++){
                for(int iy = -2; iy <= 2; iy++){
                    for(int iz = -2; iz <= 2; iz++){
                        adac = "gc;";
                        adac += toString(ix) + ";";
                        adac += toString(iy) + ";";
                        adac += toString(iz) + ";";
                        client.send(ada);
                    }
                }
            }*/
    client.generateTerrain({0,0,0}, 2);
    while (!Window::shouldClose()) {
        client.update();

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

        if( Window::isPressed(Keys::KeyEscape) )
            Window::close();


        if(gotChunk == false &&  Window::isPressed(Keys::KeyH)){
            client.generateTerrain({0,0,0}, 3);
            /*String ada;

            for(int ix = -2; ix <= 2; ix++){
                for(int iy = -2; iy <= 2; iy++){
                    for(int iz = -2; iz <= 2; iz++){
                        ada = "gc;";
                        ada += toString(ix) + ";";
                        ada += toString(iy) + ";";
                        ada += toString(iz) + ";";
                        client.send(ada);
                    }
                }
            }*/


            gotChunk = true;
        }
        if (Window::isPressed(Keys::KeyR)) {
            auto shader = Renderer::loadShader("default");

            auto shad1 = ReadFile("../Hei/Resources/Shaders/shader.vert");
            auto shad2 = ReadFile("../Hei/Resources/Shaders/shader.frag");

            shader->recomplie(shad1, shad2);
        }

        auto& kr = EventStack::getEvents<WindowApi::KeyPressedEvent>();
        for (auto* w : kr) {
            if(w->key == Keys::KeyM && !w->repeat) Sound::playSound("/home/samuel/Downloads/marcin.wav");
            if(w->key == Keys::KeyC && !w->repeat) Window::showCursor(cursor = !cursor);
            if(w->key == Keys::KeyP && !w->repeat) {
                {
                    glm::quat r = camTra.rotation;
                    LOG("X : " + toString(r.x) + " Y : " + toString(r.y) + " Z : " + toString(r.z) + " W : " + toString(r.w), 1);
                }
                glm::vec3 r = glm::degrees(glm::eulerAngles(camTra.rotation));
                LOG("X : " + toString(r.x) + " Y : " + toString(r.y) + " Z : " + toString(r.z), 1);
            }
            EventStack::popFront<WindowApi::KeyPressedEvent>();
        }

        auto& wr = EventStack::getEvents<WindowApi::WindowResizedEvent>();
        for (auto* w : wr) {
            camera.resolution = glm::vec2(w->data.width, w->data.height);
            camera.updatePerspective();
            Renderer::setViewport(0, 0, w->data.width, w->data.height);
            //Renderer::setViewport(w->data.width, w->data.height);

            EventStack::popFront<WindowApi::WindowResizedEvent>();
        }

        //camMov.update(mainCamera);

        for (auto& scene : scenes) scene.second->update();

        { LOG_SCOPE("D-F-R");
//            Transform tran = Transform(
//                {10.f, 475.f, 0.f},
//                {.1f, .1f, .1f}
//            );
//
//            tran.position = glm::vec3(0, 0, 0);
//            Renderer::renderText(".1% LOW FPS: " + toString(1.0 / deltaTime), tran, "../Hei/Resources/Fonts/xd.ttf");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Renderer::renderFramebuffer(framebuffer);
        auto tmpTra = camTra.getRelativeTransform();
        client.movePlayer(tmpTra);
        EventStack::clear();

        Window::swapBuffers();
        Window::pollEvents ();
//        benchmarker.frameDone();
    }

    Sound::destroy();
    Text ::destroy();
}
//#define OnlyServer
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

#ifndef OnlyServer
    Game a = Game(ip, port);
#endif

    if(strcmp(ip, "localhost") == 0)
        serverThread->join();

    delete serverThread;

    return 0;
}
