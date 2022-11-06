#pragma once
#include <common/endian_integer.hpp>

namespace LR35902
{

using sim16_t = common::uint16e_t<std::endian::big>;
using nat16_t = common::uint16e_t<std::endian::native>;

}