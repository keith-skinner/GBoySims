#pragma once
#include "registers.hpp"


namespace LR35902
{

namespace Access
{

struct Immediate{
    using addr_t = std::uint16_t;
    static constexpr addr_t offset = 0x0000;
};
template<std::uint16_t Offset /*native endian*/>
struct Mem { 
    using addr_t = std::uint16_t;
    static constexpr addr_t offset = Offset; /*native endian*/ 
};
struct  Reference : public Mem<0x0000> {};
struct zReference : public Mem<0xFF00> {};

template<typename A> 
static constexpr bool is_imm_v = std::is_same_v<A, Immediate>;

template<typename T>
concept Imm = is_imm_v<T>;

template<typename T> 
static constexpr bool is_ref_v = 
       std::is_same_v<T, Reference> 
    || std::is_same_v<T, zReference>;

template<typename T>
concept Ref = is_ref_v<T>;

template<typename A, typename Addr = A::addr_t, Addr Offset = A::offset>
Addr getOffset(const A)
{
    return Offset;
}

}

namespace Type
{
    // If its not an immediate its a register
    template <typename T> struct Immediate{ using size_t = T; const T value = {}; };

    // Immediate Data
    using S8  = Immediate<int8_t>;
    using D8  = Immediate<uint8_t>;
    using A8  = Immediate<uint8_t>;
    using A16 = Immediate<uint16_t>;
    using D16 = Immediate<uint16_t>;

    template<typename T, typename S = T::size_t>
    static constexpr bool is_imm_v = std::is_same_v<T, Immediate<S>>;
    
    template<typename T>
    concept Imm = is_imm_v<T>;
    
    // 8-Bit registers
    using Register::A;
    using Register::F;
    using Register::B;
    using Register::C;
    using Register::D;
    using Register::E;
    using Register::H;
    using Register::L;

    // 16-Bit registers
    using Register::AF;
    using Register::BC;
    using Register::DE;
    using Register::HL;
    using Register::PC;
    using Register::SP;

    using Register::is_register8_v;
    using Register::is_register16_v;
    using Register::is_register_v;

    template<typename T>
    concept Reg8 = is_register8_v<T>;
    template<typename T>
    concept Reg16 = is_register16_v<T>;
    template<typename T>
    concept Reg = Reg8<T> || Reg16<T>;
}

namespace Args
{

template<typename T, typename A>
struct Argument { 
    using Type = T; 
    using Access = A;
    using size_t = Type::size_t;

    template<typename A2>
    explicit constexpr Argument(const Argument<T, A2>& other)
    : type{other.type}
    , access{}
    {}

    constexpr Argument(const size_t value)
    : type{value}
    , access{}
    {}

    constexpr Argument() = default;

    const Type type = {};
    const Access access = {};
};

struct A  : public Argument<Type::A, Access::Immediate>  {};
struct F  : public Argument<Type::F, Access::Immediate>  {};
struct B  : public Argument<Type::B, Access::Immediate>  {};
struct C  : public Argument<Type::C, Access::Immediate>  {};
struct D  : public Argument<Type::D, Access::Immediate>  {};
struct E  : public Argument<Type::E, Access::Immediate>  {};
struct H  : public Argument<Type::H, Access::Immediate>  {};
struct L  : public Argument<Type::L, Access::Immediate>  {};
struct zC : public Argument<Type::C, Access::zReference> {};

struct AF  : public Argument<Type::AF, Access::Immediate> {};
struct BC  : public Argument<Type::BC, Access::Immediate> {};
struct DE  : public Argument<Type::DE, Access::Immediate> {};
struct HL  : public Argument<Type::HL, Access::Immediate> {};
struct SP  : public Argument<Type::SP, Access::Immediate> {};
struct PC  : public Argument<Type::PC, Access::Immediate> {};
struct rBC : public Argument<Type::BC, Access::Reference> {};
struct rDE : public Argument<Type::DE, Access::Reference> {};
struct rHL : public Argument<Type::HL, Access::Reference> {};
struct rPC : public Argument<Type::PC, Access::Reference> {};
struct rSP : public Argument<Type::SP, Access::Reference> {};

struct S8   : public Argument<Type::S8 , Access::Immediate>  {};
struct D8   : public Argument<Type::D8 , Access::Immediate>  {};
struct zA8  : public Argument<Type::A8 , Access::zReference> {};
struct A16  : public Argument<Type::A16, Access::Immediate>  {};
struct rA16 : public Argument<Type::A16, Access::Reference>  {};
struct D16  : public Argument<Type::D16, Access::Immediate>  {};

template<typename T>
static constexpr bool is_reg8_v = 
       std::is_same_v<T, A>
    || std::is_same_v<T, F>
    || std::is_same_v<T, B>
    || std::is_same_v<T, C>
    || std::is_same_v<T, D>
    || std::is_same_v<T, E>
    || std::is_same_v<T, H>
    || std::is_same_v<T, L>;

template<typename T>
concept Reg8 = is_reg8_v<T>;

template <typename T>
static constexpr bool is_std_arg_v = 
       is_reg8_v<T>
    || std::is_same_v<T, rHL>;

template<typename T>
concept StdArg = is_std_arg_v<T>;

template<typename T>
concept StdArgImm = StdArg<T> || std::is_same_v<T, D8>;

template<typename T>
static constexpr bool is_reg16_v =
       std::is_same_v<T, AF>
    || std::is_same_v<T, BC>
    || std::is_same_v<T, DE>
    || std::is_same_v<T, HL>
    || std::is_same_v<T, SP>;

template<typename T>
concept Reg16 = is_reg16_v<T>;

template<typename T>
concept Reg = Reg8<T> || Reg16<T>;

template<typename T>
static constexpr bool is_ref_t =
       std::is_same_v<T, zA8>
    || std::is_same_v<T, rA16>
    || std::is_same_v<T, zC>
    || std::is_same_v<T, rBC>
    || std::is_same_v<T, rDE>
    || std::is_same_v<T, rHL>
    || std::is_same_v<T, rPC>
    || std::is_same_v<T, rSP>;

template<typename T>
concept Ref = is_ref_t<T>;

template<typename T>
concept Dst = Reg8<T> || Reg16<T> || Ref<T>;

template<typename T>
concept Arg = Dst<T>
    || std::is_same_v<T, S8>
    || std::is_same_v<T, D8>
    || std::is_same_v<T, A16>
    || std::is_same_v<T, D16>;

}

}
