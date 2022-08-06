#pragma once
#include <array>
#include <cstdint>

namespace Components
{

struct Simple
{
    uint8_t read(uint16_t addr) const { return mem[addr]; }
    void write(uint16_t addr, uint8_t value) { mem[addr] = value; }
    std::array<uint8_t, 0x10000> mem{};
};

}