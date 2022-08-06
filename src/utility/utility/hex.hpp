#pragma once
#include <cstdint>
#include <string>

static std::string to_hex(std::uint16_t value)
{
    static constexpr char index[17] = "0123456789ABCDEF";
    auto hex = std::string{4, '0'};
    hex[0] = index[(value & 0xF000u) >> (16 - 4 )];
    hex[1] = index[(value & 0x0F00u) >> (16 - 8 )];
    hex[2] = index[(value & 0x00F0u) >> (16 - 12)];
    hex[3] = index[(value & 0x000Fu) >> (16 - 16)];
    return hex;
}