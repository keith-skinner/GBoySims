#pragma once
#include <cstdint>
#include <type_traits>
#include <concepts>
#include <algorithm>
#include <limits>
#include <tuple>
#include <utility>

#include <cib/tuple.hpp>

#include "utility/meta.hpp"

namespace CPU
{

template <
    typename NameT,
    std::size_t MsbV, // inclusive
    std::size_t LsbV> // inclusive
class Register
{
public:
    static_assert(LsbV < MsbV, "Msb must be greater than or equal to Lsb. Msb == Lsb implies 1 bit.");

    using Name = NameT;
    static consteval Name name{};
    static consteval std::size_t MSB = MsbV;
    static consteval std::size_t LSB = LsbV;
    static consteval std::size_t WIDTH = MSB - LSB + 1;

    [[nodiscard]] static constexpr std::uint64_t create_bit_mask() const {
        if constexpr (WIDTH == std::numeric_limits<std::uint64_t>::digits)
            return std::numeric_limits<std::uint64_t>::max();
        else
            return (1ULL << static_cast<std::uint64_t>(width)) - 1ULL;
    };

    static consteval std::uint64_t bit_mask = create_bit_mask(); // eg: 0b0000'0111
    static consteval std::uint64_t reg_mask = bit_mask << lsb; // eg: 0b0011'1000

    //extract the bits represented by this register from data
    template<typename DataStore>
    static constexpr auto read(const DataStore data)
    {
        return (data & reg_mask) >> lsb;
    }

    //insert the bits represented by this register from value into data
    template <typename DataStore>
    static constexpr void write(DataStore & data, const auto value)
    {
        // align value bits with bits to insert value into
        // zero out bits to insert value into and insert value into data
        const auto shifted_value = value << lsb ;
        data = (shifted_value & reg_mask) | (data & ~reg_mask );
    }
};


/**
 * @brief A grouping of registers that logically all write to the same place
 *
 * @tparam Repr What memory representation the registerdef should be. Must be able to be indexable by all subregisters
 * @tparam SubRegisters All addressable registers of this registerdef
 */
template <
    std::integral ReprT,
    std::size_t WidthV,
    typename ... SubRegisters>
class RegisterDef
{
public:
    static constexpr std::width_t Width = WidthV;
    using Repr = ReprT;

    static_assert(Width >= std::numeric_limits<ReprT>::digits, "Repr must be at least Width number of bits.");
    static_assert((true && ... && (Width > SubRegisters::MSB)), "Width must be greater than all subregister msb.");

    template <typename SubRegister>
    static constexpr bool is_registered_v = meta::is_member_of_v<std::remove_cvref_t<SubRegister>, SubRegisters...>;

    template <typename SubRegister>
    constexpr void write(typename SubRegister::Repr reg)
    {
        static_assert(is_registered_v<SubRegister>, "Register is not registered to this register definition.");
        SubRegister::write(m_value, reg);
    }

    template <typename SubRegister>
    constexpr Repr read() const {
        static_assert(is_registered_v<SubRegister>, "Register is not registered to this register definition.");
        return SubRegister::read(m_value);
    }

    constexpr Repr value() const {
        return m_value;
    }

private:
    Repr m_value{}; // actually holds a value unlike register_t which is strictly type info
};


template<typename SubRegister>
struct Find
{
    // bind SubRegister
    template<typename RegisterList>
    struct is_registered : std::bool_constant<RegisterList::template is_registered_v<SubRegister> {};

    template<typename RegisterList>
    static constexpr bool is_registered_v = is_registered<RegisterList>::value;

    template<typename ... RegisterLists>
    static constexpr std::size_t index = meta::find_if_v<is_registered, RegisterLists...>;
};

template <
    typename NameT,
    typename ... RegisterLists>
class RegisterFile
{
public:
    using Name = NameT;

    template<typename SubRegister>
    constexpr void write(typename SubRegister::Repr reg)
    {
        static constexpr std::size_t INDEX = Find<SubRegister>.index<RegisterLists>;
        static_assert(INDEX < sizeof...(RegisterLists), "Could not find SubRegister in RegisterLists");
        std::get<INDEX>(registers).template write<SubRegister>(reg);
    }

    template<typename SubRegister>
    constexpr auto read() const
    {
        static constexpr std::size_t INDEX = Find<SubRegister>.index<RegisterLists>;
        static_assert(INDEX < sizeof...(RegisterLists), "Could not find SubRegister in RegisterLists");
        return std::get<INDEX>.template read<SubRegister>();
    }

private:
    std::tuple<RegisterLists...> registers{};
};


} // namespace CPU