#pragma once
#include <concepts>

namespace common::Arguments::Access
{


template<typename T>
struct immediate_t{
    using Type = T;
    static constexpr Type offset{0};
};

template<typename T, std::unsigned_integral I, I Offset>
struct memory_t {
    using Type = T;
    using OffsetType = I;
    static constexpr OffsetType value = Offset;
    static constexpr Type offset{Offset};
};

template<typename T, typename Type = typename T::Type>
static constexpr bool is_immediate_v = std::is_same_v<T, immediate_t<Type>>;
template<typename T>
concept a_immediate = is_immediate_v<T>;

template<typename T, typename Type = typename T::Type, typename OffsetType= typename T::OffsetType, OffsetType Offset = T::value>
static constexpr bool is_reference_v = std::is_same_v<T, memory_t<Type, OffsetType, Offset>>;
template<typename T>
concept a_reference = is_reference_v<T>;

template<typename T>
concept a_arg_access = a_immediate<T> || a_reference<T>;


} // namespace Arguments::Access
namespace common::Arguments::Type
{

template <std::integral T>
struct immediate_t
{
    using Type = T;
    const Type value{};
};

template<typename T, std::integral Type = typename T::Type>
static constexpr bool is_immediate_v = std::is_same_v<T, immediate_t<Type>>;
template<typename T>
concept a_immediate = is_immediate_v<T>;


template<std::unsigned_integral T>
struct register_t
{
    using Type = T;
    [[deprecated("Value of a register should not be used")]]
    const Type value{};
};

template<typename T, typename Type = typename T::Type>
static constexpr bool is_register_v = std::is_same_v<T, register_t<Type>>;
template<typename T>
concept a_register = is_register_v<T>;   


} // namespace Arguments::Type
namespace common::Arguments
{

template<
    typename T,
    typename A>
struct argument_t {
    using Type = T;
    using Access = A;

    constexpr argument_t() = default;
    constexpr argument_t(const size_t value)
    :   type{value}
    {}

    const Type type = {};
    // will never be instantiating an access
    // type includes the raw data or memory address being accessed
    // therefore it needs to be instantiated sometimes
    // if type is a register it doesn't need to be accessed and the
    // required info can be deduced at compile time
};

} // namespace common::Arguments::Argument