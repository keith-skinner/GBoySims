#pragma once
#include <tuple>
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <cib/tuple.hpp>

#include "utility/meta.hpp"

namespace common
{

// register_file_t is templated with register_def_t's that are templated with regster_t's
// how to select the correct register_def_t inside a register_file_t after getting passed a register_t

// https://github.com/intel/compile-time-init-build


template<typename Register>
static constexpr std::size_t get_index_helper()
{
    return -1;
}

template<typename Register, typename RegDef, typename ... Rest>
static constexpr std::size_t get_index_helper()
{
    if constexpr (RegDef::template is_valid_register<Register>)
        return 0;
    else 
        return 1 + get_index_helper<Register, Rest...>();
}


template <
    typename NameT,
    typename ReprT,
    std::size_t MsbV,
    std::size_t LsbV>
class register_t
{
public:
    using Name = NameT;
    using Repr = ReprT;
    static constexpr Name name{};
    static constexpr std::size_t msb = MsbV;
    static constexpr std::size_t lsb = LsbV;
    static constexpr std::size_t width = msb - lsb + 1;

    static_assert(lsb <= msb, "Msb needs to be lower than or equal to Lsb");

    Repr value{};
    explicit operator std::uint64_t() const { return value; }

    constexpr register_t() noexcept = default;
    constexpr register_t(Repr value) noexcept : value{value} {};

    static constexpr uint64_t bit_mask = []() consteval -> std::uint64_t {
        if constexpr (width == 64)
            return 0xFFFFFFFFFFFFFFFF;
        else
            // Move 1 to bit size, make bit size 0 and all other lower bits 1
            // E.g. Msb=7 & Lsb=4 -> width=4; shift 1 by 4; 0b10000; -1; 0b01111;
            // E.g. Msb=0 & Lsb=0 -> width=1; shift 1 by 1; 0b00010; -1; 0b00001;
            return (static_cast<uint64_t>(1) << static_cast<uint64_t>(width)) - static_cast<uint64_t>(1);
    }();
    static constexpr uint64_t reg_mask = bit_mask << lsb;

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
        data = (static_cast<std::uint64_t>(value) << lsb) | (data & ~reg_mask);
    }
};


/**
 * @brief Register value that describes a Register and its SubRegisters
 * @note  A flag can be described as a subregister with the width of 1 bit
 *
 * @tparam Name
 * @tparam Repr What representation a register should be
 * @tparam Width Max number of bits representable in the register
 * @tparam SubRegisters All addressable registers of this register including the name of the register itself
 */
template <
    typename NameT,
    typename ReprT,
    std::size_t WidthV,
    typename ... SubRegisters>

class registerdef_t
{
public:
    using Name = NameT;
    using Repr = ReprT;
    static constexpr Name name{};
    static constexpr std::size_t repr_width = (sizeof(Repr) * 8) - 1;
    static constexpr std::size_t width = WidthV;

    static_assert((width > 0), "Register must have a Width of at least 1");
    static_assert((width >= repr_width), "Repr must be able to contain width");
    static_assert((true && ... && (SubRegisters::msb <= width)), "SubRegister must be contained within width");
    static_assert((true && ... && (SubRegisters::msb <= repr_width)), "SubRegister must be contained within representation");

    template<typename ... SubRegisterTypes>
    constexpr 
    registerdef_t(SubRegisterTypes... subregisters)
    : value {}
    {
        (write(std::forward<SubRegisterTypes>(subregisters)), ...);
    }

    template <typename SubRegister>
    static constexpr bool is_valid_register = is_member_of_v<std::remove_cvref_t<SubRegister>, SubRegisters...>;

    template <typename SubRegister>
    constexpr void write(SubRegister reg)
    {
        static_assert(is_valid_register<SubRegister>, "Must be writing to a registered subregister.");
        SubRegister::write(value, reg);
    }

    template <typename SubRegister>
    constexpr Repr read() const {
        static_assert(is_valid_register<SubRegister>, "Must be reading from a registered subregister");
        return SubRegister::read(value);
    }

private:
    Repr value{};
};

template <typename NameT, typename ... SubRegisters>
class registerfile_t {
public:
    using Name = NameT;
    static constexpr Name name{};

    template<typename ... RegisterList>
    constexpr registerfile_t(RegisterList... regs)
    : registers{}
    {
        (set<RegisterList>(regs), ...);
    }


    template <typename SubRegister>
    constexpr void set(SubRegister reg)
    {
        using Register = std::remove_cvref_t<SubRegister>;
        std::get<Register>(registers) = reg;
    }

    template <typename SubRegister>
    static constexpr bool is_valid_register = (false || ... || SubRegisters::template is_valid_register<SubRegister>);

    template<typename SubRegister>
    constexpr void write(SubRegister reg)
    {
        std::get<(get_index<SubRegister>)>(registers).write(reg);
    }

    template<typename SubRegister>
    constexpr auto read([[maybe_unused]] SubRegister r = SubRegister{})
    {
        static_assert(is_valid_register<SubRegister>,
            "Must be reading from a registered subregister");
        return std::get<(get_index<SubRegister>)>(registers).template read<SubRegister>();
    }

    template<typename ... FlagRegisters>
    constexpr void flags(FlagRegisters ... rflags)
    {
        // do nothing if empty
        if constexpr (sizeof...(FlagRegisters) == 0)
            return;
        static_assert((true && ... && is_valid_register<FlagRegisters>),
            "FlagRegisters must be registered to this register file");
        (write(rflags), ...);
    }

    template <typename SubRegister>
    static constexpr std::size_t get_index = get_index_helper<SubRegister, SubRegisters...>();

private:
    std::tuple<SubRegisters...> registers;
};


} // namespace common