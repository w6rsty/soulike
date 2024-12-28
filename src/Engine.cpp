#include "Engine.hpp"
#include "ResourceManager.hpp"
#include "SDL3/SDL_gpu.h"

void Engine::Initialize()
{
    SDL_SetAppMetadata("soulike", "0.1", "com.w6rsty.soulike");
    SDL_Init(SDL_INIT_VIDEO);

    /// Create window
    m_window.handle = SDL_CreateWindow("window", 800, 600, SDL_WINDOW_RESIZABLE);

    /// Create GPU device
    m_rhi.device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, m_rhi.debug_mode, nullptr);
    SDL_ClaimWindowForGPUDevice(m_rhi.device, m_window.handle);

    ResourceManager::Initialize("/Users/w6rsty/dev/Cpp/soulike/config", m_rhi.device);
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

auto Engine::UploadModel(SDL_GPUCopyPass* pass, std::string const& name) -> ModelInfo const&
{
    auto& mgr = ResourceManager::Instance();
    auto const& model = mgr.GetModel(name);
    
    uint32_t offset{ 0 };
    for (auto const& mesh : model.meshes)
    {
        for (auto const& buffer : mesh.buffers)
        {
            SDL_GPUTransferBufferLocation location{
                .transfer_buffer = model.transfer_buffer,
                .offset = offset,
            };
            SDL_GPUBufferRegion region{
                .buffer = buffer.first,
                .offset = 0,
                .size = buffer.second,
            };
            SDL_UploadToGPUBuffer(pass, &location, &region, false);
            offset += buffer.second;
        }
    }
    mgr.SetModelStatus(name, true);
    return model;
}

void Engine::DrawMesh(SDL_GPURenderPass* pass, MeshInfo const& mesh)
{
    std::vector<SDL_GPUBufferBinding> vertex_bindings(mesh.buffers.size() - 1);
    for (size_t i{ 0 }; i < vertex_bindings.size(); ++i)
    {
        vertex_bindings[i].buffer = mesh.buffers[i].first;
        vertex_bindings[i].offset = 0;
    }
    SDL_BindGPUVertexBuffers(pass, 0, vertex_bindings.data(), static_cast<uint32_t>(vertex_bindings.size()));
    SDL_GPUBufferBinding index_binding{ mesh.buffers.back().first, 0 };
    SDL_BindGPUIndexBuffer(pass, &index_binding, mesh.index_type);
    SDL_DrawGPUIndexedPrimitives(pass, static_cast<uint32_t>(mesh.index_count), 1, 0, 0, 0);
}

void Engine::DrawModel(SDL_GPURenderPass* pass, ModelInfo const& model)
{
    if (!model.active)
    {
        return;
    }
    for (auto const& mesh : model.meshes)
    {
        DrawMesh(pass, mesh);
    }
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