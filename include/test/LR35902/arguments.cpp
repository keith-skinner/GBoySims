#include <iostream>
#include <LR35902/endian.hpp>
#include <LR35902/arguments.hpp>

// Testing that nat16_t and sim16_t are explicitly convertable
constexpr auto a = LR35902::nat16_t{5};
constexpr auto b = static_cast<LR35902::sim16_t>(a);
[[maybe_unused]] // proving it can be done
constexpr auto c = static_cast<LR35902::nat16_t>(b);