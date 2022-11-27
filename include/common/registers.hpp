#pragma once
#include <tuple>
#include <cstdint>
#include <type_traits>
#include <cib/tuple.hpp>
#include <algorithm>

namespace common
{

// https://github.com/intel/compile-time-init-build

template <
    typename NameT,
    typename ReprT,
    std::size_t MsbV,
    std::size_t LsbV>
class register_t
{
private:
    ReprT m_value;
public:
    using This = register_t<NameT, ReprT, MsbV, LsbV>;
    using Name = NameT;
    using Repr = ReprT;
    static constexpr Name name{};
    static constexpr std::size_t msb = MsbV;
    static constexpr std::size_t lsb = LsbV;
    static constexpr std::size_t width = msb - lsb + 1;
    static constexpr std::size_t repr_width = sizeof(Repr) * 8;

    static_assert(lsb <= msb, "Msb needs to be lower than or equal to Lsb");
    static_assert(width <= repr_width, "Msb must fit within the size of the representation");

    constexpr register_t(Repr const & value) noexcept : m_value{value} {}
    constexpr register_t() noexcept = default;
    constexpr register_t(const register_t& other) noexcept = default;
    constexpr register_t(register_t&& other) noexcept = default;
    constexpr register_t& operator=(const register_t&) noexcept = default;
    constexpr register_t& operator=(register_t&&) noexcept  = default;

    static constexpr uint64_t bit_mask = [](){
        if constexpr (width == 64)
            return 0xFFFFFFFFFFFFFFFF;
        else
            // Move 1 to bit size, make bit size 0 and all other lower bits 1
            // E.g. Msb=7 & Lsb=4 -> width=4; shift 1 by 4; 0b10000; -1; 0b01111;
            // E.g. Msb=0 & Lsb=0 -> width=1; shift 1 by 1; 0b00010; -1; 0b00001;
            return (static_cast<uint64_t>(1) << static_cast<uint64_t>(width)) - static_cast<uint64_t>(1);
    }();
    static constexpr uint64_t reg_mask = bit_mask << lsb;


    //extract
    template<typename DataStore>
    [[nodiscard]]
    static constexpr Repr read(DataStore const & data)
    {
        return (data & reg_mask) >> lsb;
    }

    //insert
    template <std::unsigned_integral DataStore>
    constexpr void write(DataStore & data)
    {
        // align value bits with bits to insert value into
        // zero out bits to insert value into and insert value into data
        data = (static_cast<uint64_t>(m_value) << lsb) | (data & ~reg_mask);
    }

    constexpr ReprT & value() { return m_value; }
    constexpr ReprT value() const { return m_value; }
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
private:
    ReprT value;
public:
    using This = registerdef_t<NameT, ReprT, WidthV, SubRegisters...>;
    using Name = NameT;
    using Repr = ReprT;
    using SubRegistersTuple = cib::tuple<SubRegisters...>;
    static constexpr Name name{};
    static constexpr std::size_t repr_width = sizeof(Repr) * 8;
    static constexpr std::size_t width = WidthV;


    static_assert(width > 0, "Register must have a Width of at least 1");
    static_assert(width <= repr_width, "Repr must be able to contain width");
    static_assert( (true && ... && (SubRegisters::width <= width)), "SubRegister must be contained within Parent Register width" );


    constexpr registerdef_t(const registerdef_t& reg): value{reg.value} {}
    constexpr registerdef_t(registerdef_t && reg) : value {reg.value} {}
    constexpr registerdef_t & operator = (const registerdef_t & reg) { value = reg.value; return *this; }
    constexpr registerdef_t & operator = (registerdef_t && reg) { value = reg.value; return *this; }
    constexpr registerdef_t(Repr value): value{value} {}
    template<typename ... SubRegisterTypes>
    constexpr registerdef_t(SubRegisterTypes... subregisters)
    : value {}
    {
        if constexpr (sizeof...(subregisters) == 0)
            SubRegistersTuple{}.for_each([&](auto field){ write(field); });
        else {
            SubRegistersTuple{}.for_each([&](auto field){ write(field); });
            cib::make_tuple(subregisters...).for_each([&](auto field){ write(field); });
        }
    }

    /**
     * @brief Is this SubRegister in the list of SubRegisters...
     *
     * @tparam SubRegister
     * @return true Subregister is a member of SubRegisters...
     * @return false SubRegister is not a member of SubRegisters...
     */
    template<typename SubRegister>
    [[nodiscard]]
    static constexpr bool is_valid_register(SubRegister = SubRegister{})
    {
        //spdlog::info("is_valid_register: {}", SubRegister::name);
        return SubRegistersTuple{}.fold_right(false, [](auto field, bool isValid) {
            return ( isValid || std::is_same_v<SubRegister, decltype(field)> );
        });
    }

    template <typename SubRegister>
    constexpr void write(SubRegister reg)
    {
        static_assert(is_valid_register<SubRegister>(),
            "Must be writing to a registered subregister");
        reg.write(this->value);
    }

    template <typename SubRegister>
    constexpr auto read() const {
        static_assert(is_valid_register<SubRegister>(),
            "Must be reading from a registered subregister");
        return SubRegister::read(this->value);
    }
};

template <typename NameT, typename ... Registers>
class registerfile_t {
    std::tuple<Registers...> registers;
public:
    using This = registerfile_t<NameT, Registers...>;
    using Name = NameT;
    static constexpr Name name{};

    using RegisterDefTuple = cib::tuple<Registers...>;

    template<typename ... RegisterList>
    constexpr registerfile_t(RegisterList... regs)
    : registers{}
    {
        (set<RegisterList>(regs), ...);
    }

    template <typename R>
    constexpr void set(R reg)
    {
        std::get<R>(registers) = reg;
    }

    template<typename SubRegister>
    static constexpr bool is_valid_register([[maybe_unused]] SubRegister r = SubRegister{})
    {
        return RegisterDefTuple{}.fold_right(false, [](auto field, bool isValid) {
            return isValid || field.template is_valid_register<SubRegister>();
        });
    }

    template<typename SubRegister>
    static constexpr std::size_t get_index([[maybe_unused]]SubRegister r = SubRegister{})
    {
        static_assert(is_valid_register<SubRegister>(), "SubRegister not defined for this register file");
        return std::get<1>(RegisterDefTuple{}.fold_left(std::make_tuple(false, 0), [](auto accumulator, auto field) {
            if (std::get<0>(accumulator))
                return accumulator; // already found so skip
            else if (decltype(field)::template is_valid_register<decltype(r)>())
                return std::make_tuple(true, std::get<1>(accumulator));
            else
                return std::make_tuple(false, ++std::get<1>(accumulator));
        }));
    }

    template<typename SubRegister>
    constexpr void write(SubRegister reg)
    {
        static_assert(is_valid_register<SubRegister>(),
            "Must be writing to a registered subregister");
        std::get<( get_index<SubRegister>() )>(registers).write(reg);
    }

    template<typename SubRegister>
    constexpr auto read([[maybe_unused]] SubRegister r = SubRegister{})
    {
        static_assert(is_valid_register<SubRegister>(),
            "Must be reading from a registered subregister");
        return std::get<( get_index<SubRegister>() )>(registers).template read<SubRegister>();
    }

    template<typename ... FlagRegisters>
    constexpr void flags(FlagRegisters ... rflags)
    {
        // do nothing if empty
        if constexpr (sizeof...(FlagRegisters) == 0)
            return;
        static_assert((true && ... && is_valid_register<FlagRegisters>()),
            "FlagRegisters must be registered to this register file");
        (write(rflags), ...);
    }
};


} // namespace common