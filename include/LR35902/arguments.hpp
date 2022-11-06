#pragma once
#include <common/arguments.hpp>
#include "endian.hpp"

/***
 * The intention of this file is to create a system for the multiple types of arguments that can appear for opcodes
 *   - An immediate that is not a reference to an address
 *   - An immediate that is a reference to an absoulte address
 *   - An immediate that is a reference to a zero-page address
 *   - A register that is a not a reference to an address
 *   - A register that is a reference to an absolute address
 *   - A register that is a reference to a zero-page address
*/

//Access is either Immediate or via Reference
namespace LR35902::Access
{


using common::Arguments::Access::a_immediate;
using common::Arguments::Access::a_reference;
using common::Arguments::Access::a_arg_access;

using Immediate = common::Arguments::Access::immediate_t<nat16_t>;
template<nat16_t::Type Offset>
using Memory = common::Arguments::Access::memory_t<nat16_t, nat16_t::Type, Offset>;
using Reference = Memory<0x0000>;
using zReference = Memory<0xFF00>;


} // namespace LR35902::Access
//Type is either Immediate or via Register
namespace LR35902::Type
{


template<typename T>
using Immediate = common::Arguments::Type::immediate_t<T>;
using common::Arguments::Type::a_immediate;

enum class Name : std::uint8_t
{
    A, F, B, C, D, E, H, L,
    AF, BC, DE, HL, PC, SP,
};

template<Name R, std::unsigned_integral T>
struct Register
:   common::Arguments::Type::register_t<T>
{
    static constexpr Name name = R;
    using common::Arguments::Type::register_t<T>::Type;
};

// Immediate Data Types
using S8  = Immediate<std::int8_t>;
using D8  = Immediate<std::uint8_t>;
using A8  = Immediate<std::uint8_t>;
using A16 = Immediate<std::uint16_t>;
using D16 = Immediate<std::uint16_t>;

// 8-Bit dregisters
using A = Register<Name::A, std::uint8_t>;
using F = Register<Name::F, std::uint8_t>;
using B = Register<Name::B, std::uint8_t>;
using C = Register<Name::C, std::uint8_t>;
using D = Register<Name::D, std::uint8_t>;
using E = Register<Name::E, std::uint8_t>;
using H = Register<Name::H, std::uint8_t>;
using L = Register<Name::L, std::uint8_t>;

// 16-Bit registers
using AF = Register<Name::AF, std::uint16_t>;
using BC = Register<Name::BC, std::uint16_t>;
using DE = Register<Name::DE, std::uint16_t>;
using HL = Register<Name::HL, std::uint16_t>;
using PC = Register<Name::PC, std::uint16_t>;
using SP = Register<Name::SP, std::uint16_t>;

template<typename T, Name name=T::name>
static constexpr bool is_register8_v = std::is_same_v<T, Register<name, std::uint8_t>>;
template<typename T>
concept a_register8 = is_register8_v<T>;

template<typename T, Name name=T::name>
static constexpr bool is_register16_v = std::is_same_v<T, Register<name, std::uint16_t>>;
template<typename T>
concept a_register16 = is_register16_v<T>;

template<typename T>
concept a_register = a_register8<T> || a_register16<T>;

template <typename T>
concept a_arg_type = a_immediate<T> || a_register<T>;


} // namespace LR35902::Type
//Type is a merging of Access and Type
namespace LR35902::Args
{


template<LR35902::Type::a_arg_type T, LR35902::Access::a_arg_access A>
using Argument = common::Arguments::argument_t<T, A>;

//LR35902 Specific
// 8-bit register arguments
struct A  : public Argument<Type::A, Access::Immediate>  {};
struct F  : public Argument<Type::F, Access::Immediate>  {};
struct B  : public Argument<Type::B, Access::Immediate>  {};
struct C  : public Argument<Type::C, Access::Immediate>  {};
struct D  : public Argument<Type::D, Access::Immediate>  {};
struct E  : public Argument<Type::E, Access::Immediate>  {};
struct H  : public Argument<Type::H, Access::Immediate>  {};
struct L  : public Argument<Type::L, Access::Immediate>  {};
struct zC : public Argument<Type::C, Access::zReference> {};

// 16-bit register arguments
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

// data arguments
struct S8   : public Argument<Type::S8 , Access::Immediate>  {};
struct D8   : public Argument<Type::D8 , Access::Immediate>  {};
struct zA8  : public Argument<Type::A8 , Access::zReference> {};
struct A16  : public Argument<Type::A16, Access::Immediate>  {};
struct rA16 : public Argument<Type::A16, Access::Reference>  {};
struct D16  : public Argument<Type::D16, Access::Immediate>  {};

template<typename T>
static constexpr bool is_immreg8_v =
       std::is_same_v<T, A>
    || std::is_same_v<T, F>
    || std::is_same_v<T, B>
    || std::is_same_v<T, C>
    || std::is_same_v<T, D>
    || std::is_same_v<T, E>
    || std::is_same_v<T, H>
    || std::is_same_v<T, L>;
template<typename T>
concept a_immreg8 = is_immreg8_v<T>;

template<typename T>
static constexpr bool is_immreg16_v =
       std::is_same_v<T, AF>
    || std::is_same_v<T, BC>
    || std::is_same_v<T, DE>
    || std::is_same_v<T, HL>
    || std::is_same_v<T, SP>;
template<typename T>
concept a_immreg16_v = is_immreg16_v<T>;



template <typename T>
static constexpr bool is_std_arg_v =
       is_immreg8_v<T>
    || std::is_same_v<T, rHL>;
template <typename T>
concept a_std_arg = is_std_arg_v<T>;

template<typename T>
static constexpr bool is_reference_v =
       std::is_same_v<T, zA8>
    || std::is_same_v<T, rA16>
    || std::is_same_v<T, zC>
    || std::is_same_v<T, rBC>
    || std::is_same_v<T, rDE>
    || std::is_same_v<T, rHL>
    || std::is_same_v<T, rPC>
    || std::is_same_v<T, rSP>;
template<typename T>
concept a_reference = is_reference_v<T>;


} // namespce LR35902::Args
