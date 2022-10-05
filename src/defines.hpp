#pragma once
#include <cstdint>

namespace GBOYSIM
{
    using addr_t = uint16_t;
    using data_t = uint8_t;
}

consteval GBOYSIM::addr_t operator "" _a(unsigned long long int addr) { return addr; }
consteval GBOYSIM::data_t operator "" _d(unsigned long long int data) { return data; }