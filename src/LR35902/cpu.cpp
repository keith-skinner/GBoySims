#include "cpu.hpp"
#include <stdexcept>
#include "utility/hex.hpp"
namespace LR35902
{

constexpr bool run(const Opcode& code, micro::Alu& alu, const std::uint16_t arg);

uint16_t read_byte(const Component& component, const uint16_t addr)
{
    return component.read(addr);
}

uint16_t read_word(const Component& component, const uint16_t addr)
{
    const uint8_t b0 = component.read(addr);
    const uint8_t b1 = component.read(addr+1);
    const uint16_t word = to_native_endian(b0, b1);
}

constexpr bool has_argument(const Opcode& code)
{
    return (!code.CB && code.length != 1);
}

constexpr uint16_t argument(const Opcode& code, const Component& component, const uint16_t loc)
{
    if (code.length == 2)
        return read_byte(component, loc);
    else
        return read_word(component, loc);
}

Opcode Cpu::instruction() const
{
    return decode();
}

std::uint16_t Cpu::argument() const
{
    Opcode code = instruction();
    return has_argument(code)
        ? LR35902::argument(code, component, PC()+1) 
        : 0x0000u;
}

Opcode Cpu::decode() const
{
    uint8_t code = read_byte(component, PC());
    return code == 0xCB
        ? cb_opcodes.at(read_byte(component, PC()+1))
        : opcodes.at(code);
}

void Cpu::run()
{
    const Opcode opcode = decode();
    const uint16_t argument = has_argument(opcode) 
        ? LR35902::argument(opcode, component, PC()+1)
        : 0x0000u;
    bool cycles = LR35902::run(opcode, alu, argument);
    
}

bool illegal(const Opcode& code)
{
    throw std::runtime_error("Attempted to run illegal opcode: 0x" + to_hex(code.code));
    return true;
}

constexpr bool run(const Opcode& code, micro::Alu& alu, const std::uint16_t arg)
{
    if (!code.CB)
    {
        using Jump = micro::Alu::Jump;
        using namespace Args;
        switch (code.code)
        {
        case 0x00: return alu.NOP();
        case 0x01: return alu.LD(BC{}, D16{arg});
        case 0x02: return alu.LD(rBC{}, A{});
        case 0x03: return alu.INC(BC{});
        case 0x04: return alu.INC(B{});
        case 0x05: return alu.DEC(B{});
        case 0x06: return alu.LD(B{}, D8{arg});
        case 0x07: return alu.RLCA();
        case 0x08: return alu.LD(rA16{arg}, SP{});
        case 0x09: return alu.ADD(HL{}, BC{});
        case 0x0A: return alu.LD(A{}, rBC{});
        case 0x0B: return alu.DEC(BC{});
        case 0x0C: return alu.INC(C{});
        case 0x0D: return alu.DEC(C{});
        case 0x0E: return alu.LD(C{}, D8{arg});
        case 0x0F: return alu.RRCA();
        
        case 0x10: return alu.STOP();
        case 0x11: return alu.LD(DE{}, D16{arg}); 
        case 0x12: return alu.LD(rDE{}, A{});
        case 0x13: return alu.INC(DE{});
        case 0x14: return alu.INC(D{});
        case 0x15: return alu.DEC(D{});
        case 0x16: return alu.LD(D{}, D8{arg});
        case 0x17: return alu.RLA();
        case 0x18: return alu.JR(S8{arg}, Jump::None);
        case 0x19: return alu.ADD(HL{}, DE{});
        case 0x1A: return alu.LD(A{}, rDE{});
        case 0x1B: return alu.DEC(DE{});
        case 0x1C: return alu.INC(E{});
        case 0x1D: return alu.DEC(E{});
        case 0x1E: return alu.LD(E{}, D8{arg});
        case 0x1F: return alu.RRA();
        
        case 0x20: return alu.JR(S8{arg}, Jump::NZ);
        case 0x21: return alu.LD(HL{}, D16{arg});
        case 0x22: return alu.LD(rHL{}, A{}, true);
        case 0x23: return alu.INC(HL{});
        case 0x24: return alu.INC(H{});
        case 0x25: return alu.DEC(H{});
        case 0x26: return alu.LD(H{}, D8{arg});
        case 0x27: return alu.DAA();
        case 0x28: return alu.JR(S8{arg}, Jump::Z);
        case 0x29: return alu.ADD(HL{}, HL{});
        case 0x2A: return alu.LD(A{}, rHL{}, true);
        case 0x2B: return alu.DEC(HL{});
        case 0x2C: return alu.INC(L{});
        case 0x2D: return alu.DEC(L{});
        case 0x2E: return alu.LD(L{}, D8{arg});
        case 0x2F: return alu.CPL();
        
        case 0x30: return alu.JR(S8{arg}, Jump::NC);
        case 0x31: return alu.LD(SP{}, D16{arg});
        case 0x32: return alu.LD(rHL{}, A{}, false);
        case 0x33: return alu.INC(SP{});
        case 0x34: return alu.INC(rHL{});
        case 0x35: return alu.DEC(rHL{});
        case 0x36: return alu.LD(rHL{}, D8{arg});
        case 0x37: return alu.SCF();
        case 0x38: return alu.JR(S8{arg}, Jump::C);
        case 0x39: return alu.ADD(HL{}, SP{});
        case 0x3A: return alu.LD(A{}, rHL{}, false);
        case 0x3B: return alu.DEC(SP{});
        case 0x3C: return alu.INC(A{});
        case 0x3D: return alu.DEC(A{});
        case 0x3E: return alu.LD(A{}, D8{arg});
        case 0x3F: return alu.CCF();
        
        case 0x40: return alu.LD(B{}, B{});
        case 0x41: return alu.LD(B{}, C{});
        case 0x42: return alu.LD(B{}, D{});
        case 0x43: return alu.LD(B{}, E{});
        case 0x44: return alu.LD(B{}, H{});
        case 0x45: return alu.LD(B{}, L{});
        case 0x46: return alu.LD(B{}, rHL{});
        case 0x47: return alu.LD(B{}, A{});
        case 0x48: return alu.LD(C{}, B{});
        case 0x49: return alu.LD(C{}, C{});
        case 0x4A: return alu.LD(C{}, D{});
        case 0x4B: return alu.LD(C{}, E{});
        case 0x4C: return alu.LD(C{}, H{});
        case 0x4D: return alu.LD(C{}, L{});
        case 0x4E: return alu.LD(C{}, rHL{});
        case 0x4F: return alu.LD(C{}, A{});
        
        case 0x50: return alu.LD(D{}, B{});
        case 0x51: return alu.LD(D{}, C{});
        case 0x52: return alu.LD(D{}, D{});
        case 0x53: return alu.LD(D{}, E{});
        case 0x54: return alu.LD(D{}, H{});
        case 0x55: return alu.LD(D{}, L{});
        case 0x56: return alu.LD(D{}, rHL{});
        case 0x57: return alu.LD(D{}, A{});
        case 0x58: return alu.LD(E{}, B{});
        case 0x59: return alu.LD(E{}, C{});
        case 0x5A: return alu.LD(E{}, D{});
        case 0x5B: return alu.LD(E{}, E{});
        case 0x5C: return alu.LD(E{}, H{});
        case 0x5D: return alu.LD(E{}, L{});
        case 0x5E: return alu.LD(E{}, rHL{});
        case 0x5F: return alu.LD(E{}, A{});
        
        case 0x60: return alu.LD(H{}, B{});
        case 0x61: return alu.LD(H{}, C{});
        case 0x62: return alu.LD(H{}, D{});
        case 0x63: return alu.LD(H{}, E{});
        case 0x64: return alu.LD(H{}, H{});
        case 0x65: return alu.LD(H{}, L{});
        case 0x66: return alu.LD(H{}, rHL{});
        case 0x67: return alu.LD(H{}, A{});
        case 0x68: return alu.LD(L{}, B{});
        case 0x69: return alu.LD(L{}, C{});
        case 0x6A: return alu.LD(L{}, D{});
        case 0x6B: return alu.LD(L{}, E{});
        case 0x6C: return alu.LD(L{}, H{});
        case 0x6D: return alu.LD(L{}, L{});
        case 0x6E: return alu.LD(L{}, rHL{});
        case 0x6F: return alu.LD(L{}, A{});
        
        case 0x70: return alu.LD(rHL{}, B{});
        case 0x71: return alu.LD(rHL{}, C{});
        case 0x72: return alu.LD(rHL{}, D{});
        case 0x73: return alu.LD(rHL{}, E{});
        case 0x74: return alu.LD(rHL{}, H{});
        case 0x75: return alu.LD(rHL{}, L{});
        case 0x76: return alu.HALT();
        case 0x77: return alu.LD(rHL{}, A{});
        case 0x78: return alu.LD(A{}, B{});
        case 0x79: return alu.LD(A{}, C{});
        case 0x7A: return alu.LD(A{}, D{});
        case 0x7B: return alu.LD(A{}, E{});
        case 0x7C: return alu.LD(A{}, H{});
        case 0x7D: return alu.LD(A{}, L{});
        case 0x7E: return alu.LD(A{}, rHL{});
        case 0x7F: return alu.LD(A{}, A{});
        
        case 0x80: return alu.ADD(A{}, B{});
        case 0x81: return alu.ADD(A{}, C{});
        case 0x82: return alu.ADD(A{}, D{});
        case 0x83: return alu.ADD(A{}, E{});
        case 0x84: return alu.ADD(A{}, H{});
        case 0x85: return alu.ADD(A{}, L{});
        case 0x86: return alu.ADD(A{}, rHL{});
        case 0x87: return alu.ADD(A{}, A{});
        case 0x88: return alu.ADC(A{}, B{});
        case 0x89: return alu.ADC(A{}, C{});
        case 0x8A: return alu.ADC(A{}, D{});
        case 0x8B: return alu.ADC(A{}, E{});
        case 0x8C: return alu.ADC(A{}, H{});
        case 0x8D: return alu.ADC(A{}, L{});
        case 0x8E: return alu.ADC(A{}, rHL{});
        case 0x8F: return alu.ADC(A{}, A{});
        
        case 0x90: return alu.SUB(A{}, B{});
        case 0x91: return alu.SUB(A{}, C{});
        case 0x92: return alu.SUB(A{}, D{});
        case 0x93: return alu.SUB(A{}, E{});
        case 0x94: return alu.SUB(A{}, H{});
        case 0x95: return alu.SUB(A{}, L{});
        case 0x96: return alu.SUB(A{}, rHL{});
        case 0x97: return alu.SUB(A{}, A{});
        case 0x98: return alu.SBC(A{}, B{});
        case 0x99: return alu.SBC(A{}, C{});
        case 0x9A: return alu.SBC(A{}, D{});
        case 0x9B: return alu.SBC(A{}, E{});
        case 0x9C: return alu.SBC(A{}, H{});
        case 0x9D: return alu.SBC(A{}, L{});
        case 0x9E: return alu.SBC(A{}, rHL{});
        case 0x9F: return alu.SBC(A{}, A{});
        
        case 0xA0: return alu.AND(A{}, B{});
        case 0xA1: return alu.AND(A{}, C{});
        case 0xA2: return alu.AND(A{}, D{});
        case 0xA3: return alu.AND(A{}, E{});
        case 0xA4: return alu.AND(A{}, H{});
        case 0xA5: return alu.AND(A{}, L{});
        case 0xA6: return alu.AND(A{}, rHL{});
        case 0xA7: return alu.AND(A{}, A{});
        case 0xA8: return alu.XOR(A{}, B{});
        case 0xA9: return alu.XOR(A{}, C{});
        case 0xAA: return alu.XOR(A{}, D{});
        case 0xAB: return alu.XOR(A{}, E{});
        case 0xAC: return alu.XOR(A{}, H{});
        case 0xAD: return alu.XOR(A{}, L{});
        case 0xAE: return alu.XOR(A{}, rHL{});
        case 0xAF: return alu.XOR(A{}, A{});
        
        case 0xB0: return alu.OR(A{}, B{});
        case 0xB1: return alu.OR(A{}, C{});
        case 0xB2: return alu.OR(A{}, D{});
        case 0xB3: return alu.OR(A{}, E{});
        case 0xB4: return alu.OR(A{}, H{});
        case 0xB5: return alu.OR(A{}, L{});
        case 0xB6: return alu.OR(A{}, rHL{});
        case 0xB7: return alu.OR(A{}, A{});
        case 0xB8: return alu.CP(A{}, B{});
        case 0xB9: return alu.CP(A{}, C{});
        case 0xBA: return alu.CP(A{}, D{});
        case 0xBB: return alu.CP(A{}, E{});
        case 0xBC: return alu.CP(A{}, H{});
        case 0xBD: return alu.CP(A{}, L{});
        case 0xBE: return alu.CP(A{}, rHL{});
        case 0xBF: return alu.CP(A{}, A{});
        
        case 0xC0: return alu.RET(Jump::NZ);
        case 0xC1: return alu.POP(BC{});
        case 0xC2: return alu.JP(A16{arg}, Jump::NZ);
        case 0xC3: return alu.JP(A16{arg}, Jump::None);
        case 0xC4: return alu.CALL(A16{arg}, Jump::NZ);
        case 0xC5: return alu.PUSH(BC{});
        case 0xC6: return alu.ADD(A{}, D8{arg});
        case 0xC7: return alu.RST(A16{0x0000});
        case 0xC8: return alu.RET(Jump::Z);
        case 0xC9: return alu.RET(Jump::None);
        case 0xCA: return alu.JP(A16{arg}, Jump::Z);
        case 0xCB: return illegal(code);
        case 0xCC: return alu.CALL(A16{arg}, Jump::Z);
        case 0xCD: return alu.CALL(A16{arg}, Jump::None);
        case 0xCE: return alu.ADC(A{}, D8{arg});
        case 0xCF: return alu.RST(A16{0x0008});
        
        case 0xD0: return alu.RET(Jump::NC);
        case 0xD1: return alu.POP(DE{});
        case 0xD2: return alu.JP(A16{arg}, Jump::NC);
        case 0xD3: return illegal(code);
        case 0xD4: return alu.CALL(A16{arg}, Jump::NC);
        case 0xD5: return alu.PUSH(DE{});
        case 0xD6: return alu.SUB(A{}, D8{arg});
        case 0xD7: return alu.RST(A16{0x0010});
        case 0xD8: return alu.RET(Jump::C);
        case 0xD9: return alu.RETI();
        case 0xDA: return alu.JP(A16{arg}, Jump::C);
        case 0xDB: return illegal(code);
        case 0xDC: return alu.CALL(A16{arg}, Jump::C);
        case 0xDD: return illegal(code);
        case 0xDE: return alu.SBC(A{}, D8{arg});
        case 0xDF: return alu.RST(A16{0x0018});
        
        case 0xE0: return alu.LD(zA8{arg}, A{});
        case 0xE1: return alu.POP(HL{});
        case 0xE2: return alu.LD(zC{}, A{});
        case 0xE3: return illegal(code);
        case 0xE4: return illegal(code);
        case 0xE5: return alu.PUSH(HL{});
        case 0xE6: return alu.AND(A{}, D8{arg});
        case 0xE7: return alu.RST(A16{0x0020});
        case 0xE8: return alu.ADD(SP{}, S8{arg});
        case 0xE9: return alu.JP(HL{});
        case 0xEA: return alu.LD(rA16{arg}, A{});
        case 0xEB: return illegal(code);
        case 0xEC: return illegal(code);
        case 0xED: return illegal(code);
        case 0xEE: return alu.XOR(A{}, D8{arg});
        case 0xEF: return alu.RST(A16{0x0028});
        
        case 0xF0: return alu.LD(A{}, zA8{arg});
        case 0xF1: return alu.POP(AF{});
        case 0xF2: return alu.LD(A{}, zC{});
        case 0xF3: return alu.DI();
        case 0xF4: return illegal(code);
        case 0xF5: return alu.PUSH(AF{});
        case 0xF6: return alu.OR(A{}, D8{arg});
        case 0xF7: return alu.RST(A16{0x0030});
        case 0xF8: return alu.LD(HL{}, SP{}, S8{arg});
        case 0xF9: return alu.LD(SP{}, HL{});
        case 0xFA: return alu.LD(A{}, zA8{arg});
        case 0xFB: return alu.EI();
        case 0xFC: return illegal(code);
        case 0xFD: return illegal(code);
        case 0xFE: return alu.CP(A{}, D8{arg});
        case 0xFF: return alu.RST(A16{0x0038});
        
        default: return illegal(code); // if we got here we missed an instruction.
        }
    }
    else 
    {
        using namespace Args;
        switch (code.code)
        {
            case 0x00: return alu.RLC(B{});  // {0x00, "RLC B", 2, 8},
            case 0x01: return alu.RLC(C{});  // {0x01, "RLC C", 2, 8},
            case 0x02: return alu.RLC(D{});  // {0x02, "RLC D", 2, 8},
            case 0x03: return alu.RLC(E{});  // {0x03, "RLC E", 2, 8},
            case 0x04: return alu.RLC(H{});  // {0x04, "RLC H", 2, 8},
            case 0x05: return alu.RLC(L{});  // {0x05, "RLC L", 2, 8},
            case 0x06: return alu.RLC(rHL{});// {0x06, "RLC (HL)", 2, 16},
            case 0x07: return alu.RLC(A{});  // {0x07, "RLC A", 2, 8},
            case 0x08: return alu.RRC(B{});  // {0x08, "RRC B", 2, 8},
            case 0x09: return alu.RRC(C{});  // {0x09, "RRC C", 2, 8},
            case 0x0A: return alu.RRC(D{});  // {0x0A, "RRC D", 2, 8},
            case 0x0B: return alu.RRC(E{});  // {0x0B, "RRC E", 2, 8},
            case 0x0C: return alu.RRC(H{});  // {0x0C, "RRC H", 2, 8},
            case 0x0D: return alu.RRC(L{});  // {0x0D, "RRC L", 2, 8},
            case 0x0E: return alu.RRC(rHL{});// {0x0E, "RRC (HL)", 2, 16},
            case 0x0F: return alu.RRC(A{});  // {0x0F, "RRC A", 2, 8},
            case 0x10: return alu.RL(B{});  // {0x10, "RL B", 2, 8},
            case 0x11: return alu.RL(C{});  // {0x11, "RL C", 2, 8},
            case 0x12: return alu.RL(D{});  // {0x12, "RL D", 2, 8},
            case 0x13: return alu.RL(E{});  // {0x13, "RL E", 2, 8},
            case 0x14: return alu.RL(H{});  // {0x14, "RL H", 2, 8},
            case 0x15: return alu.RL(L{});  // {0x15, "RL L", 2, 8},
            case 0x16: return alu.RL(rHL{});// {0x16, "RL (HL)", 2, 16},
            case 0x17: return alu.RL(A{});  // {0x17, "RL A", 2, 8},
            case 0x18: return alu.RR(B{});  // {0x18, "RR B", 2, 8},
            case 0x19: return alu.RR(C{});  // {0x19, "RR C", 2, 8},
            case 0x1A: return alu.RR(D{});  // {0x1A, "RR D", 2, 8},
            case 0x1B: return alu.RR(E{});  // {0x1B, "RR E", 2, 8},
            case 0x1C: return alu.RR(H{});  // {0x1C, "RR H", 2, 8},
            case 0x1D: return alu.RR(L{});  // {0x1D, "RR L", 2, 8},
            case 0x1E: return alu.RR(rHL{});// {0x1E, "RR (HL)", 2, 16},
            case 0x1F: return alu.RR(A{});  // {0x1F, "RR A", 2, 8},
            case 0x20: return alu.SLA(B{});  // {0x20, "SLA B", 2, 8},
            case 0x21: return alu.SLA(C{});  // {0x21, "SLA C", 2, 8},
            case 0x22: return alu.SLA(D{});  // {0x22, "SLA D", 2, 8},
            case 0x23: return alu.SLA(E{});  // {0x23, "SLA E", 2, 8},
            case 0x24: return alu.SLA(H{});  // {0x24, "SLA H", 2, 8},
            case 0x25: return alu.SLA(L{});  // {0x25, "SLA L", 2, 8},
            case 0x26: return alu.SLA(rHL{});// {0x26, "SLA (HL)", 2, 16},
            case 0x27: return alu.SLA(A{});  // {0x27, "SLA A", 2, 8},
            case 0x28: return alu.SRA(B{});  // {0x28, "SRA B", 2, 8},
            case 0x29: return alu.SRA(C{});  // {0x29, "SRA C", 2, 8},
            case 0x2A: return alu.SRA(D{});  // {0x2A, "SRA D", 2, 8},
            case 0x2B: return alu.SRA(E{});  // {0x2B, "SRA E", 2, 8},
            case 0x2C: return alu.SRA(H{});  // {0x2C, "SRA H", 2, 8},
            case 0x2D: return alu.SRA(L{});  // {0x2D, "SRA L", 2, 8},
            case 0x2E: return alu.SRA(rHL{});// {0x2E, "SRA (HL)", 2, 16},
            case 0x2F: return alu.SRA(A{});  // {0x2F, "SRA A", 2, 8},
            case 0x30: return alu.SWAP(B{});  // {0x30, "SWAP B", 2, 8},
            case 0x31: return alu.SWAP(C{});  // {0x31, "SWAP C", 2, 8},
            case 0x32: return alu.SWAP(D{});  // {0x32, "SWAP D", 2, 8},
            case 0x33: return alu.SWAP(E{});  // {0x33, "SWAP E", 2, 8},
            case 0x34: return alu.SWAP(H{});  // {0x34, "SWAP H", 2, 8},
            case 0x35: return alu.SWAP(L{});  // {0x35, "SWAP L", 2, 8},
            case 0x36: return alu.SWAP(rHL{});// {0x36, "SWAP (HL)", 2, 16},
            case 0x37: return alu.SWAP(A{});  // {0x37, "SWAP A", 2, 8},
            case 0x38: return alu.SRL(B{});  // {0x38, "SRL B", 2, 8},
            case 0x39: return alu.SRL(C{});  // {0x39, "SRL C", 2, 8},
            case 0x3A: return alu.SRL(D{});  // {0x3A, "SRL D", 2, 8},
            case 0x3B: return alu.SRL(E{});  // {0x3B, "SRL E", 2, 8},
            case 0x3C: return alu.SRL(H{});  // {0x3C, "SRL H", 2, 8},
            case 0x3D: return alu.SRL(L{});  // {0x3D, "SRL L", 2, 8},
            case 0x3E: return alu.SRL(rHL{});// {0x3E, "SRL (HL)", 2, 16},
            case 0x3F: return alu.SRL(A{});  // {0x3F, "SRL A", 2, 8},
            case 0x40: return alu.BIT(0, B{});  // {0x40, "BIT 0, B", 2, 8},
            case 0x41: return alu.BIT(0, C{});  // {0x41, "BIT 0, C", 2, 8},
            case 0x42: return alu.BIT(0, D{});  // {0x42, "BIT 0, D", 2, 8},
            case 0x43: return alu.BIT(0, E{});  // {0x43, "BIT 0, E", 2, 8},
            case 0x44: return alu.BIT(0, H{});  // {0x44, "BIT 0, H", 2, 8},
            case 0x45: return alu.BIT(0, L{});  // {0x45, "BIT 0, L", 2, 8},
            case 0x46: return alu.BIT(0, rHL{});// {0x46, "BIT 0, (HL)", 2, 12},
            case 0x47: return alu.BIT(0, A{});  // {0x47, "BIT 0, A", 2, 8},
            case 0x48: return alu.BIT(1, B{});  // {0x48, "BIT 1, B", 2, 8},
            case 0x49: return alu.BIT(1, C{});  // {0x49, "BIT 1, C", 2, 8},
            case 0x4A: return alu.BIT(1, D{});  // {0x4A, "BIT 1, D", 2, 8},
            case 0x4B: return alu.BIT(1, E{});  // {0x4B, "BIT 1, E", 2, 8},
            case 0x4C: return alu.BIT(1, H{});  // {0x4C, "BIT 1, H", 2, 8},
            case 0x4D: return alu.BIT(1, L{});  // {0x4D, "BIT 1, L", 2, 8},
            case 0x4E: return alu.BIT(1, rHL{});// {0x4E, "BIT 1, (HL)", 2, 12},
            case 0x4F: return alu.BIT(1, A{});  // {0x4F, "BIT 1, A", 2, 8},
            case 0x50: return alu.BIT(2, B{});  // {0x50, "BIT 2, B", 2, 8},
            case 0x51: return alu.BIT(2, C{});  // {0x51, "BIT 2, C", 2, 8},
            case 0x52: return alu.BIT(2, D{});  // {0x52, "BIT 2, D", 2, 8},
            case 0x53: return alu.BIT(2, E{});  // {0x53, "BIT 2, E", 2, 8},
            case 0x54: return alu.BIT(2, H{});  // {0x54, "BIT 2, H", 2, 8},
            case 0x55: return alu.BIT(2, L{});  // {0x55, "BIT 2, L", 2, 8},
            case 0x56: return alu.BIT(2, rHL{});// {0x56, "BIT 2, (HL)", 2, 12},
            case 0x57: return alu.BIT(2, A{});  // {0x57, "BIT 2, A", 2, 8},
            case 0x58: return alu.BIT(3, B{});  // {0x58, "BIT 3, B", 2, 8},
            case 0x59: return alu.BIT(3, C{});  // {0x59, "BIT 3, C", 2, 8},
            case 0x5A: return alu.BIT(3, D{});  // {0x5A, "BIT 3, D", 2, 8},
            case 0x5B: return alu.BIT(3, E{});  // {0x5B, "BIT 3, E", 2, 8},
            case 0x5C: return alu.BIT(3, H{});  // {0x5C, "BIT 3, H", 2, 8},
            case 0x5D: return alu.BIT(3, L{});  // {0x5D, "BIT 3, L", 2, 8},
            case 0x5E: return alu.BIT(3, rHL{});// {0x5E, "BIT 3, (HL)", 2, 12},
            case 0x5F: return alu.BIT(3, A{});  // {0x5F, "BIT 3, A", 2, 8},
            case 0x60: return alu.BIT(4, B{});  // {0x60, "BIT 4, B", 2, 8},
            case 0x61: return alu.BIT(4, C{});  // {0x61, "BIT 4, C", 2, 8},
            case 0x62: return alu.BIT(4, D{});  // {0x62, "BIT 4, D", 2, 8},
            case 0x63: return alu.BIT(4, E{});  // {0x63, "BIT 4, E", 2, 8},
            case 0x64: return alu.BIT(4, H{});  // {0x64, "BIT 4, H", 2, 8},
            case 0x65: return alu.BIT(4, L{});  // {0x65, "BIT 4, L", 2, 8},
            case 0x66: return alu.BIT(4, rHL{});// {0x66, "BIT 4, (HL)", 2, 12},
            case 0x67: return alu.BIT(4, A{});  // {0x67, "BIT 4, A", 2, 8},
            case 0x68: return alu.BIT(5, B{});  // {0x68, "BIT 5, B", 2, 8},
            case 0x69: return alu.BIT(5, C{});  // {0x69, "BIT 5, C", 2, 8},
            case 0x6A: return alu.BIT(5, D{});  // {0x6A, "BIT 5, D", 2, 8},
            case 0x6B: return alu.BIT(5, E{});  // {0x6B, "BIT 5, E", 2, 8},
            case 0x6C: return alu.BIT(5, H{});  // {0x6C, "BIT 5, H", 2, 8},
            case 0x6D: return alu.BIT(5, L{});  // {0x6D, "BIT 5, L", 2, 8},
            case 0x6E: return alu.BIT(5, rHL{});// {0x6E, "BIT 5, (HL)", 2, 12},
            case 0x6F: return alu.BIT(5, A{});  // {0x6F, "BIT 5, A", 2, 8},
            case 0x70: return alu.BIT(6, B{});  // {0x70, "BIT 6, B", 2, 8},
            case 0x71: return alu.BIT(6, C{});  // {0x71, "BIT 6, C", 2, 8},
            case 0x72: return alu.BIT(6, D{});  // {0x72, "BIT 6, D", 2, 8},
            case 0x73: return alu.BIT(6, E{});  // {0x73, "BIT 6, E", 2, 8},
            case 0x74: return alu.BIT(6, H{});  // {0x74, "BIT 6, H", 2, 8},
            case 0x75: return alu.BIT(6, L{});  // {0x75, "BIT 6, L", 2, 8},
            case 0x76: return alu.BIT(6, rHL{});// {0x76, "BIT 6, (HL)", 2, 12},
            case 0x77: return alu.BIT(6, A{});  // {0x77, "BIT 6, A", 2, 8},
            case 0x78: return alu.BIT(7, B{});  // {0x78, "BIT 7, B", 2, 8},
            case 0x79: return alu.BIT(7, C{});  // {0x79, "BIT 7, C", 2, 8},
            case 0x7A: return alu.BIT(7, D{});  // {0x7A, "BIT 7, D", 2, 8},
            case 0x7B: return alu.BIT(7, E{});  // {0x7B, "BIT 7, E", 2, 8},
            case 0x7C: return alu.BIT(7, H{});  // {0x7C, "BIT 7, H", 2, 8},
            case 0x7D: return alu.BIT(7, L{});  // {0x7D, "BIT 7, L", 2, 8},
            case 0x7E: return alu.BIT(7, rHL{});// {0x7E, "BIT 7, (HL)", 2, 12},
            case 0x7F: return alu.BIT(7, A{});  // {0x7F, "BIT 7, A", 2, 8},
            case 0x80: return alu.RES(0, B{});  // {0x80, "RES 0, B", 2, 8},
            case 0x81: return alu.RES(0, C{});  // {0x81, "RES 0, C", 2, 8},
            case 0x82: return alu.RES(0, D{});  // {0x82, "RES 0, D", 2, 8},
            case 0x83: return alu.RES(0, E{});  // {0x83, "RES 0, E", 2, 8},
            case 0x84: return alu.RES(0, H{});  // {0x84, "RES 0, H", 2, 8},
            case 0x85: return alu.RES(0, L{});  // {0x85, "RES 0, L", 2, 8},
            case 0x86: return alu.RES(0, rHL{});// {0x86, "RES 0, (HL)", 2, 16},
            case 0x87: return alu.RES(0, A{});  // {0x87, "RES 0, A", 2, 8},
            case 0x88: return alu.RES(1, B{});  // {0x88, "RES 1, B", 2, 8},
            case 0x89: return alu.RES(1, C{});  // {0x89, "RES 1, C", 2, 8},
            case 0x8A: return alu.RES(1, D{});  // {0x8A, "RES 1, D", 2, 8},
            case 0x8B: return alu.RES(1, E{});  // {0x8B, "RES 1, E", 2, 8},
            case 0x8C: return alu.RES(1, H{});  // {0x8C, "RES 1, H", 2, 8},
            case 0x8D: return alu.RES(1, L{});  // {0x8D, "RES 1, L", 2, 8},
            case 0x8E: return alu.RES(1, rHL{});// {0x8E, "RES 1, (HL)", 2, 16},
            case 0x8F: return alu.RES(1, A{});  // {0x8F, "RES 1, A", 2, 8},
            case 0x90: return alu.RES(2, B{});  // {0x90, "RES 2, B", 2, 8},
            case 0x91: return alu.RES(2, C{});  // {0x91, "RES 2, C", 2, 8},
            case 0x92: return alu.RES(2, D{});  // {0x92, "RES 2, D", 2, 8},
            case 0x93: return alu.RES(2, E{});  // {0x93, "RES 2, E", 2, 8},
            case 0x94: return alu.RES(2, H{});  // {0x94, "RES 2, H", 2, 8},
            case 0x95: return alu.RES(2, L{});  // {0x95, "RES 2, L", 2, 8},
            case 0x96: return alu.RES(2, rHL{});// {0x96, "RES 2, (HL)", 2, 16},
            case 0x97: return alu.RES(2, A{});  // {0x97, "RES 2, A", 2, 8},
            case 0x98: return alu.RES(3, B{});  // {0x98, "RES 3, B", 2, 8},
            case 0x99: return alu.RES(3, C{});  // {0x99, "RES 3, C", 2, 8},
            case 0x9A: return alu.RES(3, D{});  // {0x9A, "RES 3, D", 2, 8},
            case 0x9B: return alu.RES(3, E{});  // {0x9B, "RES 3, E", 2, 8},
            case 0x9C: return alu.RES(3, H{});  // {0x9C, "RES 3, H", 2, 8},
            case 0x9D: return alu.RES(3, L{});  // {0x9D, "RES 3, L", 2, 8},
            case 0x9E: return alu.RES(3, rHL{});// {0x9E, "RES 3, (HL)", 2, 16},
            case 0x9F: return alu.RES(3, A{});  // {0x9F, "RES 3, A", 2, 8},
            case 0xA0: return alu.RES(4, B{});  // {0xA0, "RES 4, B", 2, 8},
            case 0xA1: return alu.RES(4, C{});  // {0xA1, "RES 4, C", 2, 8},
            case 0xA2: return alu.RES(4, D{});  // {0xA2, "RES 4, D", 2, 8},
            case 0xA3: return alu.RES(4, E{});  // {0xA3, "RES 4, E", 2, 8},
            case 0xA4: return alu.RES(4, H{});  // {0xA4, "RES 4, H", 2, 8},
            case 0xA5: return alu.RES(4, L{});  // {0xA5, "RES 4, L", 2, 8},
            case 0xA6: return alu.RES(4, rHL{});// {0xA6, "RES 4, (HL)", 2, 16},
            case 0xA7: return alu.RES(4, A{});  // {0xA7, "RES 4, A", 2, 8},
            case 0xA8: return alu.RES(5, B{});  // {0xA8, "RES 5, B", 2, 8},
            case 0xA9: return alu.RES(5, C{});  // {0xA9, "RES 5, C", 2, 8},
            case 0xAA: return alu.RES(5, D{});  // {0xAA, "RES 5, D", 2, 8},
            case 0xAB: return alu.RES(5, E{});  // {0xAB, "RES 5, E", 2, 8},
            case 0xAC: return alu.RES(5, H{});  // {0xAC, "RES 5, H", 2, 8},
            case 0xAD: return alu.RES(5, L{});  // {0xAD, "RES 5, L", 2, 8},
            case 0xAE: return alu.RES(5, rHL{});// {0xAE, "RES 5, (HL)", 2, 16},
            case 0xAF: return alu.RES(5, A{});  // {0xAF, "RES 5, A", 2, 8},
            case 0xB0: return alu.RES(6, B{});  // {0xB0, "RES 6, B", 2, 8},
            case 0xB1: return alu.RES(6, C{});  // {0xB1, "RES 6, C", 2, 8},
            case 0xB2: return alu.RES(6, D{});  // {0xB2, "RES 6, D", 2, 8},
            case 0xB3: return alu.RES(6, E{});  // {0xB3, "RES 6, E", 2, 8},
            case 0xB4: return alu.RES(6, H{});  // {0xB4, "RES 6, H", 2, 8},
            case 0xB5: return alu.RES(6, L{});  // {0xB5, "RES 6, L", 2, 8},
            case 0xB6: return alu.RES(6, rHL{});// {0xB6, "RES 6, (HL)", 2, 16},
            case 0xB7: return alu.RES(6, A{});  // {0xB7, "RES 6, A", 2, 8},
            case 0xB8: return alu.RES(7, B{});  // {0xB8, "RES 7, B", 2, 8},
            case 0xB9: return alu.RES(7, C{});  // {0xB9, "RES 7, C", 2, 8},
            case 0xBA: return alu.RES(7, D{});  // {0xBA, "RES 7, D", 2, 8},
            case 0xBB: return alu.RES(7, E{});  // {0xBB, "RES 7, E", 2, 8},
            case 0xBC: return alu.RES(7, H{});  // {0xBC, "RES 7, H", 2, 8},
            case 0xBD: return alu.RES(7, L{});  // {0xBD, "RES 7, L", 2, 8},
            case 0xBE: return alu.RES(7, rHL{});// {0xBE, "RES 7, (HL)", 2, 16},
            case 0xBF: return alu.RES(7, A{});  // {0xBF, "RES 7, A", 2, 8},
            case 0xC0: return alu.SET(0, B{});  // {0xC0, "SET 0, B", 2, 8},
            case 0xC1: return alu.SET(0, C{});  // {0xC1, "SET 0, C", 2, 8},
            case 0xC2: return alu.SET(0, D{});  // {0xC2, "SET 0, D", 2, 8},
            case 0xC3: return alu.SET(0, E{});  // {0xC3, "SET 0, E", 2, 8},
            case 0xC4: return alu.SET(0, H{});  // {0xC4, "SET 0, H", 2, 8},
            case 0xC5: return alu.SET(0, L{});  // {0xC5, "SET 0, L", 2, 8},
            case 0xC6: return alu.SET(0, rHL{});// {0xC6, "SET 0, (HL)", 2, 16},
            case 0xC7: return alu.SET(0, A{});  // {0xC7, "SET 0, A", 2, 8},
            case 0xC8: return alu.SET(1, B{});  // {0xC8, "SET 1, B", 2, 8},
            case 0xC9: return alu.SET(1, C{});  // {0xC9, "SET 1, C", 2, 8},
            case 0xCA: return alu.SET(1, D{});  // {0xCA, "SET 1, D", 2, 8},
            case 0xCB: return alu.SET(1, E{});  // {0xCB, "SET 1, E", 2, 8},
            case 0xCC: return alu.SET(1, H{});  // {0xCC, "SET 1, H", 2, 8},
            case 0xCD: return alu.SET(1, L{});  // {0xCD, "SET 1, L", 2, 8},
            case 0xCE: return alu.SET(1, rHL{});// {0xCE, "SET 1, (HL)", 2, 16},
            case 0xCF: return alu.SET(1, A{});  // {0xCF, "SET 1, A", 2, 8},
            case 0xD0: return alu.SET(2, B{});  // {0xD0, "SET 2, B", 2, 8},
            case 0xD1: return alu.SET(2, C{});  // {0xD1, "SET 2, C", 2, 8},
            case 0xD2: return alu.SET(2, D{});  // {0xD2, "SET 2, D", 2, 8},
            case 0xD3: return alu.SET(2, E{});  // {0xD3, "SET 2, E", 2, 8},
            case 0xD4: return alu.SET(2, H{});  // {0xD4, "SET 2, H", 2, 8},
            case 0xD5: return alu.SET(2, L{});  // {0xD5, "SET 2, L", 2, 8},
            case 0xD6: return alu.SET(2, rHL{});// {0xD6, "SET 2, (HL)", 2, 16},
            case 0xD7: return alu.SET(2, A{});  // {0xD7, "SET 2, A", 2, 8},
            case 0xD8: return alu.SET(3, B{});  // {0xD8, "SET 3, B", 2, 8},
            case 0xD9: return alu.SET(3, C{});  // {0xD9, "SET 3, C", 2, 8},
            case 0xDA: return alu.SET(3, D{});  // {0xDA, "SET 3, D", 2, 8},
            case 0xDB: return alu.SET(3, E{});  // {0xDB, "SET 3, E", 2, 8},
            case 0xDC: return alu.SET(3, H{});  // {0xDC, "SET 3, H", 2, 8},
            case 0xDD: return alu.SET(3, L{});  // {0xDD, "SET 3, L", 2, 8},
            case 0xDE: return alu.SET(3, rHL{});// {0xDE, "SET 3, (HL)", 2, 16},
            case 0xDF: return alu.SET(3, A{});  // {0xDF, "SET 3, A", 2, 8},
            case 0xE0: return alu.SET(4, B{});  // {0xE0, "SET 4, B", 2, 8},
            case 0xE1: return alu.SET(4, C{});  // {0xE1, "SET 4, C", 2, 8},
            case 0xE2: return alu.SET(4, D{});  // {0xE2, "SET 4, D", 2, 8},
            case 0xE3: return alu.SET(4, E{});  // {0xE3, "SET 4, E", 2, 8},
            case 0xE4: return alu.SET(4, H{});  // {0xE4, "SET 4, H", 2, 8},
            case 0xE5: return alu.SET(4, L{});  // {0xE5, "SET 4, L", 2, 8},
            case 0xE6: return alu.SET(4, rHL{});// {0xE6, "SET 4, (HL)", 2, 16},
            case 0xE7: return alu.SET(4, A{});  // {0xE7, "SET 4, A", 2, 8},
            case 0xE8: return alu.SET(5, B{});  // {0xE8, "SET 5, B", 2, 8},
            case 0xE9: return alu.SET(5, C{});  // {0xE9, "SET 5, C", 2, 8},
            case 0xEA: return alu.SET(5, D{});  // {0xEA, "SET 5, D", 2, 8},
            case 0xEB: return alu.SET(5, E{});  // {0xEB, "SET 5, E", 2, 8},
            case 0xEC: return alu.SET(5, H{});  // {0xEC, "SET 5, H", 2, 8},
            case 0xED: return alu.SET(5, L{});  // {0xED, "SET 5, L", 2, 8},
            case 0xEE: return alu.SET(5, rHL{});// {0xEE, "SET 5, (HL)", 2, 16},
            case 0xEF: return alu.SET(5, A{});  // {0xEF, "SET 5, A", 2, 8},
            case 0xF0: return alu.SET(6, B{});  // {0xF0, "SET 6, B", 2, 8},
            case 0xF1: return alu.SET(6, C{});  // {0xF1, "SET 6, C", 2, 8},
            case 0xF2: return alu.SET(6, D{});  // {0xF2, "SET 6, D", 2, 8},
            case 0xF3: return alu.SET(6, E{});  // {0xF3, "SET 6, E", 2, 8},
            case 0xF4: return alu.SET(6, H{});  // {0xF4, "SET 6, H", 2, 8},
            case 0xF5: return alu.SET(6, L{});  // {0xF5, "SET 6, L", 2, 8},
            case 0xF6: return alu.SET(6, rHL{});// {0xF6, "SET 6, (HL)", 2, 16},
            case 0xF7: return alu.SET(6, A{});  // {0xF7, "SET 6, A", 2, 8},
            case 0xF8: return alu.SET(7, B{});  // {0xF8, "SET 7, B", 2, 8},
            case 0xF9: return alu.SET(7, C{});  // {0xF9, "SET 7, C", 2, 8},
            case 0xFA: return alu.SET(7, D{});  // {0xFA, "SET 7, D", 2, 8},
            case 0xFB: return alu.SET(7, E{});  // {0xFB, "SET 7, E", 2, 8},
            case 0xFC: return alu.SET(7, H{});  // {0xFC, "SET 7, H", 2, 8},
            case 0xFD: return alu.SET(7, L{});  // {0xFD, "SET 7, L", 2, 8},
            case 0xFE: return alu.SET(7, rHL{});// {0xFE, "SET 7, (HL)", 2, 16},
            case 0xFF: return alu.SET(7, A{});  // {0xFF, "SET 7, A", 2, 8},
            default: return illegal(code);
        }
    }
    return illegal(code);
}

}