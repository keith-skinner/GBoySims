#pragma once
#include <concepts>
#include <common/arguments.hpp>
#include <variant>
#include "registers.hpp"
#include "names.hpp"

/***
 * The intention of this file is to create a system for the multiple types of arguments that can appear for opcodes.
 * The groups of opcode arguments include:
 *   - An immediate value
 *   - An immediate value that is a reference to an absolute address
 *   - An immediate value that is a reference to a zero-page address
 * 
 *   - A register
 *   - A register that holds a reference to an absolute address
 *   - A register that holds a reference to a zero-page address
*/

//Access is either Immediate or via Reference
namespace LR35902::Access
{

using common::Arguments::Access::a_immediate;
using common::Arguments::Access::a_reference;
template<typename T>
concept a_arg_access = a_immediate<T> || a_reference<T>;

using Immediate = common::Arguments::Access::immediate_t<uint16_t>;
using Reference = common::Arguments::Access::reference_t<uint16_t, 0x0000>;
using zReference = common::Arguments::Access::reference_t<uint16_t, 0xFF00>;


} // namespace LR35902::Access
namespace LR35902::Type
{

template<typename NameT, typename T>
using Immediate = common::Arguments::Type::immediate_t<NameT, T>;

template<typename NameT, std::unsigned_integral TypeT>
using Register = common::Arguments::Type::register_t<NameT, TypeT>;

// Immediate Data Types (S=signed, D=data, A=address)
using S8  = Immediate<Name::Literal::S8, std::int8_t>;
using D8  = Immediate<Name::Literal::D8, std::uint8_t>;
using A8  = Immediate<Name::Literal::A8, std::uint8_t>;
using A16 = Immediate<Name::Literal::A16, std::uint16_t>;
using D16 = Immediate<Name::Literal::D16, std::uint16_t>;

template<typename T>
static constexpr bool is_immediate_v = is_member_of_v<T, S8, D8, A8, A16, D16>;
template<typename T>
concept a_immediate = is_immediate_v<T>;

namespace Flag
{
    using Z = Register<Name::Flag::Z, bool>;
    using N = Register<Name::Flag::N, bool>;
    using H = Register<Name::Flag::H, bool>;
    using C = Register<Name::Flag::C, bool>;
}

template<typename T>
static constexpr bool is_flag_v = is_member_of_v<T, Flag::Z, Flag::N, Flag::H, Flag::C>;
template<typename T>
concept a_flag = is_flag_v<T>;

// 8-Bit registers
using A = Register<Name::Register::A, std::uint8_t>;
using F = Register<Name::Register::F, std::uint8_t>;
using B = Register<Name::Register::B, std::uint8_t>;
using C = Register<Name::Register::C, std::uint8_t>;
using D = Register<Name::Register::D, std::uint8_t>;
using E = Register<Name::Register::E, std::uint8_t>;
using H = Register<Name::Register::H, std::uint8_t>;
using L = Register<Name::Register::L, std::uint8_t>;

template<typename T>
static constexpr bool is_register8_v = is_member_of_v<T, A, F, B, C, D, E, H, L>; 
template<typename T>
concept a_register8 = is_register8_v<T>;

// 16-Bit registers
using AF = Register<Name::Register::AF, std::uint16_t>;
using BC = Register<Name::Register::BC, std::uint16_t>;
using DE = Register<Name::Register::DE, std::uint16_t>;
using HL = Register<Name::Register::HL, std::uint16_t>;
using PC = Register<Name::Register::PC, std::uint16_t>;
using SP = Register<Name::Register::SP, std::uint16_t>;

template<typename T>
static constexpr bool is_register16_v = is_member_of_v<T, AF, BC, DE, HL, PC, SP>;
template<typename T>
concept a_register16 = is_register16_v<T>;

template<typename T>
static constexpr bool is_register_v = is_flag_v<T> || is_register8_v<T> || is_register16_v<T>;
template<typename T>
concept a_register = is_register_v<T>;

template <typename T>
concept a_arg_type = a_immediate<T> || a_register<T>;


} // namespace LR35902::Type
//Type is a merging of Access and Type
namespace LR35902::Args
{

static constexpr std::uint16_t INC = +1;
static constexpr std::uint16_t DEC = -1;

template<LR35902::Type::a_arg_type T, LR35902::Access::a_arg_access A>
using Argument = common::Arguments::argument_t<T, A>;

//LR35902 Specific
// 8-bit register arguments
using A = Argument<Type::A, Access::Immediate>;
using B = Argument<Type::B, Access::Immediate>;
using C = Argument<Type::C, Access::Immediate>;
using D = Argument<Type::D, Access::Immediate>;
using E = Argument<Type::E, Access::Immediate>;
using H = Argument<Type::H, Access::Immediate>;
using L = Argument<Type::L, Access::Immediate>;
using F = Argument<Type::F, Access::Immediate>; // TODO: do i need this?

// 16-bit register arguments
using AF = Argument<Type::AF, Access::Immediate>;
using BC = Argument<Type::BC, Access::Immediate>;
using DE = Argument<Type::DE, Access::Immediate>;
using HL = Argument<Type::HL, Access::Immediate>;
using SP = Argument<Type::SP, Access::Immediate>;
using PC = Argument<Type::PC, Access::Immediate>;

// 8-bit register references
using zC = Argument<Type::C, Access::zReference>; /// 8-bit reference referenced by C (Offset by 0xFF00)

// 16-bit register references
using rBC = Argument<Type::BC, Access::Reference>; //! 16-bit reference referenced by BC
using rDE = Argument<Type::DE, Access::Reference>; //! 16-bit reference referenced by DE
using rHL = Argument<Type::HL, Access::Reference>; //! 16-bit reference referenced by HL
using rPC = Argument<Type::PC, Access::Reference>; //! 16-bit reference referenced by PC
using rSP = Argument<Type::SP, Access::Reference>; //! 16-bit reference referenced by SP

// immediate arguments
using S8  = Argument<Type::S8 , Access::Immediate>; //! 8-bit Signed Immediate offset
using D8  = Argument<Type::D8 , Access::Immediate>; //! 8-bit Immediate Data
using A16 = Argument<Type::A16, Access::Immediate>; //! 16-bit Immediate Address
using D16 = Argument<Type::D16, Access::Immediate>; //! 16-bit Immediate Data

// immediate reference arguments
using zA8  = Argument<Type::A8 , Access::zReference>; //! Memory Referenced by 8-bit address (Offset by 0xFF00)
using rA16 = Argument<Type::A16, Access::Reference>; //! Memory Referenced by 16-bit address

template<typename Arg>
struct to_register {
    using type = Arg::Type;
};

template<typename Arg>
using to_register_v = typename to_register<std::remove_cv_t<Arg>>::type;

} // namespce LR35902::Args
