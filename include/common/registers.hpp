#pragma once
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <limits>
#include <tuple> // needed for std::get to work on indices

#include <cib/tuple.hpp>

#include "utility/meta.hpp"

namespace common
{

// register_file_t is templated with register_def_t's that are templated with register_t's
// how to select the correct register_def_t inside a register_file_t after getting passed a register_t

// https://github.com/intel/compile-time-init-build

template <
    typename NameT,
    typename ReprT,
    std::size_t MsbV, // inclusive
    std::size_t LsbV> // inclusive
class register_t
{
public:
    static_assert(LsbV <= MsbV, "Msb must be greater than or equal to Lsb. Msb == Lsb implies 1 bit.");

    using Name = NameT;
    using Repr = ReprT;
    static constexpr Name name{};
    static constexpr std::size_t msb = MsbV;
    static constexpr std::size_t lsb = LsbV;
    // bits [5, 9] means bits 5, 6, 7, 8, 9
    // bits [5, 5] means bits 5
    static constexpr std::size_t width = msb - lsb + 1;

    // mask for the value
    static constexpr uint64_t bit_mask = []() consteval -> std::uint64_t {
        if constexpr (width == 64)
            return 0xFFFFFFFFFFFFFFFF;
        else
            // Move 1 to bit size, make bit size 0 and all other lower bits 1
            // E.g. Msb=7 & Lsb=4 -> width=4; shift 1 by 4; 0b10000; -1; 0b01111;
            // E.g. Msb=0 & Lsb=0 -> width=1; shift 1 by 1; 0b00010; -1; 0b00001;
            return (static_cast<uint64_t>(1) << static_cast<uint64_t>(width)) - static_cast<uint64_t>(1);
    }();
    // mask for the larger register representation
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
 * @brief A grouping of registers that logically all write to the same place
 *
 * @tparam Name A unique name to distinguish register defs that have the same repr and subregisters
 * @tparam Repr What memory representation the registerdef should be. Must be able to be indexable by all subregisters
 * @tparam SubRegisters All addressable registers of this registerdef
 */
template <
    typename NameT,
    std::integral ReprT,
    size_t WidthT,
    typename ... SubRegisters>
class registerdef_t
{
public:
    static_assert(WidthT >= std::numeric_limits<ReprT>::digits, "Repr must be at least Width number of bits");
    static_assert((true && ... && (WidthT > SubRegisters::msb)), "WidthT");

    using Name = NameT;
    using Repr = ReprT;
    static constexpr Name name{};

    template<typename ... SubRegisterTypes>
    constexpr registerdef_t(SubRegisterTypes&&... subregisters)
    : m_value {}
    {
        (write(std::forward<SubRegisterTypes>(subregisters)), ...);
    }
    constexpr registerdef_t(Repr value) : m_value(value) {}

    template <typename SubRegister>
    static constexpr bool is_registered = is_member_of_v<std::remove_cvref_t<SubRegister>, SubRegisters...>;

    template <typename SubRegister>
    constexpr void write(typename SubRegister::Repr reg)
    {
        static_assert(is_registered<SubRegister>, "Must be writing to a registered subregister.");
        SubRegister::write(m_value, reg);
    }

    template <typename SubRegister>
    constexpr Repr read() const {
        static_assert(is_registered<SubRegister>, "Must be reading from a registered subregister");
        return SubRegister::read(m_value);
    }

    constexpr Repr value() const {
        return m_value;
    }

private:
    Repr m_value{};
};

template <
    typename NameT, 
    typename ... RegisterListsT>
class registerfile_t
{
public: //private:
    template <typename SubRegister>
    auto& get() { return std::get<index_of<SubRegister>>(registers); }
public:
    using Name = NameT;
    using RegisterLists = cib::tuple<RegisterListsT...>;
    static constexpr Name name{};

    template<typename ... RegisterList>
    constexpr registerfile_t(RegisterList... regs)
    : registers{}
    {
        (set<RegisterList>(regs), ...);
    }

    template<typename Register>
    static constexpr bool is_registered = (false || ... || RegisterListsT::template is_registered<Register>);

    template <typename Register>
    constexpr void set(Register reg)
    {
        registers.template get<Register>() = reg;
    }

    template<typename SubRegister>
    constexpr void write(typename SubRegister::Repr reg)
    {
        static_assert(index_of<SubRegister> < sizeof...(RegisterListsT), "SubRegister not registered to any register list");
        get<SubRegister>().template write<SubRegister>(reg);
    }

    template<typename SubRegister>
    constexpr auto read()
    {
        static_assert(index_of<SubRegister> < sizeof...(RegisterListsT), "SubRegister not registered to any register list");
        return get<SubRegister>().template read<SubRegister>();
    }

    template<typename Register>
    static constexpr std::size_t get_index()
    {
        return 1;
    }

    template<typename Register, typename RegDef, typename ... Rest>
    static constexpr std::size_t get_index()
    {
        if constexpr (RegDef::template is_registered<Register>)
            return 0;
        else
            return 1 + get_index<Register, Rest...>();
    }

    // returns either the index of the 
    template <typename SubRegister>
    static constexpr std::size_t index_of = get_index<SubRegister, RegisterListsT...>();

private: // private members
    RegisterLists registers;
};


} // namespace common