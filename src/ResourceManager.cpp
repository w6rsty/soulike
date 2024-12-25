#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define JSON_NOEXCEPTION
#define TINYGLTF_NOEXCEPTION
#include <tiny_gltf.h>

#include "ResourceManager.hpp"
#include "Logger.hpp"

namespace {
    ResourceManager*       s_instance{ nullptr };
    lua_State*             s_lua_state{ nullptr };
    std::hash<std::string> s_hasher{};
}

void ResourceManager::Initialize(std::filesystem::path const& root, SDL_GPUDevice* device)
{
    if (s_instance)
    {
        return;
    }
    s_instance = new ResourceManager();

    s_lua_state = luaL_newstate();
    luaL_openlibs(s_lua_state);

    s_instance->m_root_dir = root;
    s_instance->m_device = device;

    std::filesystem::path prelude_files = root/"preludes";
    for (auto& entry : std::filesystem::directory_iterator(prelude_files))
    {
        std::filesystem::path path = entry.path();
        if (path.extension().string() == ".lua")
        {
            if (luaL_dofile(s_lua_state, path.c_str()))
            {
                SO_ERROR("Error loading prelude: {}", lua_tostring(s_lua_state, -1));
                lua_pop(s_lua_state, 1);
            }
        }
    }

    // Shaders - load shader scripts
    std::filesystem::path shaders_dir = root/"shaders";
    for (auto& entry : std::filesystem::directory_iterator(shaders_dir))
    {
        std::filesystem::path path = entry.path();
        if (path.extension().string() == ".lua")
        {
            s_instance->LoadShader(s_lua_state, path);
        }
    }
    
    // Graphics Pipelines - load graphics pipeline scripts
    std::filesystem::path pipelines_dir = root/"pipelines";
    for (auto& entry : std::filesystem::directory_iterator(pipelines_dir))
    {
        std::filesystem::path path = entry.path();
        if (path.extension().string() == ".lua")
        {
            s_instance->LoadPipeline(s_lua_state, path);
        }
    }
}

auto ResourceManager::Instance() -> ResourceManager&
{
    return *s_instance;
}

void ResourceManager::Destroy()
{
    for (auto& [_, shader] : s_instance->m_shaders)
    {
        SDL_ReleaseGPUShader(s_instance->m_device, shader.second);
    }

    for (auto& [_, pipeline] : s_instance->m_pipelines)
    {
        SDL_ReleaseGPUGraphicsPipeline(s_instance->m_device, pipeline);
    }

    if (s_instance)
    {
        s_instance->m_root_dir.clear();
        s_instance->m_shaders.clear();
        s_instance->m_pipelines.clear();

        delete s_instance;
        lua_close(s_lua_state);
        s_instance = nullptr;
        s_lua_state = nullptr;
    }
}

auto ResourceManager::LoadModel(std::filesystem::path const& path) -> bool
{
    tinygltf::Model model{};
    tinygltf::TinyGLTF loader{};
    std::string err{};
    std::string warn{};
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path.string());


    return ret;
}

auto ResourceManager::GetShader(std::string const& name) -> SDL_GPUShader*
{
    return m_shaders[Hash(name)].second;
}

auto ResourceManager::GetPipeline(std::string const& name) -> SDL_GPUGraphicsPipeline*
{
    return m_pipelines[Hash(name)];
}


auto ResourceManager::Hash(const std::string &str) -> ResouceID
{
    return s_hasher(str);
}

auto ResourceManager::Slangc(SlangcCompileOption const& option) -> bool
{
    if (option.input_file.empty())
    {
        SO_ERROR("Input file is empty");
        return false;
    }

    if (!std::filesystem::exists(option.input_file))
    {
        SO_ERROR("File not found: {}", option.input_file.string());
        return false;
    }

    if (option.output_file.empty())
    {
        SO_ERROR("Output directory is empty");
        return false;
    }

    std::string shader_format_str{ "" };
    switch (option.format)
    {
        case SDL_GPU_SHADERFORMAT_SPIRV:    
            shader_format_str = "spirv";
            break;
        case SDL_GPU_SHADERFORMAT_MSL:
            shader_format_str = "metal";
            break;
        default:
            SO_ERROR("Unsupported shader format");
            return false;
    }

    std::string profile = std::format(
        "slangc -profile {} -target {}",
        "glsl_450",
        shader_format_str);

    std::string io = std::format(
        "{} -o {} -entry {}",
        option.input_file.string(),
        option.output_file.string(),
        option.entry_point);

    std::string shader_stage_str{ "" };
    switch (option.stage)
    {
    case SDL_GPUShaderStage::SDL_GPU_SHADERSTAGE_VERTEX:
        shader_stage_str = "vertex";
        break;
    case SDL_GPUShaderStage::SDL_GPU_SHADERSTAGE_FRAGMENT:
        shader_stage_str = "fragment";
        break;
    };

    std::string options = std::format(
        "-stage {} -O{} -g{}",
        shader_stage_str,
        static_cast<uint32_t>(option.optimization_level),
        static_cast<uint32_t>(option.debug_level));

    std::string command = std::format("{} {} {}", profile, io, options);
    int result = std::system(command.c_str());
    if (result != 0)
    {
        SO_ERROR("Failed to compile: {}", option.input_file.string());
    }
    SO_INFO("Shader compiled: {}", option.output_file.string());
    return result == 0;
}

void ResourceManager::DebugLuaStack(lua_State* L)
{
    std::string output = "Lua stack:\n";

    for (int i = 1; i <= lua_gettop(L); i++)
    {
        switch (lua_type(L, i))
        {
        case LUA_TSTRING:
            output += std::format("[{}] string: {}\n", i, lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            output += std::format("[{}] boolean: {}\n", i, lua_toboolean(L, i) ? "true" : "false");
            break;
        case LUA_TNUMBER:
            output += std::format("[{}] number: {}\n", i, lua_tonumber(L, i));
            break;
        case LUA_TTABLE:
            output += std::format("[{}] table\n", i);
            break;
        case LUA_TFUNCTION:
            output += std::format("[{}] function\n", i);
            break;
        case LUA_TUSERDATA:
            output += std::format("[{}] userdata\n", i);
            break;
        case LUA_TLIGHTUSERDATA:
            output += std::format("[{}] light userdata\n", i);
            break;
        case LUA_TNIL:
            output += std::format("[{}] nil\n", i);
            break;
        default:
            output += std::format("[{}] unknown type\n", i);
            break;
        }
    }

    SO_INFO("{}", output);
}

void ResourceManager::DebugLuaShowTable(lua_State *L) {
    if (!lua_istable(L, -1))
    {
        SO_WARN("Not a table");
        return;
    }

    std::string output = "Lua table:\n";
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
        std::string key, value;

        if (lua_type(L, -2) == LUA_TSTRING)
        {
            key = std::format("\"{}\"", lua_tostring(L, -2));
        }
        else if (lua_type(L, -2) == LUA_TNUMBER)
        {
            key = std::format("[{}]", lua_tonumber(L, -2));
        } 
        else
        {
            key = std::format("<{}>", lua_typename(L, lua_type(L, -2)));
        }

        switch (lua_type(L, -1))
        {
        case LUA_TSTRING:
            value = std::format("\"{}\"", lua_tostring(L, -1));
            break;
        case LUA_TNUMBER:
            value = std::format("{}", lua_tonumber(L, -1));
            break;
        case LUA_TBOOLEAN:
            value = lua_toboolean(L, -1) ? "true" : "false";
            break;
        case LUA_TTABLE:
            value = "table";
            break;
        case LUA_TFUNCTION:
            value = "function";
            break;
        case LUA_TUSERDATA:
            value = "userdata";
            break;
        case LUA_TLIGHTUSERDATA:
            value = "light userdata";
            break;
        case LUA_TNIL:
            value = "nil";
            break;
        default:
            value = "<unknown>";
            break;
        }

        output += std::format("{}: {}\n", key, value);

        lua_pop(L, 1);
    }

    output.pop_back();
    SO_INFO("{}", output);
}