#include <iostream>
#include "Logger.hpp"

namespace {
    Logger*      s_instance{ nullptr };
    LogLevelFlag s_Filter{ LogLevelFlagBits::None };
}

void Logger::Initialize()
{
    s_instance = new Logger();
}

void Logger::Destroy()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

void Logger::SetFilter(LogLevelFlag filter)
{
    s_Filter = filter;
}

void Logger::LogF(
    LogLevelFlag level,
    std::string_view file,
    std::string_view function,
    int line,
    std::string_view fmt, std::format_args args)
{
    if (ShouldLog(level))
    {
        std::cout << std::format(
            "󱞩[ {}:{} ({}) ] {}\n",
            file, line, function,
            std::vformat(fmt, args)
        );
    }
}

auto Logger::ShouldLog(LogLevelFlag level) -> bool
{
    return s_instance && (level & ~s_Filter);
}