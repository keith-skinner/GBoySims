#pragma once
#include <cstdint>
#include <array>
#include <utility/bits.hpp>

constexpr auto ROMSIZE = 0x0100; // 0x0000 -> 0x00FF

extern constinit 
const std::array<uint8_t, ROMSIZE> rom;

struct RomComponent
{
    bool read_rom = true;

    std::uint8_t read(const std::uint16_t addr)
    {
        if (!read_rom)
            throw std::runtime_error("should not be reading from rom after rom is turned off");
        return rom.at(addr);
    }
    void write(const std::uint16_t addr, const std::uint8_t data)
    {
        if (!read_rom)
            throw std::runtime_error("should not be reading from rom after rom is turned off");
        else if (addr == 0xFF50)
            read_rom = false;
        else
            throw std::runtime_error("attempted to write to ROM at invalid address" + )
    }
};

