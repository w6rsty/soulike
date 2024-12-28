#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "GLTFHelper.hpp"
#include "Logger.hpp"

auto VertexElementSize(SDL_GPUVertexElementFormat format) -> uint32_t
{
    switch (format)
    {
    case SDL_GPU_VERTEXELEMENTFORMAT_BYTE2:
    case SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2:
    case SDL_GPU_VERTEXELEMENTFORMAT_BYTE2_NORM:
    case SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2_NORM:
        return 2;

    case SDL_GPU_VERTEXELEMENTFORMAT_BYTE4:
    case SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4:
    case SDL_GPU_VERTEXELEMENTFORMAT_BYTE4_NORM:
    case SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM:
    case SDL_GPU_VERTEXELEMENTFORMAT_INT:
    case SDL_GPU_VERTEXELEMENTFORMAT_UINT:
    case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT:
        return 4;
    
    case SDL_GPU_VERTEXELEMENTFORMAT_SHORT2:
    case SDL_GPU_VERTEXELEMENTFORMAT_USHORT2:
    case SDL_GPU_VERTEXELEMENTFORMAT_SHORT2_NORM:
    case SDL_GPU_VERTEXELEMENTFORMAT_USHORT2_NORM:
    case SDL_GPU_VERTEXELEMENTFORMAT_HALF2:
    case SDL_GPU_VERTEXELEMENTFORMAT_INT2:
    case SDL_GPU_VERTEXELEMENTFORMAT_UINT2:
    case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2:
        return 8;

    case SDL_GPU_VERTEXELEMENTFORMAT_INT3:
    case SDL_GPU_VERTEXELEMENTFORMAT_UINT3:
    case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3:
        return 12;

    case SDL_GPU_VERTEXELEMENTFORMAT_SHORT4:
    case SDL_GPU_VERTEXELEMENTFORMAT_USHORT4:
    case SDL_GPU_VERTEXELEMENTFORMAT_SHORT4_NORM:
    case SDL_GPU_VERTEXELEMENTFORMAT_USHORT4_NORM:
    case SDL_GPU_VERTEXELEMENTFORMAT_HALF4:
    case SDL_GPU_VERTEXELEMENTFORMAT_INT4:
    case SDL_GPU_VERTEXELEMENTFORMAT_UINT4:
    case SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4:
        return 16;

    case SDL_GPU_VERTEXELEMENTFORMAT_INVALID:
    default:
        return 0;
    }
}

auto IndexElementSize(SDL_GPUIndexElementSize size) -> uint32_t
{
    switch (size)
    {
    case SDL_GPU_INDEXELEMENTSIZE_16BIT:
        return 2;
    case SDL_GPU_INDEXELEMENTSIZE_32BIT:
        return 4;
    }
}

auto GLTFHelper::Load(std::filesystem::path const& path) -> std::pair<uint32_t, std::vector<MeshDescription> const&>
{
    tinygltf::TinyGLTF loader{};
    std::string warn, error;
    bool result{ false };
    if (path.extension() == ".glb")
    {
        result = loader.LoadBinaryFromFile(&m_model, &warn, &error, path.string());
    }
    else if (path.extension() == ".gltf")
    {
        result = loader.LoadASCIIFromFile(&m_model, &warn, &error, path.string());
    }
    else
    {
        SO_ERROR("Unsupported file format: {}", path.string());
        return {0, m_meshes};
    }

    if (!warn.empty())
    {
        SO_WARN("{}", warn);
    }
    if (!error.empty())
    {
        SO_ERROR("{}", error);
    }
    if (!result)
    {
        SO_ERROR("Failed to load: {}", path.string());
    }
    else
    {
        SO_INFO("Loaded: {}", path.string());
    }

    auto const& scene = m_model.scenes[m_model.defaultScene];
    for (auto node_index : scene.nodes)
    {
        LoadNode(m_model.nodes[node_index]);
    }

    return {m_total_size, m_meshes};
}

void GLTFHelper::LoadNode(tinygltf::Node const& node)
{
    if (node.mesh >= 0)
    {
        LoadMesh(m_model.meshes[node.mesh]);
    }
    for (auto child_index : node.children)
    {
        LoadNode(m_model.nodes[child_index]);
    }
}

void GLTFHelper::LoadMesh(tinygltf::Mesh const& mesh)
{
    for (auto const& primitive : mesh.primitives)
    {
        MeshDescription mesh_info{};
        mesh_info.attributes.reserve(primitive.attributes.size() + 1);

        if (primitive.attributes.find("POSITION") != primitive.attributes.end())
        {
            int position_idx = primitive.attributes.at("POSITION");
            const auto& position_accessor = m_model.accessors[primitive.attributes.at("POSITION")];
            const auto& position_view = m_model.bufferViews[position_accessor.bufferView];

            mesh_info.attributes.emplace_back(
                BufferAttribute{
                    .data_section = m_model.buffers[position_view.buffer].data.data(),
                    .byte_size = position_view.byteLength,
                    .byte_offset = position_view.byteOffset,
                }
            );
            m_total_size += position_view.byteLength;
        }
        
        if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
        {
            int normal_idx = primitive.attributes.at("NORMAL");
            const auto& normal_accessor = m_model.accessors[primitive.attributes.at("NORMAL")];
            const auto& normal_view = m_model.bufferViews[normal_accessor.bufferView];

            mesh_info.attributes.emplace_back(
                BufferAttribute{
                    .data_section = m_model.buffers[normal_view.buffer].data.data(),
                    .byte_size = normal_view.byteLength,
                    .byte_offset = normal_accessor.byteOffset,
                }
            );
            m_total_size += normal_view.byteLength;
        }

        // index buffer
        if (primitive.indices >= 0)
        {
            const auto& index_accessor = m_model.accessors[primitive.indices];
            const auto& index_buffer_view = m_model.bufferViews[index_accessor.bufferView];

            auto index_buffer_attribute = mesh_info.attributes.emplace_back(
                BufferAttribute{
                    .data_section = m_model.buffers[index_buffer_view.buffer].data.data(),
                    .byte_size = index_buffer_view.byteLength,
                    .byte_offset = index_buffer_view.byteOffset,
                }
            );

            mesh_info.index_count = index_accessor.count;
            if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                mesh_info.index_type = SDL_GPU_INDEXELEMENTSIZE_16BIT;
            }
            else if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            {
                mesh_info.index_type = SDL_GPU_INDEXELEMENTSIZE_32BIT;
            }
            else
            {
                SO_ERROR("Invalid index component type: {}", index_accessor.componentType);
            }
            m_total_size += index_buffer_view.byteLength;
        }

        m_meshes.push_back(mesh_info);
    }
}

void GLTFHelper::Clear()
{
    // MeshInfo hold data reference from tinygltf::Model,
    // so when clearing the model, the data become invalid as well.
    m_model = {};
    m_meshes.clear();
    m_total_size = 0;
}