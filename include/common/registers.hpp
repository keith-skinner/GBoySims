#pragma once
#include <tuple>
#include <cstdint>
#include <type_traits>
#include <cib/tuple.hpp>

namespace common
{
// https://github.com/intel/compile-time-init-build

template <
    typename Name,
    typename Repr,
    std::size_t Msb,
    std::size_t Lsb>
class register_t
{
private:
    Repr value;

public:
    static_assert(Lsb <= Msb, "Msb needs to be lower than or equal to Lsb");

    static constexpr size_t size = (Msb - Lsb) + 1;
    static constexpr size_t repr_size = sizeof(Repr) * 8; /* 8-bit systems only */
    static_assert(size <= repr_size, "Msb must fit within the size of the representation");

    using This = register_t<Name, Repr, Msb, Lsb>;
    using ValueType = Repr;
    static constexpr Name name{};

    constexpr register_t(Repr const & value) : value{value} {}
    constexpr register_t() : value{} {}

    static constexpr uint64_t bit_mask = [](){
        if constexpr (size == 64)
            return 0xFFFFFFFFFFFFFFFF;
        else
            // Move 1 to bit size, make bit size 0 and all other lower bits 1
            return (static_cast<uint64_t>(1) << static_cast<uint64_t>(size)) - static_cast<uint64_t>(1);
    }();
    static constexpr uint64_t reg_mask = bit_mask << Lsb;


    //extract
    template<typename DataStore>
    [[nodiscard]]
    static constexpr Repr read(DataStore const & data)
    {
        return static_cast<Repr>(
            (data >> Lsb) // get relevent data to the lsb of data;
            & bit_mask // remove data that is not relevant
        );
    }

    //insert
    template <typename DataStore>
    constexpr void write(DataStore & data)
    {
        // align value bits with bits to insert value into
        // zero out bits to insert value into and insert value into data
        data = (static_cast<uint64_t>(value) << size) | (data & ~reg_mask);
    }
};


/**
 * @brief Register value that describes a Register and its SubRegisters
 * 
 * @tparam Name 
 * @tparam Repr What representation a register should be 
 * @tparam Width 
 * @tparam SubRegisters 
 */
template <typename Name, typename Repr, std::size_t Width, typename ... SubRegisters>
class registerdef_t {
    Repr value;
public:
    static_assert(Width > 0, "Register must have a Width of at least 1");
    static constexpr std::size_t size = Width;
    static constexpr std::size_t repr_size = sizeof(Repr) * 8;
    static_assert(size <= repr_size, "Repr must be able to contain width");
    static_assert( (true && ... && (SubRegisters::size <= Width)), "SubRegister must be contained within Parent Register" );

    using This = registerdef_t<Name, Repr, Width, SubRegisters...>;
    static constexpr Name name{};
    using SubRegistersTuple = cib::tuple<SubRegisters...>;

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
    static constexpr bool is_valid_register()
    {
        return SubRegistersTuple{}.fold_right(false, [](auto field, bool isValid) {
            return isValid || (std::is_same_v<typename SubRegister::This, typename decltype(field)::This>);
        });
    }

    template <typename SubRegister>
    constexpr void write(SubRegister reg)
    {
        static_assert(is_valid_register<SubRegister>());
        reg.write(this->value);
    }

    template <typename SubRegister>
    constexpr auto read() const {
        static_assert(is_valid_register<SubRegister>());
        return SubRegister::read(this->value);
    }
};

template <typename Name, typename ... Registers>
class registerfile_t {
    std::tuple<Registers...> registers;
public:
    using This = registerfile_t<Name, Registers...>;
    static constexpr Name name{};

    using RegistersTuple = cib::tuple<Registers...>;

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


    template<typename Register>
    [[nodiscard]]
    static constexpr bool is_valid_register()
    {
        return RegistersTuple{}.fold_right(false, [](auto reg, bool isValid) {
            return isValid || reg.is_valid_register(reg);
        });
    }

    template<typename Register>
    static constexpr auto find_first(Register reg)
    {
        auto result = RegistersTuple{}.filter([&](auto test){
            return test.is_valid_register(reg);
        });
    }

    template<typename Register>
    constexpr void write(Register reg)
    {
        static_assert(is_valid_register<Register>());
        std::get<decltype(find_first(reg))>(registers).write(reg);
    }
};

}