#pragma once
#include <concepts>

namespace common::Arguments::Access
{


template<typename OffsetT>
struct immediate_t{
    using offset_t = OffsetT;
    static constexpr offset_t offset{0};
};

template<typename OffsetT, std::unsigned_integral IOffsetT, IOffsetT OffsetV>
struct reference_t {
    using offset_t = OffsetT;
    using constant_t = IOffsetT;
    static constexpr constant_t constant_value = OffsetV;
    static constexpr offset_t offset{OffsetV};
};

template<typename T, typename offset_t = typename T::offset_t>
static constexpr bool is_immediate_v = std::is_same_v<T, immediate_t<offset_t>>;
template<typename T>
concept a_immediate = is_immediate_v<T>;

template<typename T, typename offset_t = typename T::offset_t, typename constant_t = typename T::constant_t, constant_t constant_value = T::constant_value>
static constexpr bool is_reference_v = std::is_same_v<T, reference_t<offset_t, constant_t, constant_value>>;
template<typename T>
concept a_reference = is_reference_v<T>;


} // namespace Arguments::Access
namespace common::Arguments::Type
{

template <std::integral T>
struct immediate_t
{
    using Type = T;
    const Type value{};
};

template<std::unsigned_integral T>
struct register_t
{
    using Type = T;
    [[deprecated("Value of a register should not be used")]]
    const Type value{};
};

template<typename T, std::integral Type = typename T::Type>
static constexpr bool is_immediate_v = std::is_same_v<T, immediate_t<Type>>;
template<typename T>
concept a_immediate = is_immediate_v<T>;

template<typename T, typename Type = typename T::Type>
static constexpr bool is_register_v = std::is_same_v<T, register_t<Type>>;
template<typename T>
concept a_register = is_register_v<T>;


} // namespace Arguments::Type
namespace common::Arguments
{

template<
    typename TypeT,
    typename AccessT>
struct argument_t {
    using Type = TypeT;
    using Access = AccessT;

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