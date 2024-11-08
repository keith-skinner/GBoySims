#pragma once
#include <concepts>


namespace common::Arguments::Access
{
template<typename OffsetT>
struct immediate_t{
    using offset_t = OffsetT;
    static constexpr offset_t offset_v{0};
};

template<std::integral OffsetT, OffsetT OffsetV>
struct reference_t {
    using offset_t = OffsetT;
    static constexpr offset_t offset_v{OffsetV};
};

template<typename T, typename offset_t = typename T::offset_t>
static constexpr bool is_immediate_v = std::is_same_v<T, immediate_t<offset_t>>;
template<typename T>
concept a_immediate = is_immediate_v<T>;

template<typename T, typename offset_t = typename T::offset_t, offset_t offset_v = T::offset_v>
static constexpr bool is_reference_v = std::is_same_v<T, reference_t<offset_t, offset_v>>;
template<typename T>
concept a_reference = is_reference_v<T>;


} // namespace common::Arguments::Access

namespace common::Arguments::Type
{

template <typename NameT, std::integral T>
struct immediate_t
{
    using Name = NameT;
    using Type = T;
    const Type value{};
};

/**
 * @brief 
 * 
 * @tparam NameT  Certain opcodes can only work on certain registers. Naming the register can enforce this.
 * @tparam T      In memory representation of the register as a type.
 */
template<typename NameT, std::unsigned_integral T>
struct register_t
{
    using Name = NameT;
    using Type = T;
    Type value{};
};

template<typename T, typename Name = typename T::Name, std::integral Type = typename T::Type>
static constexpr bool is_immediate_v = std::is_same_v<T, immediate_t<Name, Type>>;
template<typename T>
concept a_immediate = is_immediate_v<T>;

template<typename T, typename Name = typename T::Name, typename Type = typename T::Type>
static constexpr bool is_register_v = std::is_same_v<T, register_t<Name, Type>>;
template<typename T>
concept a_register = is_register_v<T>;


} // namespace common::Arguments::Type
namespace common::Arguments
{

template<
    typename TypeT,
    typename AccessT>
struct argument_t {
    using Type = TypeT;
    using Name = Type::Name;
    using Access = AccessT;

    constexpr argument_t() = default;
    constexpr argument_t(const Type::Type value)
    : value{value}
    {}

    using Repr = typename Type::Type;
    typename Type::Type value = {};
};

} // namespace common::Arguments