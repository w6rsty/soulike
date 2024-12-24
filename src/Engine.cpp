#include "Engine.hpp"
#include "ResourceManager.hpp"

void Engine::Initialize()
{
    SDL_SetAppMetadata("soulike", "0.1", "com.w6rsty.soulike");
    SDL_Init(SDL_INIT_VIDEO);

    /// Create window
    m_window.handle = SDL_CreateWindow("window", 800, 600, SDL_WINDOW_RESIZABLE);

    /// Create GPU device
    m_rhi.device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, m_rhi.debug_mode, nullptr);
    SDL_ClaimWindowForGPUDevice(m_rhi.device, m_window.handle);

    ResourceManager::Initialize("config", m_rhi.device);
}

void Engine::Destroy()
{
    ResourceManager::Destroy();

    if (m_window.handle)
    {
        SDL_ReleaseWindowFromGPUDevice(m_rhi.device, m_window.handle);
    }
    if (m_rhi.device) {
        SDL_DestroyGPUDevice(m_rhi.device);
    }

    SDL_DestroyWindow(m_window.handle);

    SDL_Quit();
}

void Engine::Update()
{
}
    
auto Engine::CreateShader(SDL_GPUShaderCreateInfo const& info) const -> SDL_GPUShader*
{
    return SDL_CreateGPUShader(m_rhi.device, &info);
}

auto Engine::CreateGraphicsPipeline(SDL_GPUGraphicsPipelineCreateInfo const& info) const -> SDL_GPUGraphicsPipeline*
{
    return SDL_CreateGPUGraphicsPipeline(m_rhi.device, &info);
}

auto Engine::AcquireCmdBuf() -> SDL_GPUCommandBuffer*
{
    return SDL_AcquireGPUCommandBuffer(m_rhi.device);
}

void Engine::SubmitCmdBuf(SDL_GPUCommandBuffer* cmd)
{
    SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
    SDL_WaitForGPUFences(m_rhi.device, false, &fence, 1);
    SDL_ReleaseGPUFence(m_rhi.device, fence);
}


auto Engine::AcquireSwapchainImage(SDL_GPUCommandBuffer* cmd) -> Texture const&
{
    SDL_AcquireGPUSwapchainTexture(
        cmd,
        m_window.handle,
        &m_rhi.present_texture.handle,
        &m_rhi.present_texture.width,
        &m_rhi.present_texture.height);
    return m_rhi.present_texture;
}