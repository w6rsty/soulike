#include <SDL3/SDL_gpu.h>
#include "ResourceManager.hpp"
#include "Logger.hpp"
#include "Script.hpp"

LuaTableScope::LuaTableScope(lua_State* L, char const* table, bool is_global, bool required)
    : m_state(L)
{
    if (is_global)
    {
        lua_getglobal(L, table);
    }
    else
    {
        lua_getfield(L, -1, table);
    }

    if (lua_istable(L, -1))
    {
        m_valid = true;
        return;
    }
    
    if (required)
    {
        SO_WARN("Table not found: {}", table);
    }
    m_valid = false;
    lua_pop(L, 1);
}

LuaTableScope::LuaTableScope(lua_State* L, int index, bool required)
    : m_state(L)
{
    lua_pushinteger(L, index);
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
    {
        m_valid = true;
        return;
    }
    
    if (required)
    {
        SO_WARN("Element not found");
        lua_pop(L, 1);
    }
    m_valid = false;
}

LuaTableScope::~LuaTableScope()
{
    if (m_valid)
    {
        lua_pop(m_state, 1);
    }
}

auto LuaTableScope::IsValid() const -> bool
{
    return m_valid;
}

auto Script::ReadArrayLength(lua_State* L) -> int
{
    return lua_rawlen(L, -1);
}

auto Script::Load(lua_State* L, std::filesystem::path const& path) -> bool
{
    if (luaL_loadfile(L, path.c_str()) || lua_pcall(L, 0, 0, 0))
    {
        SO_ERROR("Error loading script: {}", lua_tostring(L, -1));
        lua_pop(L, 1); // pop error message
        return false;
    }

    return true;
}

auto Script::ReadStringField(lua_State* L, char const* key) -> std::optional<std::string>
{
    lua_getfield(L, -1, key);
    if (!lua_isstring(L, -1)) {
        lua_pop(L, 1);
        return std::nullopt;
    }
    std::string value = lua_tostring(L, -1);
    lua_pop(L, 1);
    return value;
}

auto Script::ReadFloatingField(lua_State* L, char const* key) -> std::optional<float>
{
    lua_getfield(L, -1, key);
    if (!lua_isnumber(L, -1)) {
        lua_pop(L, 1);
        return std::nullopt;
    }
    float value = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return value;
}

auto Script::ReadIntegerField(lua_State* L, char const* key) -> std::optional<int>
{
    lua_getfield(L, -1, key);
    if (!lua_isinteger(L, -1)) {
        lua_pop(L, 1);
        return std::nullopt;
    }
    int value = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return value;
}

auto Script::ReadBooleanField(lua_State* L, char const* key) -> std::optional<bool>
{
    lua_getfield(L, -1, key);
    if (!lua_isboolean(L, -1)) {
        lua_pop(L, 1);
        return std::nullopt;
    }
    bool value = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return value;
}

auto ResourceManager::LoadShader(
    lua_State* L, 
    std::filesystem::path const& path) -> bool
{
    if (!Script::Load(L, path))
    {
        return false;
    }

    ShaderInfo shader_info{};
    {   
        LuaTableScope shader_scope(L, "shader");
        if (!shader_scope.IsValid())
        {
            return false;
        }

        shader_info.is_byte_code = Script::ReadBooleanField(L, "is_byte_code").value_or(false);
        shader_info.source_path = Script::ReadStringField(L, "source_path").value_or("");
        shader_info.stage = static_cast<SDL_GPUShaderStage>(Script::ReadIntegerField(L, "stage").value_or(0));
        shader_info.format = static_cast<uint32_t>(Script::ReadIntegerField(L, "format").value_or(0));
        shader_info.entry_point = Script::ReadStringField(L, "entry_point").value_or("main");
        shader_info.num_samplers = static_cast<uint32_t>(Script::ReadIntegerField(L, "num_samplers").value_or(0));
        shader_info.num_storage_buffers = static_cast<uint32_t>(Script::ReadIntegerField(L, "num_storage_buffers").value_or(0));
        shader_info.num_uniform_buffers = static_cast<uint32_t>(Script::ReadIntegerField(L, "num_uniform_buffers").value_or(0));
    } // shader_scope

    std::size_t code_size{ 0 };
    void* code{ nullptr };
    if (shader_info.is_byte_code)
    {
        code = SDL_LoadFile(shader_info.source_path.c_str(), &code_size);
    }
    else
    {
        std::string format_suffix{ "" };
        switch (shader_info.format)
        {
            case SDL_GPU_SHADERFORMAT_SPIRV:    
                format_suffix = "spv";
                break;
            case SDL_GPU_SHADERFORMAT_MSL:
                format_suffix = "metal";
                break;
            default:
                SO_ERROR("Unsupported shader format");
                return false;
        }

        std::filesystem::path output_path = std::format(
            "build/shaders/metal/{}.{}",
            path.stem().string(),
            format_suffix);
        std::filesystem::create_directories(output_path.parent_path());

        ResourceManager::Slangc({
            .input_file = shader_info.source_path,
            .output_file = output_path,
            .entry_point = shader_info.entry_point,
            .stage = shader_info.stage,
            .format = shader_info.format,
        });

        code = SDL_LoadFile(output_path.c_str(), &code_size);
    }

    SDL_GPUShaderCreateInfo shader_create_info{
        .code_size           = code_size,
        .code                = static_cast<uint8_t*>(code),
        .entrypoint          = shader_info.entry_point.c_str(),
        .format              = shader_info.format,
        .stage               = shader_info.stage,
        .num_samplers        = shader_info.num_samplers,
        .num_storage_buffers = shader_info.num_storage_buffers,
        .num_uniform_buffers = shader_info.num_uniform_buffers,
    };
    SDL_GPUShader* shader = SDL_CreateGPUShader(m_device, &shader_create_info);
    SDL_free(code);
    if (!shader)
    {
        SO_ERROR("Failed to create shader, {}", SDL_GetError());
        return false;
    }

    std::string shader_name = path.stem().string();
    m_shaders[ResourceManager::Hash(shader_name)] = {shader_info, shader};
    SO_INFO("Shader loaded: {}", shader_name);

    return true;
}

auto ResourceManager::LoadPipeline(
    lua_State* L, 
    std::filesystem::path const& path) -> bool
{
    if (!Script::Load(L, path))
    {
        return false;
    }

    SDL_GPUGraphicsPipelineCreateInfo info{};
    std::vector<SDL_GPUVertexBufferDescription> vertex_buffer_descriptions;
    std::vector<SDL_GPUVertexAttribute> vertex_attributes;
    std::vector<SDL_GPUColorTargetDescription> color_target_descriptions;
    
    {   
        LuaTableScope pipeline_scope(L, "pipeline");
        if (!pipeline_scope.IsValid())
        {
            return false;
        }

        std::string vertex_shader_name = Script::ReadStringField(L, "vertex_shader").value_or("");
        std::string fragment_shader_name = Script::ReadStringField(L, "fragment_shader").value_or("");
        SDL_GPUShader* vertex_shader = m_shaders[ResourceManager::Hash(vertex_shader_name)].second;
        if (!vertex_shader)
        {
            SO_ERROR("Vertex shader not found: {}", vertex_shader_name);
            return false;
        }
        info.vertex_shader = vertex_shader;
        SDL_GPUShader* fragment_shader = m_shaders[ResourceManager::Hash(fragment_shader_name)].second;
        if (!fragment_shader)
        {
            SO_ERROR("Fragment shader not found: {}", fragment_shader_name);
            return false;
        }
        info.fragment_shader = fragment_shader;

        {   
            LuaTableScope vertex_input_state_scope(L, "vertex_input_state", false, false);
            if (vertex_input_state_scope.IsValid())
            {
                {   
                    LuaTableScope vertex_buffer_descriptions_scope(L, "vertex_buffer_descriptions", false, false);
                    if (vertex_buffer_descriptions_scope.IsValid())
                    {
                        uint32_t num_vertex_buffers = Script::ReadArrayLength(L);
                        vertex_buffer_descriptions.resize(num_vertex_buffers);

                        for (uint32_t i = 0; i < num_vertex_buffers; ++i)
                        {
                            {   
                                LuaTableScope i_scope(L, i + 1);
                                if (i_scope.IsValid())
                                {
                                    SDL_GPUVertexBufferDescription& vertex_buffer_description = vertex_buffer_descriptions[i];
                                    vertex_buffer_description.slot = Script::ReadIntegerField(L, "slot").value_or(0);
                                    vertex_buffer_description.pitch = Script::ReadIntegerField(L, "pitch").value_or(0);
                                    vertex_buffer_description.input_rate = static_cast<SDL_GPUVertexInputRate>(Script::ReadIntegerField(L, "input_rate").value_or(0));
                                    vertex_buffer_description.instance_step_rate = Script::ReadIntegerField(L, "instance_step_rate").value_or(0);

                                }
                            } // i_scope
                        }
                    }
                } // vertex_buffer_descriptions_scope

                {   
                    LuaTableScope vertex_attributes_scope(L, "vertex_attributes", false, false);
                    if (vertex_attributes_scope.IsValid())
                    {
                        uint32_t num_vertex_attributes = Script::ReadArrayLength(L);
                        vertex_attributes.resize(num_vertex_attributes);
                        for (uint32_t i = 0; i < num_vertex_attributes; ++i)
                        {
                            {   
                                LuaTableScope i_scope(L, i + 1);
                                if (i_scope.IsValid())
                                {
                                    SDL_GPUVertexAttribute& vertex_attribute = vertex_attributes[i];
                                    vertex_attribute.location = Script::ReadIntegerField(L, "location").value_or(0);
                                    vertex_attribute.buffer_slot = Script::ReadIntegerField(L, "buffer_slot").value_or(0);
                                    vertex_attribute.format = static_cast<SDL_GPUVertexElementFormat>(Script::ReadIntegerField(L, "format").value_or(0));
                                    vertex_attribute.offset = Script::ReadIntegerField(L, "offset").value_or(0);
                                }
                            } // i_scope
                        }                        
                    }
                } // vertex_attributes_scope

                info.vertex_input_state = {
                    .vertex_buffer_descriptions = vertex_buffer_descriptions.data(),
                    .num_vertex_buffers = static_cast<uint32_t>(vertex_buffer_descriptions.size()),
                    .vertex_attributes = vertex_attributes.data(),
                    .num_vertex_attributes = static_cast<uint32_t>(vertex_attributes.size()),
                };
            }
        } // vertex_input_state_scope

        info.primitive_type = static_cast<SDL_GPUPrimitiveType>(Script::ReadIntegerField(L, "primitive_type").value_or(0));

        {   
            LuaTableScope rasterizer_state_scope(L, "rasterizer_state", false);
            if (!rasterizer_state_scope.IsValid())
            {
                return false;
            }

            info.rasterizer_state.fill_mode = static_cast<SDL_GPUFillMode>(Script::ReadIntegerField(L, "fill_mode").value_or(0));
            info.rasterizer_state.cull_mode = static_cast<SDL_GPUCullMode>(Script::ReadIntegerField(L, "cull_mode").value_or(0));
            info.rasterizer_state.front_face = static_cast<SDL_GPUFrontFace>(Script::ReadIntegerField(L, "front_face").value_or(0));
            info.rasterizer_state.depth_bias_constant_factor = Script::ReadFloatingField(L, "depth_bias_constant_factor").value_or(0.0f);
            info.rasterizer_state.depth_bias_clamp = Script::ReadFloatingField(L, "depth_bias_clamp").value_or(0.0f);
            info.rasterizer_state.depth_bias_slope_factor = Script::ReadFloatingField(L, "depth_bias_slope_factor").value_or(0.0f);
            info.rasterizer_state.enable_depth_bias = Script::ReadBooleanField(L, "enable_depth_bias").value_or(false);
            info.rasterizer_state.enable_depth_clip = Script::ReadBooleanField(L, "enable_depth_clip").value_or(false);
        } // rasterizer_state_scope

        {   
            LuaTableScope multisample_state_scope(L, "multisample_state", false, false);
            if (multisample_state_scope.IsValid()) {
                info.multisample_state.sample_count = static_cast<SDL_GPUSampleCount>(Script::ReadIntegerField(L, "sample_count").value_or(0));
                info.multisample_state.sample_mask = Script::ReadIntegerField(L, "sample_mask").value_or(0x0);
                info.multisample_state.enable_mask = Script::ReadBooleanField(L, "enable_mask").value_or(false);
            }
        } // mutisample_state_scope

        {   
            LuaTableScope depth_stencil_state_scope(L, "depth_stencil_state", false, false);
            if (!depth_stencil_state_scope.IsValid())
            {
                return false;
            }

            info.depth_stencil_state.compare_op = static_cast<SDL_GPUCompareOp>(Script::ReadIntegerField(L, "compare_op").value_or(0));

            {   
                LuaTableScope back_stencil_state_scope(L, "back_stencil_state", false, false);
                if (back_stencil_state_scope.IsValid())
                {
                    info.depth_stencil_state.back_stencil_state.fail_op = static_cast<SDL_GPUStencilOp>(Script::ReadIntegerField(L, "fail_op").value_or(0));
                    info.depth_stencil_state.back_stencil_state.pass_op = static_cast<SDL_GPUStencilOp>(Script::ReadIntegerField(L, "pass_op").value_or(0));
                    info.depth_stencil_state.back_stencil_state.depth_fail_op = static_cast<SDL_GPUStencilOp>(Script::ReadIntegerField(L, "depth_fail_op").value_or(0));
                    info.depth_stencil_state.back_stencil_state.compare_op = static_cast<SDL_GPUCompareOp>(Script::ReadIntegerField(L, "compare_op").value_or(0));
                }
            } // back_stencil_state_scope

            {   
                LuaTableScope front_stencil_state_scope(L, "front_stencil_state", false, false);
                if (front_stencil_state_scope.IsValid())
                {
                    info.depth_stencil_state.front_stencil_state.fail_op = static_cast<SDL_GPUStencilOp>(Script::ReadIntegerField(L, "fail_op").value_or(0));
                    info.depth_stencil_state.front_stencil_state.pass_op = static_cast<SDL_GPUStencilOp>(Script::ReadIntegerField(L, "pass_op").value_or(0));
                    info.depth_stencil_state.front_stencil_state.depth_fail_op = static_cast<SDL_GPUStencilOp>(Script::ReadIntegerField(L, "depth_fail_op").value_or(0));
                    info.depth_stencil_state.front_stencil_state.compare_op = static_cast<SDL_GPUCompareOp>(Script::ReadIntegerField(L, "compare_op").value_or(0));
                }
            } // front_stencil_state_scope

            info.depth_stencil_state.compare_mask = Script::ReadIntegerField(L, "compare_mask").value_or(0);
            info.depth_stencil_state.write_mask = Script::ReadIntegerField(L, "write_mask").value_or(0x0);
            info.depth_stencil_state.enable_depth_test = Script::ReadBooleanField(L, "enable_depth_test").value_or(false);
            info.depth_stencil_state.enable_depth_write = Script::ReadBooleanField(L, "enable_depth_write").value_or(false);
            info.depth_stencil_state.enable_stencil_test = Script::ReadBooleanField(L, "enable_stencil_test").value_or(false);
        } // depth_stencil_state_scope

        {   
            LuaTableScope target_info_scope(L, "target_info", false, false);
            if (target_info_scope.IsValid())
            {
                {   
                    LuaTableScope color_target_descriptions_scope(L, "color_target_descriptions", false, false);
                    if (color_target_descriptions_scope.IsValid())
                    {
                        uint32_t num_color_targets = Script::ReadArrayLength(L);
                        color_target_descriptions.resize(num_color_targets);

                        for (uint32_t i = 0; i < num_color_targets; ++i)
                        {
                            {   
                                LuaTableScope i_scope(L, i + 1);
                                if (i_scope.IsValid())
                                {
                                    SDL_GPUColorTargetDescription& color_target_description = color_target_descriptions[i];
                                    color_target_description.format = static_cast<SDL_GPUTextureFormat>(Script::ReadIntegerField(L, "format").value_or(0));
                                    {   
                                        LuaTableScope blend_state_scope(L, "blend_state", false, false);
                                        if (blend_state_scope.IsValid())
                                        {
                                            color_target_description.blend_state.src_color_blendfactor = static_cast<SDL_GPUBlendFactor>(Script::ReadIntegerField(L, "src_color_blendfactor").value_or(0));
                                            color_target_description.blend_state.dst_color_blendfactor = static_cast<SDL_GPUBlendFactor>(Script::ReadIntegerField(L, "dst_color_blendfactor").value_or(0));
                                            color_target_description.blend_state.color_blend_op = static_cast<SDL_GPUBlendOp>(Script::ReadIntegerField(L, "color_blend_op").value_or(0));
                                            color_target_description.blend_state.src_alpha_blendfactor = static_cast<SDL_GPUBlendFactor>(Script::ReadIntegerField(L, "src_alpha_blendfactor").value_or(0));
                                            color_target_description.blend_state.dst_alpha_blendfactor = static_cast<SDL_GPUBlendFactor>(Script::ReadIntegerField(L, "dst_alpha_blendfactor").value_or(0));
                                            color_target_description.blend_state.alpha_blend_op = static_cast<SDL_GPUBlendOp>(Script::ReadIntegerField(L, "alpha_blend_op").value_or(0));
                                            color_target_description.blend_state.color_write_mask = Script::ReadIntegerField(L, "color_write_mask").value_or(0);
                                            color_target_description.blend_state.enable_blend = Script::ReadBooleanField(L, "enable_blend").value_or(false);
                                            color_target_description.blend_state.enable_color_write_mask = Script::ReadBooleanField(L, "enable_color_write_mask").value_or(false);
                                        }
                                    } // blend_state_scope
                                }
                            } // i_scope
                        }
                    }
                } // color_target_descriptions_scope
                info.target_info = {
                    .color_target_descriptions = color_target_descriptions.data(),
                    .num_color_targets = static_cast<uint32_t>(color_target_descriptions.size()),
                };

                info.target_info.depth_stencil_format = static_cast<SDL_GPUTextureFormat>(Script::ReadIntegerField(L, "depth_stencil_format").value_or(0));
                info.target_info.has_depth_stencil_target = Script::ReadBooleanField(L, "has_depth_stencil_target").value_or(false);
            }
        } // target_info_scope
    } // pipeline_scope

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &info);
    if (!pipeline)
    {
        SO_ERROR("Failed to create graphics pipeline, {}", SDL_GetError());
        return false;
    }
    m_pipelines[ResourceManager::Hash(path.stem().string())] = pipeline;
    SO_INFO("Pipeline loaded: {}", path.stem().string());

    return true;
}