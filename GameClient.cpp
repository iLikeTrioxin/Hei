#include "GameClient.h"
#include "Chunk.h"

#include <Core/Aliases.h>
#include <Core/Components/Mesh.h>
#include <Core/EventStack.h>
#include <Static/Renderer/Renderer.h>
#include <Core/Components/Entity.h>
#include <Core/Physics/Collider.h>

#include "Events.h"
#include "Movement.h"
#include <cmath>

using namespace PetrolEngine;

namespace Hei {

    GameClient::GameClient(String username, const char* ip, int port): Client(ip, port), username(username) {
        Renderer::setContext(OpenGL);
        Window  ::setContext(GLFW);
    
        Window  ::init(800, 600, "Hei");
        Renderer::init(true);
        Sound   ::init();
        Text    ::init();
        
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

    void GameClient::gameLoop() { LOG_FUNCTION();
        auto& camTra = player.camera->getComponent<Transform>();
        auto& camera = player.camera->addComponent<  Camera >();
    
        auto& camMov = player.entity->addComponent<Movement>(&camera);
        
        // bpx
        //volatile auto xax = BoxCollider(1, false);
        Image* stoneImg = Image::create("Resources/Wood.png");
        Ref<Texture> stoneTex = Renderer::createTexture(stoneImg);
        auto  box    = worlds["Overworld"]->createGameObject("falling");
        box->getComponent<Transform>().position = glm::vec3(0, 100, 0);
        box->addComponent<int>(1); //true
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
        auto terrain = worlds["Overworld"]->createGameObject("terrain");
        // skybox
    
        auto skybox = worlds["Overworld"]->createGameObject("skybox");
        auto& skyMesh = skybox->addComponent<Mesh>(createCube());
    
        skyMesh.invertFaces();
        skyMesh.recalculateMesh();
    
        auto skyboxTexture = Renderer::createTexture(0, 0, TextureFormat::RGBA8, TextureType::TextureCube);
        skyboxTexture->hejka("Resources/FS000_Night_02_Moonless.png");
    
        skyMesh.material.textures.push_back(skyboxTexture);;
        skyMesh.material.shader = Renderer::loadShader("skybox",
            ReadFile("Resources/Shaders/skybox.vert"),
            ReadFile("Resources/Shaders/skybox.frag")
        );
    
        auto terraina = worlds["Overworld"]->createGameObject("terrain1");
    
        Image* tex = Image::create("Resources/Stone.png");
    
        Material material;
    
        auto frag = ReadFile("Resources/Shaders/terrain.frag");
        auto vert = ReadFile("Resources/Shaders/terrain.vert");
        material.shader = Renderer::loadShader("terrain", vert, frag);
        material.textures.push_back(Renderer::createTexture(tex));
    
        bool gotChunk = false;
        for(auto& scene : scenes) scene.second->start();
    //    Benchmarker benchmarker = Benchmarker();
    
        glm::ivec3 lastChunkOffset = glm::ivec3(0,0,0);
        auto& playerTransform = player.entity->getComponent<Transform>();
        //client.generateTerrain({0,0,0}, 2);
        while (!Window::shouldClose()) {
            //client.update();
            //client.dispatchEvents();
    
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
    
    
            glm::ivec3 chunkOffset = glm::ivec3(playerTransform.position / 16.0f);
            if(chunkOffset != lastChunkOffset){
                lastChunkOffset = chunkOffset;
                std::cout<<"eeee jooo"<<std::endl;
                //client.generateTerrain(lastChunkOffset, 2);
            }
            
            if (Window::isPressed(Keys::KeyR)) {
                auto shader = Renderer::loadShader("default");
    
                auto shad1 = ReadFile("Resources/Shaders/shader.vert");
                auto shad2 = ReadFile("Resources/Shaders/shader.frag");
    
                shader->recomplie(shad1, shad2);
            }
    
            auto& kr = EventStack::getEvents<WindowApi::KeyPressedEvent>();
            for (auto* w : kr) {
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
    
            auto& wr = EventStack::getEvents<WindowApi::WindowResizedEvent>();
            for (auto* w : wr) {
                camera.resolution = glm::vec2(w->data.width, w->data.height);
                camera.updatePerspective();
                Renderer::setViewport(0, 0, w->data.width, w->data.height);
                //Renderer::setViewport(w->data.width, w->data.height);
    
                EventStack::popFront<WindowApi::WindowResizedEvent>();
            }
    
            //camMov.update(mainCamera);
    
            for (auto& world : worlds) world.second->update();
    
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
            movePlayer(tmpTra);
            EventStack::clear();
    
            Window::swapBuffers();
            Window::pollEvents ();
    //        benchmarker.frameDone();
        }
    
        Sound::destroy();
        Text ::destroy();
    }

    UnorderedMap<String, GameObject*> players;

    void GameClient::onConnect(){
        String cmd = "wj;";
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
        radius += 1;

        Vector<Pair<float, glm::ivec3>> vec;
        
        auto& chunkSize = terrain->chunkSize;
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
        }
    }

    void GameClient::onRecive(String& msg){
        Vector<String> parts = split(msg, ';');

        if(parts[0] == "np"){
            String name = parts[1];
            
            if(this->username == name) return;

            Image* stoneImg = Image::create("Resources/Wood.png");
            Ref<Texture> stoneTex = Renderer::createTexture(stoneImg);
    
            players[name] = world->terrain->parent->getScene()->createGameObject(name.c_str());
            auto& mesh = players[name]->addComponent<Mesh>(createCube());
            mesh.material.shader = Renderer::loadShader("default");
            mesh.material.textures.push_back(stoneTex);
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

        if(parts[0] == "gcr"){
            String& world = parts[1];

            int offsetX = stoi(parts[2]);
            int offsetY = stoi(parts[3]);
            int offsetZ = stoi(parts[4]);
            
            auto& chunkSize = world->chunkSize;
            ChunkData*data=new ChunkData(chunkSize, glm::ivec3(offsetX, offsetY, offsetZ));

            int i = 0;

            for(int ix=0;ix< chunkSize.x; ix++){
                for(int iy = 0; iy < chunkSize.y; iy++){
                    for(int iz = 0; iz < chunkSize.z; iz++){
                        data->blocks[ix][iy][iz] = BlockData(parts[4][i++] - '0');
                    }
                }
            }
            worlds[world]->terrain.renderChunk(data, glm::ivec3 center);
            //EventStack::addEvent(new GotChunkNetwork(data));
        }
    }
}
