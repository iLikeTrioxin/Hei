#include "GameClient.h"
#include "Chunk.h"

#include <Core/Aliases.h>
#include <Core/EventStack.h>
#include <Core/GameObject.h>

#include <Core/Components/Entity.h>

#include <Static/Renderer/defaultShaders.h>
#include <Static/Renderer/Renderer.h>
#include <Static/Window/Window.h>

#include <Core/Physics/Collider.h>
#include <Bullet/src/Bullet.h>
#include <OpenAL/Sound/Sound.h>
#include <OpenGL/OpenGL.h>
#include <GLFW/GLFW.h>
//#include <Linus/Linus.h>
#include <Assimp/modelLoader.h>
#include <ImGui/ImGui.h>

#include "Core/Components/Properties.h"
#include "Core/Components/Transform.h"
#include "Core/DebugTools.h"
#include "Events.h"
#include "Movement.h"
#include "TerrainGenerator.h"
//#include "TerrainRenderer.h"
#include "imgui.h"
#include <cmath>
#include <HeiAttributes.h>
using namespace PetrolEngine;

namespace Hei {

    GameClient::GameClient(String username, const char* ip, int port): Client(ip, port), username(username) {
        Renderer::setContext(OpenGL);
        Window  ::setContext(GLFW);
    
        Window  ::init(1280, 720, "Hei"); // 800 600
        Renderer::init(true);
        Sound   ::init();
        Text    ::init();
        DEBUG_STATEMENT(ImGuiLayer::init());
        Window::setIcon("Resources/fuel_distributor64.png");
    
        Image::flipImages(true);
        Text::loadFont("Resources/Fonts/xd.ttf");
        //Text::loadFont("Arial")
    
        Window::setVSync(false);
    
        String basicVertexShader   = ReadFile("Resources/Shaders/shader.vert");
        String basicFragmentShader = ReadFile("Resources/Shaders/shader.frag");
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
    
        this->worlds["Overworld"] = new World();
        world = this->worlds["Overworld"];
    
        player.entity = worlds["Overworld"]->createGameObject("player");
        player.camera = worlds["Overworld"]->createGameObject("Camera", player.entity);
    
        auto a = ModelLoader::loadModel("Resources/Models/Devildom girl/girl.fbx", worlds["Overworld"]);
    
        auto& kc = a->getComponent<Transform>();
        kc.position.x -= 10;
        
        gameLoop();
    }

    bool cursor = true;

    void GameClient::gameLoop() { LOG_FUNCTION();
        init();
        auto& camTra = player.camera->getComponent<Transform>();
        auto& camera = player.camera->addComponent<  Camera >();
    
        auto& camMov = player.entity->addComponent<Movement>(&camera);
        BulletCreator a;
        player.entity->addComponent<PhysicsController3D>(&a);
        // bpx
        //Image* stoneImg = Image::create("Resources/Wood.png");
        Ref<Texture> stoneTex = Renderer::createTexture("Resources/Wood.png");
        auto  box    = worlds["Overworld"]->createGameObject("falling");
        box->getComponent<Transform>().position = glm::vec3(0, 100, 0);
        box->addComponent<BoxCollider3D>(1, false); //true
        auto& boxMesh = box->addComponent<Mesh>(createCube());
        boxMesh.meshRenderer->material.shader = Renderer::loadShader("default");
        boxMesh.meshRenderer->material.textures.push_back(stoneTex);
    
        Renderer::setCamera(&camera);
    
        double previousXCursorPos = cursorXPos;
        double previousYCursorPos = cursorYPos;
        double previousFrame      = glfwGetTime();
    
        Texture* texture = Renderer::newTexture(1280, 720, TextureFormat::RGB8);
        Texture* textureD = Renderer::newTexture(1280, 720, TextureFormat::DEPTH24STENCIL8);
        Ref<Framebuffer> framebuffer = Renderer::createFramebuffer(FramebufferSpecification{1280, 720});
        framebuffer->addAttachment(texture);
        framebuffer->addAttachment(textureD);
    
        // terrain
        auto terrain = worlds["Overworld"]->createGameObject("terrain");

        //auto& generator = terrain->addComponent<TerrainGenerator>(2137);

        // skybox
        auto skybox = worlds["Overworld"]->createGameObject("skybox");
        auto& skyMesh = skybox->addComponent<Mesh>(createCube());
    
        skyMesh.invertFaces();
        skyMesh.recalculateMesh();
    
        auto skyboxTexture = Renderer::createTexture(0, 0, TextureFormat::RGBA8, TextureType::TextureCube);
        skyboxTexture->hejka("Resources/FS000_Night_02_Moonless.png");
    
        skyMesh.meshRenderer->material.textures.push_back(skyboxTexture);;
        skyMesh.meshRenderer->material.shader = Renderer::loadShader("skybox",
            ReadFile("Resources/Shaders/skybox.vert"),
            ReadFile("Resources/Shaders/skybox.frag")
        );
        GLuint RBO = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getID());
        glBindTexture(GL_TEXTURE_2D, texture->getID());
        glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
        for(auto& world : worlds) world.second->start();
    //    Benchmarker benchmarker = Benchmarker();
        double avg = 0;
        int frame = 0;
        //world->terrain = new TerrainGenerator(2137);
        //client.generateTerrain({0,0,0}, 2);
        world->serialize();
        while (!Window::shouldClose()) { LOG_SCOPE("Frame");
            //client.dispatchEvents();
    
            deltaXMousePos     = cursorXPos - previousXCursorPos;
            deltaYMousePos     = cursorYPos - previousYCursorPos;
            previousXCursorPos = cursorXPos;
            previousYCursorPos = cursorYPos;
    
            double currentFrame = glfwGetTime();
    
            deltaTime     = currentFrame - previousFrame;
            previousFrame = currentFrame;
                    
            Renderer::clear();
            update(); //gameclient
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            DEBUG_STATEMENT(ImGuiLayer::clear());
            DEBUG_STATEMENT(ImGuiLayer::drawEntityList());
            DEBUG_STATEMENT(ImGuiLayer::drawInspector());
            DEBUG_STATEMENT(ImGuiLayer::drawTexture(texture, RBO));
            DEBUG_STATEMENT(ImGuiLayer::draw());
            DEBUG_STATEMENT(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getID()));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if( Window::isPressed(Keys::KeyEscape) )
                Window::close();
     
            if (Window::isPressed(Keys::KeyR)) {
                auto shader = Renderer::loadShader("default");
    
                auto shad1 = ReadFile("Resources/Shaders/shader.vert");
                auto shad2 = ReadFile("Resources/Shaders/shader.frag");
    
                shader->recomplie(shad1, shad2);
            }
    
            for(auto* w : EventStack::getEvents<WindowApi::KeyPressedEvent>()) { LOG_SCOPE("Key pressed events");
                if(w->key == Keys::KeyM && !w->repeat) Sound::playSound("Resources/marcin.wav");
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
    
            for (auto* w : EventStack::getEvents<WindowApi::WindowResizedEvent>()) {
                camera.resolution = glm::vec2(w->data.width, w->data.height);
                camera.updatePerspective();
                Renderer::setViewport(0, 0, w->data.width, w->data.height);
                //Renderer::setViewport(w->data.width, w->data.height);
    
                EventStack::popFront<WindowApi::WindowResizedEvent>();
            }
    
            //camMov.update(mainCamera);
    
            for (auto& world : worlds) world.second->update();
            
            { LOG_SCOPE("D-F-R");
                Transform tran = Transform(
                    {10.f, 475.f, 0.f},
                    {0.1f, 0.1f, 0.1f}
                );
                avg += deltaTime;
                frame += 1;
                if(frame >= 40){
                    frame -= 20;
                    avg /= 2;
                }
                tran.position = glm::vec3(0, 0, 0);
                Renderer::renderText(".1% LOW FPS: " + toString(1.0/(avg/frame)), tran, "Resources/Fonts/xd.ttf");
            }
    
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
            //Renderer::renderFramebuffer(framebuffer);
            auto tmpTra = camTra.getRelativeTransform();
            movePlayer(tmpTra);
            EventStack::clear();
            Renderer::draw();
            Window::swapBuffers();
            Window::pollEvents ();
    //        benchmarker.frameDone();
        }
        
        DEBUG_STATEMENT(ImGuiLayer::destroy());
        Sound::destroy();
        Text ::destroy();
    }

    UnorderedMap<String, GameObject*> players;

    void GameClient::onConnect(){
        String cmd = "wj;";
        LOG("Connected to server!", 3);
        cmd += this->username + ";";
        send(cmd);
    }

    void GameClient::dispatchEvents(){
        auto& toSend = EventStack::getEvents<RequestNetwork>();

        for(auto& event : toSend) send(event->cmd);

        auto& events = EventStack::getEvents<PlayerMovedNetwork>();

        for(auto& event : events) movePlayer(*event->player);
    }

    void GameClient::movePlayer(Transform& t){
        String cmd = "pm;" + this->username + ";";

        cmd += toString(t.position.x) + ";";
        cmd += toString(t.position.y) + ";";
        cmd += toString(t.position.z) + ";";

        auto& rotation = t.rotation;

        cmd += toString(rotation.x) + ";";
        cmd += toString(rotation.y) + ";";
        cmd += toString(rotation.z) + ";";
        cmd += toString(rotation.w) + ";";

        send(cmd);
    }

    void GameClient::generateTerrain(glm::vec3 pos, int radius){
        /*radius += 1;
        std::cout<<"chunk requested!\n";

        Vector<Pair<float, glm::ivec3>> vec;
        
        auto& chunkSize = world->terrain->chunkSize;
        for(int ix = -radius; ix <= radius; ix++){
        for(int iy = -radius; iy <= radius; iy++){
        for(int iz = -radius; iz <= radius; iz++){
            float distance = std::sqrt(ix*ix + iy*iy + iz*iz);
            if(distance > radius) continue;
            vec.emplace_back(distance, glm::ivec3(ix,iy,iz));
        }}}

        std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });
        
        String cmd;
        for(auto& e : vec) {
            cmd = "gc;";

            cmd += toString(pos.x) + ";";
            cmd += toString(pos.y) + ";";
            cmd += toString(pos.z) + ";";

            cmd += toString(e.second.x) + ";";
            cmd += toString(e.second.y) + ";";
            cmd += toString(e.second.z) + ";";
            
            send(cmd);
        }*/
    }

    void GameClient::onRecive(String& msg){
        Vector<String> parts = split(msg, ';');

        if(parts[0] == "np"){
            String name = parts[1];
            
            if(this->username == name) return;

            // Image* stoneImg = Image::create("Resources/Wood.png");
            Ref<Texture> stoneTex = Renderer::createTexture("Resources/Wood.png");
    
            players[name] = world->createGameObject(name.c_str());
            auto& mesh = players[name]->addComponent<Mesh>(createCube());
            mesh.meshRenderer->material.shader = Renderer::loadShader("default");
            mesh.meshRenderer->material.textures.push_back(stoneTex);
        }

        if(parts[0] == "pm"){
            String name = parts[1];
        
            if(this->username == name) return;
            if(players.find(name) == players.end()) return;

            auto& transform = players[name]->getComponent<Transform>();
            
            glm::vec3 pos;
            auto&     rot = transform.rotation;
            
            pos.x = stof(parts[2]);
            pos.y = stof(parts[3]);
            pos.z = stof(parts[4]);
            
            transform.setPosition(pos);

            rot.x = stof(parts[5]);
            rot.y = stof(parts[6]);
            rot.z = stof(parts[7]);
            rot.w = stof(parts[8]);
        
            transform.updateOrientation();
            transform.updateTransformMatrix();
        }

        if(parts[0] == "gcr"){/*
            //String& world = parts[1];
            
            LOG(msg, 3);
            int offsetX = stoi(parts[1]);
            int offsetY = stoi(parts[2]);
            int offsetZ = stoi(parts[3]);
            
            auto& chunkSize = this->world->terrain->chunkSize;
            ChunkData*data=new ChunkData(chunkSize, glm::ivec3(offsetX, offsetY, offsetZ));

            int i = 0;

            for(int ix=0;ix< chunkSize.x; ix++){
                for(int iy = 0; iy < chunkSize.y; iy++){
                    for(int iz = 0; iz < chunkSize.z; iz++){
                        data->blocks[ix][iy][iz] = BlockData(parts[4][i++] - '0');
                    }
                }
            }
            std::cout<<"chunk recived!\n";
            worlds["Overworld"]->terrain->renderChunk(data, {offsetX, offsetY, offsetZ});
            //EventStack::addEvent(new GotChunkNetwork(data));
        */}
    }
}
