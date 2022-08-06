#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace Components
{

struct Recorder
{
    constexpr static bool READ = true;
    constexpr static bool WRITE = false;

    struct Recording
    {
        // true == read and flase == write
        bool oper;
        uint16_t addr;
    };

    uint8_t read(uint16_t addr) 
    { 
        recordings.emplace_back(READ, addr);
        return mem[addr]; 
    }
    void write(uint16_t addr, uint8_t value)
    {
        recordings.emplace_back(WRITE, addr); 
        mem[addr] = value;
    }
    std::vector<Recording> recordings;
    std::array<uint8_t, 0x10000> mem{};
};

bool operator==(const Recorder::Recording& lhs, const Recorder::Recording& rhs)
{
    return lhs.oper == rhs.oper
        && lhs.addr == rhs.addr;
}

}