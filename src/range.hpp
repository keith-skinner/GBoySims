#pragma once

#include <cstdint>
#include <concepts>
#include <string>

template<std::integral T, T Begin, T End = Begin>
struct InclusiveRange
{
    static constexpr bool isMember(const T t) const {
        if constexpr (Begin == End)
            return Begin == t;
        else
            return Begin <= t && t <= End;
    }

    static constexpr T begin() const {
        return Begin;
    }

    static constexpr T end() const {
        return End;
    }

    static constexpr size_t size() const {
        return End - Begin + 1; //inclusive range
    }

    static constexpr std::string to_string() const {
        return "[" + std::to_string(begin) + ", " + std::to_string(end) << "]";
    }
};
