#pragma once
#include <sc/string_constant.hpp>

namespace LR35902::Name
{

using LR35902 = decltype("LR35902"_sc);

namespace Flag
{
using Z = decltype("Z"_sc);
using N = decltype("N"_sc);
using H = decltype("H"_sc);
using C = decltype("C"_sc);
}

namespace Register
{
using A = decltype("A"_sc);
using F = decltype("F"_sc);
using B = decltype("B"_sc);
using C = decltype("C"_sc);
using D = decltype("D"_sc);
using E = decltype("E"_sc);
using H = decltype("H"_sc);
using L = decltype("L"_sc);

using AF = decltype("AF"_sc);
using BC = decltype("BC"_sc);
using DE = decltype("DE"_sc);
using HL = decltype("HL"_sc);
using PC = decltype("PC"_sc);
using SP = decltype("SP"_sc);
}

namespace Literal
{
using S8  = decltype("S8"_sc);
using A8  = decltype("A8"_sc);
using D8  = decltype("D8"_sc);
using A8  = decltype("A8"_sc);
using A16 = decltype("A16"_sc);
using D16 = decltype("D16"_sc);
} // namespace Literal

} // namespace Register