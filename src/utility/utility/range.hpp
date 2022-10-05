#pragma once

#include <cstdint>
#include <concepts>
#include <string_view>
#include <string>

template<std::integral T>
struct InclusiveRange
{
public:

    consteval InclusiveRange(const T begin, const T end) // create a range [begin, end]
    : begin_{begin}
    , end_{end}
    {
        static_assert(begin <= end, "begin must be <= to end");
    }

    constexpr bool isMember(const T t) const {
        return begin_ <= t && t <= end_;
    }

    constexpr T begin() const {
        return begin_;
    }

    constexpr T end() const {
        return end_;
    }

    constexpr size_t size() const {
        return end_ - begin_ + 1; //inclusive range
    }

    constexpr std::string to_string() const {
        return "[" + std::to_string(begin) + ", " + std::to_string(end) << "]";
    }

    const T begin_;
    const T end_;
};