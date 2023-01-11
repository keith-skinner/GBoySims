#pragma once
#include <type_traits>
#include "arguments.hpp"
#include "registers.hpp"


namespace LR35902
{

template<typename Arg>
struct to_register {  };

// 8 Bit Convert
template<> struct to_register<LR35902::Args::A> { using type = LR35902::A; };
template<> struct to_register<LR35902::Args::B> { using type = LR35902::B; };
template<> struct to_register<LR35902::Args::C> { using type = LR35902::C; };
template<> struct to_register<LR35902::Args::D> { using type = LR35902::D; };
template<> struct to_register<LR35902::Args::E> { using type = LR35902::E; };
template<> struct to_register<LR35902::Args::H> { using type = LR35902::H; };
template<> struct to_register<LR35902::Args::L> { using type = LR35902::L; };
// not including F b/c F is not an argument
template<> struct to_register<LR35902::Args::AF> { using type = LR35902::AF; };
template<> struct to_register<LR35902::Args::BC> { using type = LR35902::BC; };
template<> struct to_register<LR35902::Args::DE> { using type = LR35902::DE; };
template<> struct to_register<LR35902::Args::HL> { using type = LR35902::HL; };
template<> struct to_register<LR35902::Args::SP> { using type = LR35902::SP; };
// not including PC b/c PC is not an argument
template<> struct to_register<LR35902::Args::zC>  { using type = LR35902::C; };
template<> struct to_register<LR35902::Args::rBC> { using type = LR35902::BC; };
template<> struct to_register<LR35902::Args::rDE> { using type = LR35902::DE; };
template<> struct to_register<LR35902::Args::rHL> { using type = LR35902::HL; };
template<> struct to_register<LR35902::Args::rSP> { using type = LR35902::SP; };

template<typename Arg>
using to_register_t = typename to_register<std::remove_cv_t<Arg>>::type;

} // namespace LR35902