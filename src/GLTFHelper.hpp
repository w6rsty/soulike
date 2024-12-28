#pragma once
#include <vector>
#include <filesystem>
#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <SDL3/SDL_gpu.h>

auto VertexElementSize(SDL_GPUVertexElementFormat format) -> uint32_t;
auto IndexElementSize(SDL_GPUIndexElementSize size) -> uint32_t;

class GLTFHelper
{
    struct BufferAttribute
    {
        unsigned char const*  data_section;
        size_t                byte_size;
        size_t                byte_offset;
    };

    struct MeshDescription
    {
        std::vector<BufferAttribute> attributes;
        size_t                       index_count;
        SDL_GPUIndexElementSize      index_type;
    };
public:
    auto Load(std::filesystem::path const& path) -> std::pair<uint32_t, std::vector<MeshDescription> const&>;
    void Clear();
private:
    void LoadNode(tinygltf::Node const& node);
    void LoadMesh(tinygltf::Mesh const& mesh);
private:
    tinygltf::Model m_model;
    std::vector<MeshDescription> m_meshes;
    size_t m_total_size{ 0 };
};