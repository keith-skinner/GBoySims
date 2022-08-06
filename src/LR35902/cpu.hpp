#pragma once
#include <memory>

#include "opcodes.hpp"
#include "micro.hpp"
#include "arguments.hpp"

namespace LR35902
{

struct Cpu
{

    void run();

    // Debug info
    constexpr std::uint8_t A() const { return regs.read(Register::A{}); }
    constexpr std::uint8_t F() const { return regs.read(Register::F{}); }
    constexpr std::uint8_t B() const { return regs.read(Register::B{}); }
    constexpr std::uint8_t C() const { return regs.read(Register::C{}); }
    constexpr std::uint8_t D() const { return regs.read(Register::D{}); }
    constexpr std::uint8_t E() const { return regs.read(Register::E{}); }
    constexpr std::uint8_t H() const { return regs.read(Register::H{}); }
    constexpr std::uint8_t L() const { return regs.read(Register::L{}); }

    constexpr std::uint16_t /*native endian*/ AF() const { return regs.read(Register::AF{}); }
    constexpr std::uint16_t /*native endian*/ BC() const { return regs.read(Register::BC{}); }
    constexpr std::uint16_t /*native endian*/ DE() const { return regs.read(Register::DE{}); }
    constexpr std::uint16_t /*native endian*/ HL() const { return regs.read(Register::HL{}); }
    constexpr std::uint16_t /*native endian*/ PC() const { return regs.read(Register::PC{}); }
    constexpr std::uint16_t /*native endian*/ SP() const { return regs.read(Register::SP{}); }

    bool ZF() const { return regs.test(LR35902::Registers::Flags::Z); }
    bool NF() const { return regs.test(LR35902::Registers::Flags::N); }
    bool HF() const { return regs.test(LR35902::Registers::Flags::H); }
    bool CF() const { return regs.test(LR35902::Registers::Flags::C); }

private:
    Registers regs;
    micro::Alu alu;
    bool break_ = false;
    Opcode current{};
    uint16_t arg{};
};

}