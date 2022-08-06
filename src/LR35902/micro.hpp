#pragma once
#include <memory>
#include <type_traits>
#include "arguments.hpp"
#include "registers.hpp"
#include "../components/component.hpp"

namespace LR35902
{

namespace micro
{

struct Alu
{
    Alu(Registers& regs, Component& comp) : regs(regs), mmu(comp) {}
    Alu(Alu&&) = default;
    Alu& operator=(Alu&&) = default;


    enum class Jump { Z, NZ, C, NC, None };

    constexpr bool NOP()
    {
        regs.flags('-', '-', '-', '-');
        return true;
    }

    constexpr bool LD(const Args::Dst auto dst, const Args::Arg auto src)
    {
        using T = decltype(src)::size_t;
        const T value = read(src);
        write(dst, value);
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool LD(const Args::A dst, const Args::rHL src, const bool inc)
    {
        using Args::HL;
        LD(dst, src);
        const HL::size_t offset = inc ? +1 : -1;
        write(HL{}, read(HL{}) + offset);
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool LD(const Args::rHL dst, const Args::A src, const bool inc)
    {
        using Args::HL;
        LD(dst, src);
        const HL::size_t offset = inc ? +1 : -1;
        write(HL{}, read(HL{}) + offset);
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool LD(const Args::HL dst, const Args::SP src, const Args::S8 off)
    {
        using T = Args::SP::size_t;
        const T srcv = read(src);
        const T offv = read(off);
        const T adjusted = srcv + offv;
        write(dst, adjusted);
        const bool H = half_carry(srcv, offv);
        const bool C = carry(srcv, offv);
        regs.flags(0, 0, H, C);
        return true;
    }

    constexpr bool INC(const Args::Reg16 auto r)
    {
        write(r, read(r) + OFFSET);
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool INC(const Args::StdArg auto r)
    {
        const auto new_value = read(r) + OFFSET;
        write(r, new_value);
        const bool Z = new_value == 0;
        const bool H = half_carry(new_value, +OFFSET);
        regs.flags(Z, 0, H, '-');
        return true;
    }
    constexpr bool DEC(const Args::Reg16 auto r)
    {
        write(r, read(r) - OFFSET);
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool DEC(const Args::StdArg auto r)
    {
        const auto value = read(r);
        const auto new_value = value - OFFSET;
        write(r, new_value);
        const bool Z = new_value == 0;
        const bool H = half_carry(value, -OFFSET);
        regs.flags(Z, 1, H, '-');
        return true;
    }
    constexpr bool ADD(const Args::HL a, const Args::Reg16 auto b)
    {
        const auto av = read(a);
        const auto bv = read(b);
        const auto v = (av + bv);
        write(a, v);
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        regs.flags('-', 0, H, C);
        return true;
    }
    constexpr bool ADD(const Args::A a, const Args::StdArgImm auto b)
    {
        using T = Args::A::size_t;
        const T av = read(a);
        const T bv = read(b);
        const T v = (av + bv);
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        regs.flags(Z, 0, H, C);
        return true;
    }
    constexpr bool ADD(const Args::SP a, const Args::S8 b)
    {
        using T = Args::SP::size_t;
        const T av = read(a);
        const T bv = read(b);
        const T v = (av + bv);
        write(a, v);
        const bool H = half_carry(av, static_cast<uint16_t>(bv));
        const bool C = carry(av, static_cast<uint16_t>(bv));
        regs.flags(0, 0, H, C);
        return true;
    }
    constexpr bool ADC(const Args::A a, const Args::StdArgImm auto b)
    {
        const uint8_t av = read(a);
        const uint8_t bv = read(b) + regs.test(Registers::Flags::C);
        const auto v = av + bv;
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        regs.flags(Z, 0, H, C);
        return true;
    }
    constexpr bool SUB(const Args::A a, const Args::StdArgImm auto b)
    {
        const uint8_t av = read(a);
        const uint8_t bv = -read(b);
        const auto v = av + bv;
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        regs.flags(Z, 1, H, C);
        return true;
    }
    constexpr bool SBC(const Args::A a, const Args::StdArgImm auto b)
    {
        const uint8_t av = read(a);
        const uint8_t bv = -(read(b) + regs.test(Registers::Flags::C));
        const auto v = av + bv;
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        regs.flags(Z, 1, H, C);
        return true;
    }
    constexpr bool AND(const Args::A a, const Args::StdArgImm auto b)
    {
        const uint8_t av = read(a);
        const uint8_t bv = read(b);
        const auto v = av & bv;
        write(a, v);
        const bool Z = v == 0;
        regs.flags(Z, 0, 1, 0);
        return true;
    }
    constexpr bool XOR(const Args::A a, const Args::StdArgImm auto b)
    {
        const uint8_t av = read(a);
        const uint8_t bv = read(b);
        const auto v = av ^ bv;
        write(a, v);
        const bool Z = v == 0;
        regs.flags(Z, 0, 0, 0);
        return true;
    }
    constexpr bool OR(const Args::A a, const Args::StdArgImm auto b)
    {
        const uint8_t av = read(a);
        const uint8_t bv = read(b);
        const auto v = av | bv;
        write(a, v);
        const bool Z = v == 0;
        regs.flags(Z, 0, 0, 0);
        return true;
    }
    constexpr bool CP(const Args::A a, const Args::StdArgImm auto b)
    {
        const uint8_t av = read(a);
        const uint8_t bv = -read(b);
        const auto v = av + bv;
        // Don't write. Only compare
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        regs.flags(Z, 1, H, C);
        return true;
    }

    constexpr void push(const uint8_t value)
    {
        const auto spv = read(Args::SP{});
        write(Args::SP{}, spv - 1);
        write(Args::rSP{}, value);
    }
    constexpr void push(const uint16_t value /*native endian*/)
    {
        const uint16_t le_value = to_little_endian(value);
        push(static_cast<const uint8_t*>(static_cast<const void *>(&le_value))[1]);
        push(static_cast<const uint8_t*>(static_cast<const void *>(&le_value))[0]);
    }
    constexpr uint8_t pop8()
    {
        uint8_t value = read(Args::rSP{});
        const auto spv = read(Args::SP{});
        write(Args::SP{}, spv+1);
        return value;
    }
    constexpr uint16_t /*native endian*/ pop()
    {
        uint16_t le_value;
        reinterpret_cast<uint8_t*>(le_value)[0] = pop8();
        reinterpret_cast<uint8_t*>(le_value)[1] = pop8();
        return to_native_endian(le_value);
    }

    constexpr bool PUSH(const Args::Reg16 auto src)
    {
        push(read(src));
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool POP(const Args::Reg16 auto dst)
    {
        write(dst, pop());
        regs.flags('-', '-', '-', '-');
        return true;
    }

    constexpr bool RST(const Args::A16 addr)
    {
        push(read(Args::PC{}));
        write(Args::PC{}, read(addr));
        return true;
    }

    constexpr bool RET(Jump cond)
    {
        bool success = jump(cond);
        if (success) {
            write(Args::PC{}, pop());
        }
        return success;
    }
    constexpr bool CALL(const Args::A16 addr, Jump cond)
    {
        bool success = jump(cond);
        if (success) {
            push(read(Args::PC{}));
            write(Args::PC{}, read(addr));
        }
        regs.flags('-', '-', '-', '-');
        return success;
    }

    constexpr bool JR(const Args::S8 addr, Jump cond)
    {
        bool success = jump(cond);
        if (success) {
            write(Args::PC{}, read(Args::PC{}) + read(addr));
        }
        regs.flags('-', '-', '-', '-');
        return success;
    }
    constexpr bool JP(const Args::A16 addr, Jump cond)
    {
        bool success = jump(cond);
        if (success) {
            write(Args::PC{}, read(addr));
        }
        regs.flags('-', '-', '-', '-');
        return success;
    }
    constexpr bool JP(const Args::HL hl)
    {
        write(Args::PC{}, read(hl));
        regs.flags('-', '-', '-', '-');
        return true;
    }

    constexpr bool STOP()
    {
        return true;
    }
    constexpr bool HALT()
    {
        return true;
    }
    constexpr bool RETI()
    {
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool DI()
    {
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool EI()
    {
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool DAA()
    {
        return true;
    }

    constexpr bool RLC(const Args::StdArg auto arg)
    {
        const auto v = read(arg);
        const auto b7 = bit(7, v, 0);
        const auto shift = v << 1;
        const auto r = (shift & 0b1111'1110) | b7;
        write(arg, r);
        const bool Z = r == 0;
        const bool C = b7;
        regs.flags(Z, 0, 0, C);
        return true;
    }
    constexpr bool RRC(const Args::StdArg auto arg)
    {
        const auto v = read(arg);
        const auto b0 = bit(0, v, 7);
        const auto shift = v >> 1;
        const auto r = (shift & 0b0111'1111) | b0;
        write(arg, r);
        const bool Z = r == 0;
        const bool C = b0;
        regs.flags(Z, 0, 0, C);
        return true;
    }
    constexpr bool RL(const Args::StdArg auto arg)
    {
        const auto v = read(arg);
        const auto b7 = bit(7, v, 0);
        const auto c = regs.test(Registers::Flags::C);
        const auto shift = v << 1;
        const auto r = (shift & 0b1111'1110) | c;
        write(arg, r);
        const bool Z = r == 0;
        const bool C = b7;
        regs.flags(Z, 0, 0, C);
        return true;
    }
    constexpr bool RR(const Args::StdArg auto arg)
    {
        const auto v = read(arg);
        const auto b0 = bit(0, v, 0);
        const auto c = regs.test(Registers::Flags::C) << 7;
        const auto shift = v << 1;
        const auto r = (shift & 0b0111'1111) | c;
        write(arg, r);
        const bool Z = r == 0;
        const bool C = b0;
        regs.flags(Z, 0, 0, C);
        return true;
    }
    constexpr bool SLA(const Args::StdArg auto arg)
    {
        const auto v = read(arg);
        const auto b7 = bit(7, v, 0);
        const auto shift = v << 1;
        const auto r = (shift & 0b1111'1110);
        write(arg, r);
        const bool Z = r == 0;
        const bool C = b7;
        regs.flags(Z, 0, 0, C);
        return true;
    }
    constexpr bool SRA(const Args::StdArg auto arg)
    {
        const auto v = read(arg);
        const auto b7 = bit(7, v, 7);
        const auto b0 = bit(0, v, 0);
        const auto shift = v >> 1;
        const auto r = (shift & 0b0111'1111) | b7;
        write(arg, r);
        const bool Z = r == 0;
        const bool C = b0;
        regs.flags(Z, 0, 0, C);
        return true;
    }
    constexpr bool SWAP(const Args::StdArg auto arg)
    {
        const uint8_t v = read(arg);
        const auto r = nibbleswap(v);
        write(arg, r);
        const bool Z = r == 0;
        regs.flags(Z, 0, 0, 0);
        return true;
    }
    constexpr bool SRL(const Args::StdArg auto arg)
    {
        const uint8_t v = read(arg);
        const auto b0 = bit(0, v, 0);
        const uint8_t shift = v >> 1;
        const uint8_t r = (shift & 0b0111'1111);
        write(arg, r);
        const bool Z = r == 0;
        const bool C = b0;
        regs.flags(Z, 0, 0, C);
        return true;
    }
    constexpr bool BIT(int n, const Args::StdArg auto arg)
    {
        const uint8_t v = read(arg);
        const bool Z = !bit(n, v, n);
        regs.flags(Z, 0, 1, '-');
        return true;
    }
    constexpr bool RES(int n, const Args::StdArg auto arg)
    {
        const uint8_t v = read(arg);
        const uint8_t m = ~(uint8_t{1} << n);
        write(arg, v & m);
        regs.flags('-', '-', '-', '-');
        return true;
    }
    constexpr bool SET(int n, const Args::StdArg auto arg)
    {
        const uint8_t v = read(arg);
        const uint8_t m = uint8_t{1} << n;
        write(arg, v | m);
        regs.flags('-', '-', '-', '-');
        return true;
    }

    constexpr bool CCF()
    {
        const bool C = !regs.test(Registers::Flags::C);
        regs.flags('-', 0, 0, C);
        return true;
    }
    constexpr bool SCF()
    {
        regs.flags('-', 0, 0, 1);
        return true;
    }
    constexpr bool CPL()
    {
        const uint8_t v = read(Args::A{});
        write(Args::A{}, ~v);
        regs.flags('-', 1, 1, '-');
        return true;
    }

    constexpr bool RLCA()
    {
        const uint8_t v = read(Args::A{});
        const uint8_t b7 = bit(7, v, 0);
        const uint8_t shift = (v << 1);
        const uint8_t r = (shift & 0b1111'1110) | b7; 
        write(Args::A{}, r);
        const bool C = b7;
        regs.flags(0, 0, 0, C);
        return true;
    }
    constexpr bool RRCA()
    {
        const uint8_t v = read(Args::A{});
        const uint8_t b0 = bit(0, v, 7);
        const uint8_t shift = (v >> 1);
        const uint8_t r = (shift & 0b0111'1111) | b0;
        write(Args::A{}, r);
        const bool C = b0;
        regs.flags(0, 0, 0, C);
        return true;
    }
    constexpr bool RLA()
    {
        const uint8_t v = read(Args::A{});
        const uint8_t b7 = bit(7, v, 0);
        const uint8_t c = regs.test(Registers::Flags::C) << 0;
        const uint8_t shift = (v << 1);
        const uint8_t r = (shift & 0b1111'1110) | c;
        write(Args::A{}, r);
        const bool C = b7;
        regs.flags(0, 0, 0, C);
        return true;
    }
    constexpr bool RRA()
    {
        const uint8_t v = read(Args::A{});
        const uint8_t b0 = bit(0, v, 0);
        const uint8_t c = regs.test(Registers::Flags::C) << 7;
        const uint8_t shift = (v >> 1);
        const uint8_t r = (shift & 0b0111'1111) | c;
        write(Args::A{}, r);
        const bool C = b0;
        regs.flags(0, 0, 0, C);
        return true;
    }

private:

    constexpr bool jump(Jump cond) const
    {
        switch( cond ) {
            case Jump::Z: return regs.test(Registers::Flags::Z);
            case Jump::NZ: return !regs.test(Registers::Flags::Z);
            case Jump::C: return regs.test(Registers::Flags::C);
            case Jump::NC: return !regs.test(Registers::Flags::C);
            case Jump::None: return true;
            default: return false;
        }
        return false;
    }
    constexpr uint8_t bit(int loc, uint8_t value, int end = 0)
    {
        return ((value & (1 << loc)) >> loc) << end;
    }

    // Read from Register Immediately
    template<Type::Reg T> constexpr auto 
    read(const Args::Argument<T, Access::Immediate> reg)
        -> decltype(reg)::size_t
    {
        return regs.read(reg.type);
    }
    // Read from Immidiate Immediately
    template<Type::Imm T> constexpr auto 
    read(const Args::Argument<T, Access::Immediate> imm)
        -> decltype(imm)::size_t
    {
        return imm.type.value;
    }
    // Read Address Referenced by T
    template<typename T, Access::Ref A> constexpr auto
    read(const Args::Argument<T, A> arg)
        -> decltype(arg)::size_t
    {
        const auto addr = read(Args::Argument<T, Access::Immediate>{arg}) + A::offset;
        return mmu.read(addr);
    }
    // Write value to Register
    template<Type::Reg T> constexpr auto
    write(const Args::Argument<T, Access::Immediate> reg, const T::size_t value) -> void
    {
        regs.write(reg.type, value);
    }
    // Write value to Memory
    template<typename T, Access::Ref A> constexpr auto
    write(const Args::Argument<T, A> reg, const uint8_t value) -> void
    {
        using ArgImm = Args::Argument<T, Access::Immediate>;
        const auto addr = read(ArgImm{reg}) + A::offset;
        mmu.write(addr, value);
    }


    Registers& regs;
    Component& mmu;
    static constexpr uint8_t OFFSET = 1;
};

}
}