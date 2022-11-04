#pragma once
#include <cstdint>
#include <bit>

#ifndef __cpp_lib_byteswap
#include <array>
#include <concepts>
#include <algorithm>
#endif //__cpp_lib_byteswap

namespace LR35902
{

/* https://www.youtube.com/watch?v=b1Gq9WABaRU&ab_channel=CppNow */
template<typename Tag, typename Type, typename ... Properties>
class strong_typedef
:   Properties::template mixin<
        strong_typedef<Tag, Type, Properties...>,
        Type>...
{
public:
    constexpr strong_typedef() noexcept:value{} {} // default construction
    explicit constexpr strong_typedef(Type value):value{value} {} // value construction
    explicit constexpr operator const Type &() const noexcept { return value; } // operator const Type &
    constexpr const Type & underlying() const noexcept { return value; } // underlying(): const Type&
    constexpr Type& underlying() noexcept { return value; } // underlying(): Type&
private:
    Type value;
};


#ifdef __cpp_lib_byteswap
using std::byteswap;
#else //__cpp_lib_byteswap
template<std::integral T>
constexpr auto byteswap(T value) noexcept
{
    static_assert(std::has_unique_object_representations_v<T>,
                  "T may not have padding bits");
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::ranges::reverse(value_representation);
    return std::bit_cast<T>(value_representation);
}
#endif //__cpp_lib_byteswap

template<std::endian E>
struct endian{};

template<std::endian E>
struct uint16e_t
:   strong_typedef<
        endian<E>,
        std::uint16_t>
{
    using Tag = endian<E>;
    using Type = std::uint16_t;
    //using Properties = TBD;
    using strong_typedef<Tag, Type>::strong_typedef;

    template<std::endian E2>
    explicit constexpr operator uint16e_t<E2>() const noexcept;
};

template<>
template<>
constexpr uint16e_t<std::endian::big>::operator uint16e_t<std::endian::little>() const noexcept
{
    return uint16e_t<std::endian::little>{byteswap(underlying())};
}


template<>
template<>
constexpr uint16e_t<std::endian::little>::operator uint16e_t<std::endian::big>() const noexcept
{
    return uint16e_t<std::endian::big>{byteswap(underlying())};
}


//LR35902 Specific
using sim16_t = uint16e_t<std::endian::big>;
using nat16_t = uint16e_t<std::endian::native>;
}