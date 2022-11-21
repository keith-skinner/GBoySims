#pragma once
#include <cstdint>
#include <common/memory.hpp>

namespace LR35902
{

template<typename T>
using ReadModel = common::memory::ref::ReadModel<T, std::uint16_t, std::uint8_t>; 

template<typename T>
using WriteModel = common::memory::ref::WriteModel<T, std::uint16_t, std::uint8_t>;

using MMU = common::memory::ref::Memory<std::uint16_t, std::uint8_t>;


}