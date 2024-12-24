#pragma once
#include <compare> // IWYU pragma: keep
#include <type_traits>

#define BIT(x) (1 << x)

template <typename Category>
struct Flags
{
    using value_type = typename Category::value_type;
    static_assert(std::is_integral_v<value_type>, "Flag bits underlying type must be integral");
    value_type value;

    auto operator |= (Flags const& rhs) -> Flags&
    {
        value |= rhs.value;
        return *this;
    }

    auto operator &= (Flags const& rhs) -> Flags&
    {
        value &= rhs.value;
        return *this;
    }

    auto operator ^= (Flags const& rhs) -> Flags&
    {
        value ^= rhs.value;
        return *this;
    }

    [[nodiscard]] auto operator ~ () const -> Flags
    {
        return Flags{~value};
    }

    [[nodiscard]] auto operator | (Flags const& rhs) const -> Flags
    {
        return Flags{value | rhs.value};
    }

    [[nodiscard]] auto operator & (Flags const& rhs) const -> Flags
    {
        return Flags{value & rhs.value};
    }

    [[nodiscard]] auto operator ^ (Flags const& rhs) const -> Flags
    {
        return Flags{value ^ rhs.value};
    }

    [[nodiscard]] auto operator <=> (Flags const& rhs) const = default;

    [[nodiscard]] operator bool () const
    {
        return value != 0;
    }
};