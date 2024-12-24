#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

struct Texture
{
    SDL_GPUTexture* handle{ nullptr };
    uint32_t        width{ 0 };
    uint32_t        height{ 0 };
};

class Engine
{
public:
    void Initialize();
    void Destroy();
    void Update();

    auto CreateShader(SDL_GPUShaderCreateInfo const& info) const -> SDL_GPUShader*;
    auto CreateGraphicsPipeline(SDL_GPUGraphicsPipelineCreateInfo const& info) const -> SDL_GPUGraphicsPipeline*;

    auto AcquireCmdBuf() -> SDL_GPUCommandBuffer*;
    void SubmitCmdBuf(SDL_GPUCommandBuffer* cmd);
    auto AcquireSwapchainImage(SDL_GPUCommandBuffer* cmd) -> Texture const&;
// private:
    struct Window
    {
        SDL_Window* handle{ nullptr };
        bool b_fullscreen{ false };
        bool b_minimalize{ false };
    } m_window;
    
    struct RHI
    {
        bool debug_mode{ true };
        SDL_GPUDevice* device{ nullptr };

        Texture present_texture{};
    } m_rhi;
};