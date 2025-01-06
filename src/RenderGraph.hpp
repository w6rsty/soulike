#pragma once 
#include <vector>
#include <string>
#include <SDL3/SDL_gpu.h>

struct RenderPass
{
    std::string name;
    std::vector<SDL_GPUColorTargetInfo> render_targets;
    SDL_GPUGraphicsPipeline* pipeline;
    SDL_GPUViewport viewport;
    SDL_Rect scissor;
};

RenderPass CreateRenderPass(
    std::string const& name,
    std::vector<SDL_GPUColorTargetInfo> const& render_targets,
    SDL_GPUGraphicsPipeline* pipeline,
    SDL_GPUViewport const& viewport,
    SDL_Rect const& scissor)
{
    return RenderPass{
        .name = name,
        .render_targets = render_targets,
        .pipeline = pipeline,
        .viewport = viewport,
        .scissor = scissor,
    };
}

struct RenderGraph
{
    std::vector<RenderPass> passes;
};
