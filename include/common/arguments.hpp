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
 * @tparam NameT Certain Opcodes only work on certain registers. Defining different "names" can enforce 
 * @tparam T Representation of the register as a type. Ex. std::same_as<Flags::T, bool> or std::same_as<EAX::T, uint32_t>
 */
template<typename NameT, std::unsigned_integral T>
struct register_t
{
    using Name = NameT;
    using Type = T;
    const Type value{};
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
    using Access = AccessT;
    using Name = typename Type::Name;
    
    static constexpr Name name{};

    constexpr argument_t() = default;
    constexpr argument_t(const size_t value)
    :   type{static_cast<typename Type::Type>(value)}
    {}

    const Type type = {};
    constexpr auto value() const { return type.value; }
};

} // namespace common::Arguments