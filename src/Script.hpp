#pragma once
#include <filesystem>
#include <lua.hpp>

class LuaTableScope
{
public:
    LuaTableScope(lua_State* L, char const* table, bool is_global = true, bool required = true);
    // Table as element in an array
    LuaTableScope(lua_State* L, int index, bool required = true);
    ~LuaTableScope();
    [[nodiscard]] auto IsValid() const -> bool;
private:
    lua_State* m_state;
    bool m_valid;
};

struct Script
{
    [[nodiscard]] static auto Load(lua_State* L, std::filesystem::path const& path) -> bool;
    [[nodiscard]] static auto ReadArrayLength(lua_State* L) -> int;
    [[nodiscard]] static auto ReadStringField(lua_State* L, char const* key) -> std::optional<std::string>;
    [[nodiscard]] static auto ReadFloatingField(lua_State* L, char const* key) -> std::optional<float>;
    [[nodiscard]] static auto ReadIntegerField(lua_State* L, char const* key) -> std::optional<int>;
    [[nodiscard]] static auto ReadBooleanField(lua_State* L, char const* key) -> std::optional<bool>;
};