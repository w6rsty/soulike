#include <cassert>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine.hpp"
#include "Logger.hpp"
#include "CameraController.hpp"

struct alignas(16) CBuffer
{
    float time;
    float padding0;
    glm::vec2 resolution;
    glm::mat4 projection;
    glm::mat4 view;
};

static CBuffer cbuffer = {
    .time = 0.0f,
    .resolution = glm::vec2(800.0f, 600.0f),
    .projection = glm::mat4(1.0f),
    .view = glm::mat4(1.0f),
};

int main(int argc, char *argv[])
{
    Logger::Initialize();

    Engine engine;
    engine.Initialize();
    cbuffer.resolution = glm::vec2(800.0f, 600.0f);
    
    auto& mgr = ResourceManager::Instance();
    auto pipeline = mgr.GetPipeline("default");
    assert(pipeline);

    SDL_GPUCommandBuffer* copy_cmd = engine.AcquireCmdBuf();
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(copy_cmd);
    auto const& bunny = engine.UploadModel(copy_pass, "bunny");
    SDL_EndGPUCopyPass(copy_pass);
    engine.SubmitCmdBuf(copy_cmd);

    Camera camera;
    camera.SetPerspectiveParams(glm::radians(30.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    CameraController controller(&camera);
    controller.SetMoveSpeed(0.1f);
    controller.SetRotateSpeed(0.1f);
    cbuffer.projection = camera.GetProjectionMatrix();
    
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_W:
                    controller.MoveForward(true);
                    break;
                case SDLK_S:
                    controller.MoveBackward(true);
                    break;
                case SDLK_A:
                    controller.MoveLeft(true);
                    break;
                case SDLK_D:
                    controller.MoveRight(true);
                    break;
                case SDLK_SPACE:
                    controller.EnableRotating(true);
                    break;
                }
                break;
            case SDL_EVENT_KEY_UP:
                switch (event.key.key)
                {
                case SDLK_W:
                    controller.MoveForward(false);
                    break;
                case SDLK_S:
                    controller.MoveBackward(false);
                    break;
                case SDLK_A:
                    controller.MoveLeft(false);
                    break;
                case SDLK_D:
                    controller.MoveRight(false);
                    break;
                case SDLK_SPACE:
                    controller.EnableRotating(false);
                    break;
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                controller.RotateYaw(event.motion.xrel);
                controller.RotatePitch(event.motion.yrel);
                break;
            }
        }

        static uint64_t last_tick{ 0 };
        controller.Update((SDL_GetTicks() - last_tick) / 100.0f);
        last_tick = SDL_GetTicks();


        SDL_GPUCommandBuffer* cmd = engine.AcquireCmdBuf();
    
        Texture const& present_texture = engine.AcquireSwapchainImage(cmd);

        std::vector<SDL_GPUColorTargetInfo> color_targets{
            {
                .texture = present_texture.handle,
                .mip_level = 0,
                .layer_or_depth_plane = 0,
                .clear_color = {0.2f, 0.2f, 0.2f, 1.0f},
                .load_op = SDL_GPULoadOp::SDL_GPU_LOADOP_CLEAR,
                .store_op = SDL_GPUStoreOp::SDL_GPU_STOREOP_STORE,
                .resolve_mip_level = 0,
                .resolve_layer = 0,
                .cycle = false,
                .cycle_resolve_texture = false,
            }
        };

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(
            cmd,
            color_targets.data(), 
            static_cast<uint32_t>(color_targets.size()), 
            nullptr);
        
        SDL_GPUViewport viewport {
            .x = 0, .y = 0,
            .w = static_cast<float>(present_texture.width), .h = static_cast<float>(present_texture.height),
        };
        SDL_SetGPUViewport(render_pass, &viewport);
        SDL_Rect scissor {
            .x = 0, .y = 0,
            .w = static_cast<int>(present_texture.width), .h = static_cast<int>(present_texture.height),
        };
        SDL_SetGPUScissor(render_pass, &scissor);

        SDL_BindGPUGraphicsPipeline(render_pass, pipeline);

        cbuffer.time = SDL_GetTicks() / 1000.0f;
        cbuffer.view = camera.GetViewMatrix();
        SDL_PushGPUVertexUniformData(cmd, 0, &cbuffer, sizeof(CBuffer));
        // SDL_PushGPUFragmentUniformData(cmd, 0, &ubo, sizeof(UBO));
        engine.DrawModel(render_pass, bunny);
        SDL_EndGPURenderPass(render_pass);

        engine.SubmitCmdBuf(cmd);
    }

    engine.Destroy();
    Logger::Destroy();
}
