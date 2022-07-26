#pragma once
#include "registers.hpp"

namespace LR35902
{

namespace Access
{

struct Imm{};
template<std::uint16_t Offset /*native endian*/>
struct Mem { 
    using addr_t = std::uint16_t;
    static constexpr addr_t offset = Offset; /*native endian*/ 
};
struct  Ref : public Mem<0x0000> {};
struct ZpRef : public Mem<0xFF00> {};

template<typename A> 
static constexpr bool is_imm_v = std::is_same_v<A, Imm>;

template<typename A, typename Addr = A::addr_t, Addr Offset = A::offset> 
static constexpr bool is_ref_v = std::is_same_v<A, Mem<Offset>>;

template<typename A, typename Addr = A::addr_t, Addr Offset = A::offset>
Addr getOffset(const A)
{
    return Offset;
}

}

namespace Type
{
    // If its not an immediate its a register
    template <typename T> struct Imm{ using size_t = T; const T value = {}; };

    // Immediate Data
    using S8  = Imm<int8_t>;
    using D8  = Imm<uint8_t>;
    using A8  = Imm<uint8_t>;
    using A16 = Imm<uint16_t>;
    
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
}

namespace Args
{

template<typename T, typename A>
struct Argument { 
    using Type = T; 
    using Access = A;
    const Type type = {};
    const Access access = {};
};

struct A  : public Argument<Type::A, Access::Imm>   {};
struct F  : public Argument<Type::F, Access::Imm>   {};
struct B  : public Argument<Type::B, Access::Imm>   {};
struct C  : public Argument<Type::C, Access::Imm>   {};
struct D  : public Argument<Type::D, Access::Imm>   {};
struct E  : public Argument<Type::E, Access::Imm>   {};
struct H  : public Argument<Type::H, Access::Imm>   {};
struct L  : public Argument<Type::L, Access::Imm>   {};
struct zC : public Argument<Type::C, Access::ZpRef> {};

struct AF  : public Argument<Type::AF, Access::Imm> {};
struct BC  : public Argument<Type::BC, Access::Imm> {};
struct DE  : public Argument<Type::DE, Access::Imm> {};
struct HL  : public Argument<Type::HL, Access::Imm> {};
struct SP  : public Argument<Type::SP, Access::Imm> {};
struct rBC : public Argument<Type::BC, Access::Ref> {};
struct rDE : public Argument<Type::DE, Access::Ref> {};
struct rHL : public Argument<Type::HL, Access::Ref> {};
struct rPC : public Argument<Type::PC, Access::Ref> {};
struct rSP : public Argument<Type::SP, Access::Ref> {};

struct S8   : public Argument<Type::S8 , Access::Imm>  {};
struct D8   : public Argument<Type::D8 , Access::Imm>  {};
struct zA8   : public Argument<Type::A8 , Access::ZpRef>{};
struct A16  : public Argument<Type::A16, Access::Imm>  {};
struct rA16 : public Argument<Type::A16, Access::Ref>  {};

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

template <typename T>
static constexpr bool is_std_arg_v = 
       is_reg8_v<T>
    || std::is_same_v<T, rHL>;

template<typename T>
static constexpr bool is_reg16_v =
       std::is_same_v<T, AF>
    || std::is_same_v<T, BC>
    || std::is_same_v<T, DE>
    || std::is_same_v<T, HL>
    || std::is_same_v<T, SP>;
}

}
