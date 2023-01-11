#pragma once
#include <string_view>
#include <optional>
#include <array>
#include <cstdint>

namespace LR35902
{

struct Opcode {
    const std::uint16_t code;
    bool CB;
    const std::string_view name;
    const int length;
    const int cycles_action;
    const int cycles_no_action = 0;
};

template<typename Arg1 = std::void_t<>, typename Arg2 = std::void_t<>, typename Arg3 = std::void_t<>>
struct OpcodeDef 
{
    using arg1 = Arg1;
    using arg2 = Arg2;
    using arg3 = Arg3;
};

// TODO: Remove name from Opcodes and only include when debugging.

// (C) means to derefference the offset given by C from the base address 0xFF00
// (a8) means to derefference the offset given by a8 (an immediate) from the base address 0xFF00
// (BC) means to derefference the address given by BC or other 16-bit registers
// (a16) means to dereferrence the address given by a16 (an immediate)
// (HL+) means to dereferrence the addresss given by HL and then increment HL
// (HL-) means to dereferrence the addresss given by HL and then decrement HL
constexpr std::array<Opcode, 256> opcodes {{
    {0x00, false, "NOP", 1, 4},
    {0x01, false, "LD BC, d16", 3, 12},
    {0x02, false, "LD (BC), A", 1, 8},
    {0x03, false, "INC BC", 1, 8},
    {0x04, false, "INC B", 1, 4},
    {0x05, false, "DEC B", 1, 4},
    {0x06, false, "LD B, d8", 2, 8},
    {0x07, false, "RLCA", 1, 4},
    {0x08, false, "LD (a16), SP", 3, 20},
    {0x09, false, "ADD HL, BC", 1, 8},
    {0x0A, false, "LD A, (BC)", 1, 8},
    {0x0B, false, "DEC BC", 1, 8},
    {0x0C, false, "INC C", 1, 4},
    {0x0D, false, "DEC C", 1, 4},
    {0x0E, false, "LD C, d8", 2, 8},
    {0x0F, false, "RRCA", 1, 4},
    //
    {0x10, false, "STOP d8", 2, 4},
    {0x11, false, "LD DE, d16", 3, 12},
    {0x12, false, "LD (DE), A", 1, 8},
    {0x13, false, "INC DE", 1, 8},
    {0x14, false, "INC D", 1, 4},
    {0x15, false, "DEC D", 1, 4},
    {0x16, false, "LD D, d8", 2, 8},
    {0x17, false, "RLA", 1, 4},
    {0x18, false, "JR r8", 2, 12},
    {0x19, false, "ADD HL, DE", 1, 8},
    {0x1A, false, "LD A, (DE)", 1, 8},
    {0x1B, false, "DEC DE", 1, 8},
    {0x1C, false, "INC E", 1, 4},
    {0x1D, false, "DEC E", 1, 4},
    {0x1E, false, "LD E, d8", 2, 8},
    {0x1F, false, "RRA", 1, 4},
    //
    {0x20, false, "JR NZ, s8", 2, 12, 8},
    {0x21, false, "LD HL, d16", 3, 12},
    {0x22, false, "LD (HL+), A", 1, 8},
    {0x23, false, "INC HL", 1, 8},
    {0x24, false, "INC H", 1, 4},
    {0x25, false, "DEC H", 1, 4},
    {0x26, false, "LD H, d8", 2, 8},
    {0x27, false, "DAA", 1, 4},
    {0x28, false, "JR Z, s8", 2, 12, 8},
    {0x29, false, "ADD HL, HL", 1, 8},
    {0x2A, false, "LD A, (HL+)", 1, 8},
    {0x2B, false, "DEC HL", 1, 8},
    {0x2C, false, "INC L", 1, 4},
    {0x2D, false, "DEC L", 1, 4},
    {0x2E, false, "LD L, d8", 2, 8},
    {0x2F, false, "CPL", 1, 4},
    //
    {0x30, false, "JR NC, s8", 2, 12, 8},
    {0x31, false, "LD SP, d16", 3, 12},
    {0x32, false, "LD (HL-), A", 1, 8},
    {0x33, false, "INC SP", 1, 8},
    {0x34, false, "INC (HL)", 1, 12},
    {0x35, false, "DEC (HL)", 1, 12},
    {0x36, false, "LD (HL), d8", 2, 12},
    {0x37, false, "SCF", 1, 4},
    {0x38, false, "JR C, s8", 2, 12, 8},
    {0x39, false, "ADD HL, SP", 1, 8},
    {0x3A, false, "LD A, (HL-)", 1, 8},
    {0x3B, false, "DEC SP", 1, 8},
    {0x3C, false, "INC A", 1, 4},
    {0x3D, false, "DEC A", 1, 4},
    {0x3E, false, "LD A, d8", 2, 8},
    {0x3F, false, "CCF", 1, 4},
    //
    {0x40, false, "LD B, B", 1, 4},
    {0x41, false, "LD B, C", 1, 4},
    {0x42, false, "LD B, D", 1, 4},
    {0x43, false, "LD B, E", 1, 4},
    {0x44, false, "LD B, H", 1, 4},
    {0x45, false, "LD B, L", 1, 4},
    {0x46, false, "LD B, (HL)", 1, 8},
    {0x47, false, "LD B, A", 1, 4},
    {0x48, false, "LD C, B", 1, 4},
    {0x49, false, "LD C, C", 1, 4},
    {0x4A, false, "LD C, D", 1, 4},
    {0x4B, false, "LD C, E", 1, 4},
    {0x4C, false, "LD C, H", 1, 4},
    {0x4D, false, "LD C, L", 1, 4},
    {0x4E, false, "LD C, (HL)", 1, 8},
    {0x4F, false, "LD C, A", 1, 4},
    //
    {0x50, false, "LD D, B", 1, 4},
    {0x51, false, "LD D, C", 1, 4},
    {0x52, false, "LD D, D", 1, 4},
    {0x53, false, "LD D, E", 1, 4},
    {0x54, false, "LD D, H", 1, 4},
    {0x55, false, "LD D, L", 1, 4},
    {0x56, false, "LD D, (HL)", 1, 8},
    {0x57, false, "LD D, A", 1, 4},
    {0x58, false, "LD E, B", 1, 4},
    {0x59, false, "LD E, C", 1, 4},
    {0x5A, false, "LD E, D", 1, 4},
    {0x5B, false, "LD E, E", 1, 4},
    {0x5C, false, "LD E, H", 1, 4},
    {0x5D, false, "LD E, L", 1, 4},
    {0x5E, false, "LD E, (HL)", 1, 8},
    {0x5F, false, "LD E, A", 1, 4},
    //
    {0x60, false, "LD H, B", 1, 4},
    {0x61, false, "LD H, C", 1, 4},
    {0x62, false, "LD H, D", 1, 4},
    {0x63, false, "LD H, E", 1, 4},
    {0x64, false, "LD H, H", 1, 4},
    {0x65, false, "LD H, L", 1, 4},
    {0x66, false, "LD H, (HL)", 1, 8},
    {0x67, false, "LD H, A", 1, 4},
    {0x68, false, "LD L, B", 1, 4},
    {0x69, false, "LD L, C", 1, 4},
    {0x6A, false, "LD L, D", 1, 4},
    {0x6B, false, "LD L, E", 1, 4},
    {0x6C, false, "LD L, H", 1, 4},
    {0x6D, false, "LD L, L", 1, 4},
    {0x6E, false, "LD L, (HL)", 1, 8},
    {0x6F, false, "LD L, A", 1, 4},
    //
    {0x70, false, "LD (HL), B", 1, 8},
    {0x71, false, "LD (HL), C", 1, 8},
    {0x72, false, "LD (HL), D", 1, 8},
    {0x73, false, "LD (HL), E", 1, 8},
    {0x74, false, "LD (HL), H", 1, 8},
    {0x75, false, "LD (HL), L", 1, 8},
    {0x76, false, "HALT", 1, 4},
    {0x77, false, "LD (HL), A", 1, 8},
    {0x78, false, "LD A, B", 1, 4},
    {0x79, false, "LD A, C", 1, 4},
    {0x7A, false, "LD A, D", 1, 4},
    {0x7B, false, "LD A, E", 1, 4},
    {0x7C, false, "LD A, H", 1, 4},
    {0x7D, false, "LD A, L", 1, 4},
    {0x7E, false, "LD A, (HL)", 1, 8},
    {0x7F, false, "LD A, A", 1, 4},
    //
    {0x80, false, "ADD A, B", 1, 4},
    {0x81, false, "ADD A, C", 1, 4},
    {0x82, false, "ADD A, D", 1, 4},
    {0x83, false, "ADD A, E", 1, 4},
    {0x84, false, "ADD A, H", 1, 4},
    {0x85, false, "ADD A, L", 1, 4},
    {0x86, false, "ADD A, (HL)", 1, 8},
    {0x87, false, "ADD A, A", 1, 4},
    {0x88, false, "ADC A, B", 1, 4},
    {0x89, false, "ADC A, C", 1, 4},
    {0x8A, false, "ADC A, D", 1, 4},
    {0x8B, false, "ADC A, E", 1, 4},
    {0x8C, false, "ADC A, H", 1, 4},
    {0x8D, false, "ADC A, L", 1, 4},
    {0x8E, false, "ADC A, (HL)", 1, 8},
    {0x8F, false, "ADC A, A", 1, 4},
    //
    {0x90, false, "SUB B", 1, 4},
    {0x91, false, "SUB C", 1, 4},
    {0x92, false, "SUB D", 1, 4},
    {0x93, false, "SUB E", 1, 4},
    {0x94, false, "SUB H", 1, 4},
    {0x95, false, "SUB L", 1, 4},
    {0x96, false, "SUB (HL)", 1, 8},
    {0x97, false, "SUB A", 1, 4},
    {0x98, false, "SBC A, B", 1, 4},
    {0x99, false, "SBC A, C", 1, 4},
    {0x9A, false, "SBC A, D", 1, 4},
    {0x9B, false, "SBC A, E", 1, 4},
    {0x9C, false, "SBC A, H", 1, 4},
    {0x9D, false, "SBC A, L", 1, 4},
    {0x9E, false, "SBC A, (HL)", 1, 8},
    {0x9F, false, "SBC A, A", 1, 4},
    //
    {0xA0, false, "AND B", 1, 4},
    {0xA1, false, "AND C", 1, 4},
    {0xA2, false, "AND D", 1, 4},
    {0xA3, false, "AND E", 1, 4},
    {0xA4, false, "AND H", 1, 4},
    {0xA5, false, "AND L", 1, 4},
    {0xA6, false, "AND (HL)", 1, 8},
    {0xA7, false, "AND A", 1, 4},
    {0xA8, false, "XOR B", 1, 4},
    {0xA9, false, "XOR C", 1, 4},
    {0xAA, false, "XOR D", 1, 4},
    {0xAB, false, "XOR E", 1, 4},
    {0xAC, false, "XOR H", 1, 4},
    {0xAD, false, "XOR L", 1, 4},
    {0xAE, false, "XOR (HL)", 1, 8},
    {0xAF, false, "XOR A", 1, 4},
    //
    {0xB0, false, "OR B", 1, 4},
    {0xB1, false, "OR C", 1, 4},
    {0xB2, false, "OR D", 1, 4},
    {0xB3, false, "OR E", 1, 4},
    {0xB4, false, "OR H", 1, 4},
    {0xB5, false, "OR L", 1, 4},
    {0xB6, false, "OR (HL)", 1, 8},
    {0xB7, false, "OR A", 1, 4},
    {0xB8, false, "CP B", 1, 4},
    {0xB9, false, "CP C", 1, 4},
    {0xBA, false, "CP D", 1, 4},
    {0xBB, false, "CP E", 1, 4},
    {0xBC, false, "CP H", 1, 4},
    {0xBD, false, "CP L", 1, 4},
    {0xBE, false, "CP (HL)", 1, 8},
    {0xBF, false, "CP A", 1, 4},
    //
    {0xC0, false, "RET NZ", 1, 20, 8},
    {0xC1, false, "POP BC", 1, 12},
    {0xC2, false, "JP NZ, a16", 3, 16, 12},
    {0xC3, false, "JP a16", 3, 16},
    {0xC4, false, "CALL NZ, a16", 3, 24, 12},
    {0xC5, false, "PUSH BC", 1, 16},
    {0xC6, false, "ADD A, d8", 2, 8},
    {0xC7, false, "RST 00H", 1, 16},
    {0xC8, false, "RET Z", 1, 20, 8},
    {0xC9, false, "RET", 1, 16},
    {0xCA, false, "JP Z, a16", 3, 16, 12},
    {0xCB, false, "PREFIX", 1, 4},
    {0xCC, false, "CALL Z, a16", 3, 24, 12},
    {0xCD, false, "CALL a16", 3, 24},
    {0xCE, false, "ADC A, d8", 2, 8},
    {0xCF, false, "RST 08H", 1, 16},
    //
    {0xD0, false, "RET NC", 1, 20, 8},
    {0xD1, false, "POP DE", 1, 12},
    {0xD2, false, "JP NC, a16", 3, 16, 12},
    {0xD3, false, "ILLEGAL_D3", 1, 4},
    {0xD4, false, "CALL NC, a16", 3, 24, 12},
    {0xD5, false, "PUSH DE", 1, 16},
    {0xD6, false, "SUB d8", 2, 8},
    {0xD7, false, "RST 10H", 1, 16},
    {0xD8, false, "RET C", 1, 20, 8},
    {0xD9, false, "RETI", 1, 16},
    {0xDA, false, "JP C, a16", 3, 16, 12},
    {0xDB, false, "ILLEGAL_DB", 1, 4},
    {0xDC, false, "CALL C, a16", 3, 24, 12},
    {0xDD, false, "ILLEGAL_DD", 1, 4},
    {0xDE, false, "SBC A, d8", 2, 8},
    {0xDF, false, "RST 18H", 1, 16},
    //
    {0xE0, false, "LDH (a8), A", 2, 12},
    {0xE1, false, "POP HL", 1, 12},
    {0xE2, false, "LD (C), A", 1, 8},
    {0xE3, false, "ILLEGAL_E3", 1, 4},
    {0xE4, false, "ILLEGAL_E4", 1, 4},
    {0xE5, false, "PUSH HL", 1, 16},
    {0xE6, false, "AND d8", 2, 8},
    {0xE7, false, "RST 20H", 1, 16},
    {0xE8, false, "ADD SP, r8", 2, 16},
    {0xE9, false, "JP HL", 1, 4},
    {0xEA, false, "LD (a16), A", 3, 16},
    {0xEB, false, "ILLEGAL_EB", 1, 4},
    {0xEC, false, "ILLEGAL_EC", 1, 4},
    {0xED, false, "ILLEGAL_ED", 1, 4},
    {0xEE, false, "XOR d8", 2, 8},
    {0xEF, false, "RST 28H", 1, 16},
    //
    {0xF0, false, "LDH A, (a8)", 2, 12},
    {0xF1, false, "POP AF", 1, 12},
    {0xF2, false, "LD A, (C)", 1, 8},
    {0xF3, false, "DI", 1, 4},
    {0xF4, false, "ILLEGAL_F4", 1, 4},
    {0xF5, false, "PUSH AF", 1, 16},
    {0xF6, false, "OR d8", 2, 8},
    {0xF7, false, "RST 30H", 1, 16},
    {0xF8, false, "LD HL, SP+, r8", 2, 12},
    {0xF9, false, "LD SP, HL", 1, 8},
    {0xFA, false, "LD A, (a16)", 3, 16},
    {0xFB, false, "EI", 1, 4},
    {0xFC, false, "ILLEGAL_FC", 1, 4},
    {0xFD, false, "ILLEGAL_FD", 1, 4},
    {0xFE, false, "CP d8", 2, 8},
    {0xFF, false, "RST 38H", 1, 16}
}};

constexpr std::array<Opcode, 256> cb_opcodes {{
    {0x00, false, "RLC B", 2, 8},
    {0x01, false, "RLC C", 2, 8},
    {0x02, false, "RLC D", 2, 8},
    {0x03, false, "RLC E", 2, 8},
    {0x04, false, "RLC H", 2, 8},
    {0x05, false, "RLC L", 2, 8},
    {0x06, false, "RLC (HL)", 2, 16},
    {0x07, false, "RLC A", 2, 8},
    {0x08, false, "RRC B", 2, 8},
    {0x09, false, "RRC C", 2, 8},
    {0x0A, false, "RRC D", 2, 8},
    {0x0B, false, "RRC E", 2, 8},
    {0x0C, false, "RRC H", 2, 8},
    {0x0D, false, "RRC L", 2, 8},
    {0x0E, false, "RRC (HL)", 2, 16},
    {0x0F, false, "RRC A", 2, 8},
    //
    {0x10, false, "RL B", 2, 8},
    {0x11, false, "RL C", 2, 8},
    {0x12, false, "RL D", 2, 8},
    {0x13, false, "RL E", 2, 8},
    {0x14, false, "RL H", 2, 8},
    {0x15, false, "RL L", 2, 8},
    {0x16, false, "RL (HL)", 2, 16},
    {0x17, false, "RL A", 2, 8},
    {0x18, false, "RR B", 2, 8},
    {0x19, false, "RR C", 2, 8},
    {0x1A, false, "RR D", 2, 8},
    {0x1B, false, "RR E", 2, 8},
    {0x1C, false, "RR H", 2, 8},
    {0x1D, false, "RR L", 2, 8},
    {0x1E, false, "RR (HL)", 2, 16},
    {0x1F, false, "RR A", 2, 8},
    //
    {0x20, false, "SLA B", 2, 8},
    {0x21, false, "SLA C", 2, 8},
    {0x22, false, "SLA D", 2, 8},
    {0x23, false, "SLA E", 2, 8},
    {0x24, false, "SLA H", 2, 8},
    {0x25, false, "SLA L", 2, 8},
    {0x26, false, "SLA (HL)", 2, 16},
    {0x27, false, "SLA A", 2, 8},
    {0x28, false, "SRA B", 2, 8},
    {0x29, false, "SRA C", 2, 8},
    {0x2A, false, "SRA D", 2, 8},
    {0x2B, false, "SRA E", 2, 8},
    {0x2C, false, "SRA H", 2, 8},
    {0x2D, false, "SRA L", 2, 8},
    {0x2E, false, "SRA (HL)", 2, 16},
    {0x2F, false, "SRA A", 2, 8},
    //
    {0x30, false, "SWAP B", 2, 8},
    {0x31, false, "SWAP C", 2, 8},
    {0x32, false, "SWAP D", 2, 8},
    {0x33, false, "SWAP E", 2, 8},
    {0x34, false, "SWAP H", 2, 8},
    {0x35, false, "SWAP L", 2, 8},
    {0x36, false, "SWAP (HL)", 2, 16},
    {0x37, false, "SWAP A", 2, 8},
    {0x38, false, "SRL B", 2, 8},
    {0x39, false, "SRL C", 2, 8},
    {0x3A, false, "SRL D", 2, 8},
    {0x3B, false, "SRL E", 2, 8},
    {0x3C, false, "SRL H", 2, 8},
    {0x3D, false, "SRL L", 2, 8},
    {0x3E, false, "SRL (HL)", 2, 16},
    {0x3F, false, "SRL A", 2, 8},
    //
    {0x40, false, "BIT 0, B", 2, 8},
    {0x41, false, "BIT 0, C", 2, 8},
    {0x42, false, "BIT 0, D", 2, 8},
    {0x43, false, "BIT 0, E", 2, 8},
    {0x44, false, "BIT 0, H", 2, 8},
    {0x45, false, "BIT 0, L", 2, 8},
    {0x46, false, "BIT 0, (HL)", 2, 12},
    {0x47, false, "BIT 0, A", 2, 8},
    {0x48, false, "BIT 1, B", 2, 8},
    {0x49, false, "BIT 1, C", 2, 8},
    {0x4A, false, "BIT 1, D", 2, 8},
    {0x4B, false, "BIT 1, E", 2, 8},
    {0x4C, false, "BIT 1, H", 2, 8},
    {0x4D, false, "BIT 1, L", 2, 8},
    {0x4E, false, "BIT 1, (HL)", 2, 12},
    {0x4F, false, "BIT 1, A", 2, 8},
    //
    {0x50, false, "BIT 2, B", 2, 8},
    {0x51, false, "BIT 2, C", 2, 8},
    {0x52, false, "BIT 2, D", 2, 8},
    {0x53, false, "BIT 2, E", 2, 8},
    {0x54, false, "BIT 2, H", 2, 8},
    {0x55, false, "BIT 2, L", 2, 8},
    {0x56, false, "BIT 2, (HL)", 2, 12},
    {0x57, false, "BIT 2, A", 2, 8},
    {0x58, false, "BIT 3, B", 2, 8},
    {0x59, false, "BIT 3, C", 2, 8},
    {0x5A, false, "BIT 3, D", 2, 8},
    {0x5B, false, "BIT 3, E", 2, 8},
    {0x5C, false, "BIT 3, H", 2, 8},
    {0x5D, false, "BIT 3, L", 2, 8},
    {0x5E, false, "BIT 3, (HL)", 2, 12},
    {0x5F, false, "BIT 3, A", 2, 8},
    //
    {0x60, false, "BIT 4, B", 2, 8},
    {0x61, false, "BIT 4, C", 2, 8},
    {0x62, false, "BIT 4, D", 2, 8},
    {0x63, false, "BIT 4, E", 2, 8},
    {0x64, false, "BIT 4, H", 2, 8},
    {0x65, false, "BIT 4, L", 2, 8},
    {0x66, false, "BIT 4, (HL)", 2, 12},
    {0x67, false, "BIT 4, A", 2, 8},
    {0x68, false, "BIT 5, B", 2, 8},
    {0x69, false, "BIT 5, C", 2, 8},
    {0x6A, false, "BIT 5, D", 2, 8},
    {0x6B, false, "BIT 5, E", 2, 8},
    {0x6C, false, "BIT 5, H", 2, 8},
    {0x6D, false, "BIT 5, L", 2, 8},
    {0x6E, false, "BIT 5, (HL)", 2, 12},
    {0x6F, false, "BIT 5, A", 2, 8},
    //
    {0x70, false, "BIT 6, B", 2, 8},
    {0x71, false, "BIT 6, C", 2, 8},
    {0x72, false, "BIT 6, D", 2, 8},
    {0x73, false, "BIT 6, E", 2, 8},
    {0x74, false, "BIT 6, H", 2, 8},
    {0x75, false, "BIT 6, L", 2, 8},
    {0x76, false, "BIT 6, (HL)", 2, 12},
    {0x77, false, "BIT 6, A", 2, 8},
    {0x78, false, "BIT 7, B", 2, 8},
    {0x79, false, "BIT 7, C", 2, 8},
    {0x7A, false, "BIT 7, D", 2, 8},
    {0x7B, false, "BIT 7, E", 2, 8},
    {0x7C, false, "BIT 7, H", 2, 8},
    {0x7D, false, "BIT 7, L", 2, 8},
    {0x7E, false, "BIT 7, (HL)", 2, 12},
    {0x7F, false, "BIT 7, A", 2, 8},
    //
    {0x80, false, "RES 0, B", 2, 8},
    {0x81, false, "RES 0, C", 2, 8},
    {0x82, false, "RES 0, D", 2, 8},
    {0x83, false, "RES 0, E", 2, 8},
    {0x84, false, "RES 0, H", 2, 8},
    {0x85, false, "RES 0, L", 2, 8},
    {0x86, false, "RES 0, (HL)", 2, 16},
    {0x87, false, "RES 0, A", 2, 8},
    {0x88, false, "RES 1, B", 2, 8},
    {0x89, false, "RES 1, C", 2, 8},
    {0x8A, false, "RES 1, D", 2, 8},
    {0x8B, false, "RES 1, E", 2, 8},
    {0x8C, false, "RES 1, H", 2, 8},
    {0x8D, false, "RES 1, L", 2, 8},
    {0x8E, false, "RES 1, (HL)", 2, 16},
    {0x8F, false, "RES 1, A", 2, 8},
    //
    {0x90, false, "RES 2, B", 2, 8},
    {0x91, false, "RES 2, C", 2, 8},
    {0x92, false, "RES 2, D", 2, 8},
    {0x93, false, "RES 2, E", 2, 8},
    {0x94, false, "RES 2, H", 2, 8},
    {0x95, false, "RES 2, L", 2, 8},
    {0x96, false, "RES 2, (HL)", 2, 16},
    {0x97, false, "RES 2, A", 2, 8},
    {0x98, false, "RES 3, B", 2, 8},
    {0x99, false, "RES 3, C", 2, 8},
    {0x9A, false, "RES 3, D", 2, 8},
    {0x9B, false, "RES 3, E", 2, 8},
    {0x9C, false, "RES 3, H", 2, 8},
    {0x9D, false, "RES 3, L", 2, 8},
    {0x9E, false, "RES 3, (HL)", 2, 16},
    {0x9F, false, "RES 3, A", 2, 8},
    //
    {0xA0, false, "RES 4, B", 2, 8},
    {0xA1, false, "RES 4, C", 2, 8},
    {0xA2, false, "RES 4, D", 2, 8},
    {0xA3, false, "RES 4, E", 2, 8},
    {0xA4, false, "RES 4, H", 2, 8},
    {0xA5, false, "RES 4, L", 2, 8},
    {0xA6, false, "RES 4, (HL)", 2, 16},
    {0xA7, false, "RES 4, A", 2, 8},
    {0xA8, false, "RES 5, B", 2, 8},
    {0xA9, false, "RES 5, C", 2, 8},
    {0xAA, false, "RES 5, D", 2, 8},
    {0xAB, false, "RES 5, E", 2, 8},
    {0xAC, false, "RES 5, H", 2, 8},
    {0xAD, false, "RES 5, L", 2, 8},
    {0xAE, false, "RES 5, (HL)", 2, 16},
    {0xAF, false, "RES 5, A", 2, 8},
    //
    {0xB0, false, "RES 6, B", 2, 8},
    {0xB1, false, "RES 6, C", 2, 8},
    {0xB2, false, "RES 6, D", 2, 8},
    {0xB3, false, "RES 6, E", 2, 8},
    {0xB4, false, "RES 6, H", 2, 8},
    {0xB5, false, "RES 6, L", 2, 8},
    {0xB6, false, "RES 6, (HL)", 2, 16},
    {0xB7, false, "RES 6, A", 2, 8},
    {0xB8, false, "RES 7, B", 2, 8},
    {0xB9, false, "RES 7, C", 2, 8},
    {0xBA, false, "RES 7, D", 2, 8},
    {0xBB, false, "RES 7, E", 2, 8},
    {0xBC, false, "RES 7, H", 2, 8},
    {0xBD, false, "RES 7, L", 2, 8},
    {0xBE, false, "RES 7, (HL)", 2, 16},
    {0xBF, false, "RES 7, A", 2, 8},
    //
    {0xC0, false, "SET 0, B", 2, 8},
    {0xC1, false, "SET 0, C", 2, 8},
    {0xC2, false, "SET 0, D", 2, 8},
    {0xC3, false, "SET 0, E", 2, 8},
    {0xC4, false, "SET 0, H", 2, 8},
    {0xC5, false, "SET 0, L", 2, 8},
    {0xC6, false, "SET 0, (HL)", 2, 16},
    {0xC7, false, "SET 0, A", 2, 8},
    {0xC8, false, "SET 1, B", 2, 8},
    {0xC9, false, "SET 1, C", 2, 8},
    {0xCA, false, "SET 1, D", 2, 8},
    {0xCB, false, "SET 1, E", 2, 8},
    {0xCC, false, "SET 1, H", 2, 8},
    {0xCD, false, "SET 1, L", 2, 8},
    {0xCE, false, "SET 1, (HL)", 2, 16},
    {0xCF, false, "SET 1, A", 2, 8},
    //
    {0xD0, false, "SET 2, B", 2, 8},
    {0xD1, false, "SET 2, C", 2, 8},
    {0xD2, false, "SET 2, D", 2, 8},
    {0xD3, false, "SET 2, E", 2, 8},
    {0xD4, false, "SET 2, H", 2, 8},
    {0xD5, false, "SET 2, L", 2, 8},
    {0xD6, false, "SET 2, (HL)", 2, 16},
    {0xD7, false, "SET 2, A", 2, 8},
    {0xD8, false, "SET 3, B", 2, 8},
    {0xD9, false, "SET 3, C", 2, 8},
    {0xDA, false, "SET 3, D", 2, 8},
    {0xDB, false, "SET 3, E", 2, 8},
    {0xDC, false, "SET 3, H", 2, 8},
    {0xDD, false, "SET 3, L", 2, 8},
    {0xDE, false, "SET 3, (HL)", 2, 16},
    {0xDF, false, "SET 3, A", 2, 8},
    //
    {0xE0, false, "SET 4, B", 2, 8},
    {0xE1, false, "SET 4, C", 2, 8},
    {0xE2, false, "SET 4, D", 2, 8},
    {0xE3, false, "SET 4, E", 2, 8},
    {0xE4, false, "SET 4, H", 2, 8},
    {0xE5, false, "SET 4, L", 2, 8},
    {0xE6, false, "SET 4, (HL)", 2, 16},
    {0xE7, false, "SET 4, A", 2, 8},
    {0xE8, false, "SET 5, B", 2, 8},
    {0xE9, false, "SET 5, C", 2, 8},
    {0xEA, false, "SET 5, D", 2, 8},
    {0xEB, false, "SET 5, E", 2, 8},
    {0xEC, false, "SET 5, H", 2, 8},
    {0xED, false, "SET 5, L", 2, 8},
    {0xEE, false, "SET 5, (HL)", 2, 16},
    {0xEF, false, "SET 5, A", 2, 8},
    //
    {0xF0, false, "SET 6, B", 2, 8},
    {0xF1, false, "SET 6, C", 2, 8},
    {0xF2, false, "SET 6, D", 2, 8},
    {0xF3, false, "SET 6, E", 2, 8},
    {0xF4, false, "SET 6, H", 2, 8},
    {0xF5, false, "SET 6, L", 2, 8},
    {0xF6, false, "SET 6, (HL)", 2, 16},
    {0xF7, false, "SET 6, A", 2, 8},
    {0xF8, false, "SET 7, B", 2, 8},
    {0xF9, false, "SET 7, C", 2, 8},
    {0xFA, false, "SET 7, D", 2, 8},
    {0xFB, false, "SET 7, E", 2, 8},
    {0xFC, false, "SET 7, H", 2, 8},
    {0xFD, false, "SET 7, L", 2, 8},
    {0xFE, false, "SET 7, (HL)", 2, 16},
    {0xFF, false, "SET 7, A", 2, 8},
}};

} // namespace LR35902