#pragma once
#include <map>
#include <utility>
#include <filesystem>

#include <lua.hpp>
#include <SDL3/SDL_gpu.h>

enum class ShaderOptimizationLevel
{
    None    = 0,
    Default = 1,
    High    = 2,
    Maximal = 3
};

enum class ShaderDebugLevel
{
    None     = 0,
    Minimal  = 1,
    Standard = 2,
    Maximal  = 3
};

struct SlangcCompileOption
{
    std::filesystem::path   input_file;
    std::filesystem::path   output_file;
    std::string             entry_point;
    SDL_GPUShaderStage      stage;
    SDL_GPUShaderFormat     format;
    ShaderOptimizationLevel optimization_level{ ShaderOptimizationLevel::Default };
    ShaderDebugLevel        debug_level{ ShaderDebugLevel::None };
};

struct ShaderInfo
{
    bool                  is_byte_code;
    std::filesystem::path source_path;
    SDL_GPUShaderStage    stage;
    uint32_t              format;
    std::string           entry_point;
    uint32_t              num_samplers;
    uint32_t              num_storage_buffers;
    uint32_t              num_uniform_buffers;
};

using ResouceID = std::size_t;

class ResourceManager
{
public:
    static void Initialize(std::filesystem::path const& root, SDL_GPUDevice* device);
    [[nodiscard]] static auto Instance() -> ResourceManager&;
    static void Destroy();

    auto LoadShader(lua_State* L, std::filesystem::path const& path) -> bool;
    auto LoadPipeline(lua_State* L, std::filesystem::path const& path) -> bool;
    auto LoadModel(std::filesystem::path const& path) -> bool;

    [[nodiscard]] auto GetShader(std::string const& name) -> SDL_GPUShader*;
    [[nodiscard]] auto GetPipeline(std::string const& name) -> SDL_GPUGraphicsPipeline*;

    [[nodiscard]] static auto Hash(std::string const& str) -> ResouceID;
    static auto Slangc(SlangcCompileOption const& option) -> bool;
    static void DebugLuaStack(lua_State* L);
    static void DebugLuaShowTable(lua_State *L);
private:
    std::string m_root_dir;
    SDL_GPUDevice* m_device;
    std::map<ResouceID, std::pair<ShaderInfo, SDL_GPUShader*>> m_shaders;
    std::map<ResouceID, SDL_GPUGraphicsPipeline*> m_pipelines;
};