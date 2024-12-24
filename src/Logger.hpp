#pragma once
#include <format>
#include "Types.hpp"

struct LogLevel
{
    using value_type = uint32_t;
};
using LogLevelFlag = Flags<LogLevel>;
struct LogLevelFlagBits
{
    static constexpr LogLevelFlag None { 0u };
    static constexpr LogLevelFlag Info { BIT(2) };
    static constexpr LogLevelFlag Warn { BIT(3) };
    static constexpr LogLevelFlag Error{ BIT(4) };
    static constexpr LogLevelFlag All  { BIT(5) - 1};
};

class Logger
{
public:
    static void Initialize();
    static void Destroy();
    static void SetFilter(LogLevelFlag filter);

    static void LogF(
        LogLevelFlag level,
        std::string_view file,
        std::string_view function,
        int line,
        std::string_view fmt, std::format_args args);

    template<typename... Args>
    static void Log(
        LogLevelFlag level,
        std::string_view file,
        std::string_view function,
        int line,
        std::format_string<Args...> fmt, Args&&... args)
    {
        LogF(level, file, function, line, fmt.get(), std::make_format_args(args...));
    }
private:
    static auto ShouldLog(LogLevelFlag level) -> bool;
};

#define SO_LOG(level, ...) Logger::Log(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define SO_INFO(...)  SO_LOG(LogLevelFlagBits::Info,  __VA_ARGS__)
#define SO_WARN(...) SO_LOG(LogLevelFlagBits::Warn,  __VA_ARGS__)
#define SO_ERROR(...) SO_LOG(LogLevelFlagBits::Error, __VA_ARGS__)