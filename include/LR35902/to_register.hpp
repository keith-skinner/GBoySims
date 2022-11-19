#pragma once
#include <type_traits>
#include "arguments.hpp"
#include "registers.hpp"


namespace LR35902
{

template<typename Arg>
struct to_register { using value = std::void_t<Arg>; };

// 8 Bit Convert
template<> struct to_register<LR35902::Args::A> { using value = LR35902::A; };
template<> struct to_register<LR35902::Args::B> { using value = LR35902::B; };
template<> struct to_register<LR35902::Args::C> { using value = LR35902::C; };
template<> struct to_register<LR35902::Args::D> { using value = LR35902::D; };
template<> struct to_register<LR35902::Args::E> { using value = LR35902::E; };
template<> struct to_register<LR35902::Args::H> { using value = LR35902::H; };
template<> struct to_register<LR35902::Args::L> { using value = LR35902::L; };
// not including F b/c F is not an argument
template<> struct to_register<LR35902::Args::AF> { using value = LR35902::AF; };
template<> struct to_register<LR35902::Args::BC> { using value = LR35902::BC; };
template<> struct to_register<LR35902::Args::DE> { using value = LR35902::DE; };
template<> struct to_register<LR35902::Args::HL> { using value = LR35902::HL; };
template<> struct to_register<LR35902::Args::SP> { using value = LR35902::SP; };
// not including PC b/c PC is not an argument
template<> struct to_register<LR35902::Args::zC>  { using value = LR35902::C; };
template<> struct to_register<LR35902::Args::rBC> { using value = LR35902::BC; };
template<> struct to_register<LR35902::Args::rDE> { using value = LR35902::DE; };
template<> struct to_register<LR35902::Args::rHL> { using value = LR35902::HL; };
template<> struct to_register<LR35902::Args::rSP> { using value = LR35902::SP; };

template<typename Arg>
using to_register_t = typename to_register<Arg>::value;

} // namespace LR35902