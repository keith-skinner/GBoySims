#pragma once
#include <memory>
#include <type_traits>
#include <concepts>
#include <cstdint>

#include <sc/string_constant.hpp>

#include <common/memory.hpp>
#include <common/utility/bits.hpp>
#include <common/utility/meta.hpp>

#include "arguments.hpp"
#include "registers.hpp"
#include "mmu.hpp"

constexpr auto FORCE_READ_WRITE_FLAGS = false;

namespace LR35902
{
template<typename T, typename ... Ts>
concept a_member_of = is_member_of_v<T, Ts...>;

template<typename R8>
concept a_common_r8 = a_member_of<R8, 
    Args::A, 
    Args::B, 
    Args::C, 
    Args::D, 
    Args::E, 
    Args::H, 
    Args::L>;

// an extended list of commonly used 8 bit registers that includes the 16 bit reference to rHL
template<typename R8>
concept a_common_r8_ext = a_common_r8<R8> || 
    std::is_same_v<R8, Args::rHL>;

// registers commonly used that are 16 bits
template<typename R16>
concept a_common_r16 = a_member_of<R16,
    Args::BC, 
    Args::DE,
    Args::HL, 
    Args::SP>;

// arguments commonly used that are 8 bits
template<typename Arg8>
concept a_common_arg8 = a_common_r8<Arg8> || 
        a_member_of<Arg8, Args::D8>;

constexpr auto carry = 
    [](const auto a, const decltype(a) b, const decltype(a) c = 0) -> bool {
        using I = std::remove_cvref_t<decltype(a)>;
        return bit_carry<I, std::numeric_limits<I>::digits>(a, b, c);
    };

constexpr auto half_carry = 
    [](const std::unsigned_integral auto a, const std::unsigned_integral auto b, const decltype(a) c = 0) -> bool {
        using I = std::remove_cvref_t<decltype(a)>;
        return bit_carry<I, std::numeric_limits<I>::digits-4>(a, b, c);
    };


template<typename Memory>
class Micro
{
public:
    enum class Jump { Z, NZ, C, NC, None };
private:
    RegisterFile& m_regs;
    Memory& m_mmu;
    bool m_IME = false; // interrupt master enable
    bool m_stop = false; // can only be cancelled by a reset signal
    
    bool m_halt_begin = false; // halt begins the instruction after the halt call
    bool m_halt = false; // system clock is stopped; Oscillator and LCD continue to operate

public:

    Micro(RegisterFile& regs, Memory& mmu)
    : regs{regs}, mmu{mmu}
    {}

    constexpr bool NOP()
    {
        flags("----"_sc);
        return true;
    }

    template<
        typename DstType, typename DstAccess,
        typename SrcType, typename SrcAccess>
    constexpr bool LD(
        const Args::Argument<DstType, DstAccess> dst,
        const Args::Argument<SrcType, SrcAccess> src)
    {
        const auto value = read(src);
        write(dst, value);
        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::A, const Args::rHL, const std::uint16_t adj)
    {
        const auto hl = read(Args::HL{});
        const auto rhl = read(Args::rA16{hl});
        write(Args::A{}, rhl);
        write(Args::HL{}, hl + adj);
        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::rHL, const Args::A, const std::uint16_t adj)
    {
        const auto a = read(Args::A{});
        const auto hl = read(Args::HL{});
        write(Args::rA16{hl}, a);
        write(Args::HL{}, hl + adj);
        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::HL, const Args::SP, const Args::S8 off)
    {
        const auto sp = read(Args::SP{});
        const auto voff = static_cast<decltype(sp)>(read(off)); // convert to unsigned
        write(Args::HL{}, sp + voff);
        const Register::Flags::H H = half_carry(sp, voff);
        const Register::Flags::C C = carry(sp, voff);
        flags("00HC"_sc, H, C);
        return true;
    }

#pragma endregion("LD")

#pragma region('MATH')

    constexpr bool INC(const a_common_r16 auto r)
    {
        write(r, read(r) + 1);
        flags("----"_sc);
        return true;
    }
    
    constexpr bool INC(const a_common_r8 auto r)
    {
        using value_t = typename decltype(r)::Type::Type;
        const value_t value = read(r);
        write(r, value + 1);
        const Register::Flags::Z Z = (value + 1) == 0;
        const Register::Flags::H H = half_carry(value, 1);
        flags("Z0H-"_sc, Z, H);
        return true;
    }

    constexpr bool DEC(const a_common_r16 auto r)
    {
        write(r, read(r) - 1);
        flags("----"_sc);
        return true;
    }

    constexpr bool DEC(const a_common_arg8 auto r)
    {
        using value_t = typename decltype(r)::Type::Type;
        const value_t value = read(r);
        write(r, value - 1);
        const Register::Flags::Z Z = value - 1 == 0;
        const Register::Flags::H H = half_carry(value, -1);
        flags("Z1H-"_sc, Z, H);
        return true;
    }

    constexpr bool ADD(const Args::HL a, const a_common_r16 auto b)
    {
        const std::uint16_t av = read(a);
        const std::uint16_t bv = read(b);
        const std::uint16_t v = (av + bv);
        write(a, v);
        const Register::Flags::H::Repr H = half_carry(av, bv);
        const Register::Flags::C::Repr C = carry(av, bv);
        flags("-0HC"_sc, H, C);
        return true;
    }

    constexpr bool ADD(const Args::A a, const a_common_r8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = read(b);
        const std::uint8_t v = (av + bv);
        write(a, v);
        const Register::Flags::Z::Repr Z = v == 0;
        const Register::Flags::H::Repr H = half_carry(av, bv);
        const Register::Flags::C::Repr C = carry(av, bv);
        flags("Z0HC"_sc, Z, H, C);
        return true;
    }

    constexpr bool ADD(const Args::SP a, const Args::S8 b)
    {
        const std::uint16_t av = read(a);
        const std::int8_t bv = read(b);
        const std::uint16_t v = (av + static_cast<std::uint16_t>(bv));
        write(a, v);
        const Register::Flags::H::Repr H = half_carry(av, static_cast<uint16_t>(bv));
        const Register::Flags::C::Repr C = carry(av, static_cast<uint16_t>(bv));
        flags("00HC"_sc, H, C);
        return true;
    }

    constexpr bool ADC(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bc = read(b) + read(Register::Flags::C{});
        const std::uint8_t v = av + bc;
        write(a, v);
        const Register::Flags::Z::Repr Z = v == 0;
        const Register::Flags::H::Repr H = half_carry(av, bc);
        const Register::Flags::C::Repr C = carry(av, bc);
        flags("Z0HC"_sc, Z, H, C);
        return true;
    }

    constexpr bool SUB(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = -to_unsigned(read(b));
        const std::uint8_t v = av + bv;
        write(a, v);
        const Register::Flags::Z::Repr Z = v == 0;
        const Register::Flags::H::Repr H = half_carry(av, bv);
        const Register::Flags::C::Repr C = carry(av, bv);
        flags("Z1HC"_sc, Z, H, C);
        return true;
    }

    constexpr bool SBC(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = -(to_unsigned(read(b)) + to_unsigned(read(Register::Flags::C{})));
        const std::uint8_t v = av + bv;
        write(a, v);
        const Register::Flags::Z::Repr Z = v == 0;
        const Register::Flags::H::Repr H = half_carry(av, bv);
        const Register::Flags::C::Repr C = carry(av, bv);
        flags("Z1HC"_sc, Z, H, C);
        return true;
    }

    constexpr bool AND(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = read(b);
        const std::uint8_t v = av & bv;
        write(a, v);
        const Register::Flags::Z::Repr Z = v == 0;
        flags("Z010"_sc, Z);
        return true;
    }

    constexpr bool XOR(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = read(b);
        const std::uint8_t v = av ^ bv;
        write(a, v);
        const Register::Flags::Z::Repr Z = v == 0;
        flags("Z000"_sc, Z);
        return true;
    }

    constexpr bool OR(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = read(b);
        const std::uint8_t v = av | bv;
        write(a, v);
        const Register::Flags::Z::Repr Z = v == 0;
        flags("Z000"_sc, Z);
        return true;
    }

    constexpr bool CP(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = -to_unsigned(read(b));
        const std::uint8_t v = av + bv;
        write(a, v);
        const Register::Flags::Z::Repr Z = v == 0;
        const Register::Flags::H::Repr H = half_carry(av, bv);
        const Register::Flags::C::Repr C = carry(av, bv);
        flags("Z1HC"_sc, Z, H, C);
        return true;
    }

    template <typename R16>
    requires a_member_of<R16, Args::BC, Args::DE, Args::HL, Args::AF>
    constexpr bool PUSH(const R16 src) 
    {
        static_assert(sizeof(read(src)) == 2);
        const std::uint16_t value = read(src);
        push(value);
        flags("----"_sc);
        return true;
    }

    template <typename R16>
    requires a_member_of<R16, Args::BC, Args::DE, Args::HL, Args::AF>
    constexpr bool POP(const R16 dst)
    {
        static_assert(sizeof(read(dst)) == 2);
        std::uint16_t value = pop();
        write(dst, value);
        flags("----"_sc);
        return true;
    }

    constexpr bool JP(const Args::A16 addr, const Jump cond = Jump::None)
    {
        const bool success = jump(cond);
        if (success)
            write(Args::PC{}, read(addr));
        flags("----"_sc);
        return success;
    }

    constexpr bool JP(const Args::HL hl)
    {
        write(Args::PC{}, read(hl));
        flags("----"_sc);
        return true;
    }

    constexpr bool JR(const Args::S8 offset, const Jump cond = Jump::None)
    {
        const bool success = jump(cond);
        if (success)
            write(Args::PC{}, read(Args::PC{}) + to_unsigned(read(offset)));
        flags("----"_sc);
        return true;
    }

    constexpr bool CALL(const Args::A16 addr, const Jump cond = Jump::None)
    {
        bool success = jump(cond);
        if (success) {
            push(read(Args::PC{}));
            write(Args::PC{}, read(addr));
        }
        flags("----"_sc);
        return success;
    }

    constexpr bool RST(const Args::A16 addr)
    {
        write(Args::SP{}, read(Args::PC{}));
        write(Args::PC{}, read(addr));
        flags("----"_sc);
        return true;
    }

    constexpr bool RET(Jump cond)
    {
        bool success = jump(cond);
        if (success)
            write(Args::PC{}, pop());
        flags("----"_sc);
        return success;
    }

    constexpr bool RETI()
    {
        m_IME = true;
        write(Args::PC{}, pop());
        flags("----"_sc);
        return true;
    }

    constexpr bool EI()
    {
        m_IME = true;
        flags("----"_sc);
        return true;
    }

    constexpr bool DI()
    {
        m_IME = false;
        flags("----"_sc);
        return true;
    }

    constexpr bool DAA()
    {
        std::uint8_t f = read(Args::F{});
        std::uint8_t a = read(Args::A{});
        const bool n = f & Register::Flags::N::reg_mask;
        const bool c = f & Register::Flags::C::reg_mask;
        const bool h = f & Register::Flags::H::reg_mask;
        if (!n) {
            if (c || a > 0x99)
                a += 0x60;
            if (h || (a & 0x0f) > 0x09)
                a += 0x06;
        }
        else {
            if (c)
                a -= 0x60;
            if (h)
                a -= 0x06;
        }
        Register::Flags::C::Repr C = !n && (c || a > 0x99);
        Register::Flags::Z::Repr Z = a == 0;
        flags("Z-0C"_sc, Z, C);
        return true;
    }

    constexpr bool STOP()
    {
        m_stop = true;
        flags("----"_sc);
        return true;
    }

    constexpr bool HALT()
    {
        m_halt_begin = true;
        m_halt = true;
        if (m_IME)
            ;
        flags("----"_sc);
        return true;
    }

//     

//     constexpr bool RLC(const Args::StdArg auto arg)
//     {
//         const auto v = read(arg);
//         const auto b7 = bit(7, v, 0);
//         const auto shift = v << 1;
//         const auto r = (shift & 0b1111'1110) | b7;
//         write(arg, r);
//         const bool Z = r == 0;
//         const bool C = b7;
//         regs.flags(Z, 0, 0, C);
//         return true;
//     }

//     constexpr bool RRC(const Args::StdArg auto arg)
//     {
//         const auto v = read(arg);
//         const auto b0 = bit(0, v, 7);
//         const auto shift = v >> 1;
//         const auto r = (shift & 0b0111'1111) | b0;
//         write(arg, r);
//         const bool Z = r == 0;
//         const bool C = b0;
//         regs.flags(Z, 0, 0, C);
//         return true;
//     }

//     constexpr bool RL(const Args::StdArg auto arg)
//     {
//         const auto v = read(arg);
//         const auto b7 = bit(7, v, 0);
//         const auto c = regs.test(Registers::Flags::C);
//         const auto shift = v << 1;
//         const auto r = (shift & 0b1111'1110) | c;
//         write(arg, r);
//         const bool Z = r == 0;
//         const bool C = b7;
//         regs.flags(Z, 0, 0, C);
//         return true;
//     }

//     constexpr bool RR(const Args::StdArg auto arg)
//     {
//         const auto v = read(arg);
//         const auto b0 = bit(0, v, 0);
//         const auto c = regs.test(Registers::Flags::C) << 7;
//         const auto shift = v << 1;
//         const auto r = (shift & 0b0111'1111) | c;
//         write(arg, r);
//         const bool Z = r == 0;
//         const bool C = b0;
//         regs.flags(Z, 0, 0, C);
//         return true;
//     }

//     constexpr bool SLA(const Args::StdArg auto arg)
//     {
//         const auto v = read(arg);
//         const auto b7 = bit(7, v, 0);
//         const auto shift = v << 1;
//         const auto r = (shift & 0b1111'1110);
//         write(arg, r);
//         const bool Z = r == 0;
//         const bool C = b7;
//         regs.flags(Z, 0, 0, C);
//         return true;
//     }

//     constexpr bool SRA(const Args::StdArg auto arg)
//     {
//         const auto v = read(arg);
//         const auto b7 = bit(7, v, 7);
//         const auto b0 = bit(0, v, 0);
//         const auto shift = v >> 1;
//         const auto r = (shift & 0b0111'1111) | b7;
//         write(arg, r);
//         const bool Z = r == 0;
//         const bool C = b0;
//         regs.flags(Z, 0, 0, C);
//         return true;
//     }

//     constexpr bool SWAP(const Args::StdArg auto arg)
//     {
//         const uint8_t v = read(arg);
//         const auto r = nibbleswap(v);
//         write(arg, r);
//         const bool Z = r == 0;
//         regs.flags(Z, 0, 0, 0);
//         return true;
//     }

//     constexpr bool SRL(const Args::StdArg auto arg)
//     {
//         const uint8_t v = read(arg);
//         const auto b0 = bit(0, v, 0);
//         const uint8_t shift = v >> 1;
//         const uint8_t r = (shift & 0b0111'1111);
//         write(arg, r);
//         const bool Z = r == 0;
//         const bool C = b0;
//         regs.flags(Z, 0, 0, C);
//         return true;
//     }

//     constexpr bool BIT(int n, const Args::StdArg auto arg)
//     {
//         const uint8_t v = read(arg);
//         const bool Z = !bit(n, v, n);
//         regs.flags(Z, 0, 1, '-');
//         return true;
//     }

//     constexpr bool RES(int n, const Args::StdArg auto arg)
//     {
//         const uint8_t v = read(arg);
//         const uint8_t m = ~(uint8_t{1} << n);
//         write(arg, v & m);
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }

//     constexpr bool SET(int n, const Args::StdArg auto arg)
//     {
//         const uint8_t v = read(arg);
//         const uint8_t m = uint8_t{1} << n;
//         write(arg, v | m);
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }

//     constexpr bool CCF()
//     {
//         const bool C = !regs.test(Registers::Flags::C);
//         regs.flags('-', 0, 0, C);
//         return true;
//     }

//     constexpr bool SCF()
//     {
//         regs.flags('-', 0, 0, 1);
//         return true;
//     }

//     constexpr bool CPL()
//     {
//         const uint8_t v = read(Args::A{});
//         write(Args::A{}, ~v);
//         regs.flags('-', 1, 1, '-');
//         return true;
//     }

//     constexpr bool RLCA()
//     {
//         const uint8_t v = read(Args::A{});
//         const uint8_t b7 = bit(7, v, 0);
//         const uint8_t shift = (v << 1);
//         const uint8_t r = (shift & 0b1111'1110) | b7;
//         write(Args::A{}, r);
//         const bool C = b7;
//         regs.flags(0, 0, 0, C);
//         return true;
//     }

//     constexpr bool RRCA()
//     {
//         const uint8_t v = read(Args::A{});
//         const uint8_t b0 = bit(0, v, 7);
//         const uint8_t shift = (v >> 1);
//         const uint8_t r = (shift & 0b0111'1111) | b0;
//         write(Args::A{}, r);
//         const bool C = b0;
//         regs.flags(0, 0, 0, C);
//         return true;
//     }

//     constexpr bool RLA()
//     {
//         const auto v = read(Args::A{});
//         const uint8_t b7 = bit(7, v, 0);
//         const uint8_t c = regs.test(Registers::Flags::C) << 0;
//         const uint8_t shift = (v << 1);
//         const uint8_t r = (shift & 0b1111'1110) | c;
//         write(Args::A{}, r);
//         const bool C = b7;
//         regs.flags(0, 0, 0, C);
//         return true;
//     }

//     constexpr bool RRA()
//     {
//         const uint8_t v = read(Args::A{});
//         const uint8_t b0 = bit(0, v, 0);
//         const uint8_t c = regs.test(Registers::Flags::C) << 7;
//         const uint8_t shift = (v >> 1);
//         const uint8_t r = (shift & 0b0111'1111) | c;
//         write(Args::A{}, r);
//         const bool C = b0;
//         regs.flags(0, 0, 0, C);
//         return true;
//     }

private:
    constexpr void push(const std::uint16_t value)
    {
        std::uint16_t sp = read(Args::SP{});
        write(Args::rA16{--sp}, (0xFF00 & value) >> 8);
        write(Args::rA16{--sp}, (0x00FF & value) >> 0);
        write(Args::SP{}, sp);
    }

    constexpr std::uint16_t pop()
    {
        std::uint16_t sp = read(Args::SP{});
        const std::uint8_t lsb = read(Args::rA16{sp++});
        const std::uint8_t msb = read(Args::rA16{sp++});
        write(Args::SP{}, sp);
        return (static_cast<std::uint16_t>(msb) << 8) | lsb;
    }

    constexpr bool jump(Jump cond) const
    {
        using namespace LR35902;
        switch( cond ) {
            case Jump::Z:  return  m_regs.read<Register::Flags::Z>();
            // case Jump::NZ: return !m_regs.read<Register::Flags::Z>();
            // case Jump::C:  return  m_regs.read<Register::Flags::C>();
            // case Jump::NC: return !m_regs.read<Register::Flags::C>();
            case Jump::None: return true;
            default: return false; // Should not happen
        }
        return false;
    }

    // Read from Register Immediately
    template<Type::a_register Type, Access::a_immediate Access>
    constexpr auto read(const Args::Argument<Type, Access> arg) const -> typename Type::Type
    {
        using arg_t = decltype(arg);
        using reg_t = Args::to_register_v<arg_t>;
        return m_regs.read<reg_t>();
    }

    // Read from Immediate Immediately
    template<Type::a_immediate Type, Access::a_immediate Access>
    constexpr auto read(const Args::Argument<Type, Access> imm) const -> typename Type::Type
    {
        return imm.type.value;
    }

    // Read Referenced Address
    template<Type::a_arg_type Type, Access::a_reference Access>
    constexpr uint8_t read(const Args::Argument<Type, Access> arg) const
    {
        const auto addr = read(Args::Argument<Type, typename Access::Immediate>{arg.value()}) + Access::offset;
        return m_mmu.read(addr);
    }

    // Write value to Register
    template<Type::a_register Type, Access::a_immediate Access>
    constexpr void write(const Args::Argument<Type, Access> reg, const typename Type::Type value)
    {
        using arg_t = decltype(reg);
        m_regs.write(typename Args::to_register_v<arg_t>(value));
    }

    // Write value to Memory
    template<Type::a_arg_type Type, Access::a_reference Access>
    constexpr void write(const Args::Argument<Type, Access> arg, const uint8_t value)
    {
        using non_ref_t = Args::Argument<Type, typename Access::Immediate>;
        const auto addr = read(non_ref_t{arg.value()}) + Access::offset;
        m_mmu.write(addr, value);
    }

    template <typename FlagStr, typename ... FlagsT>
    requires (true && ... && std::same_as<FlagsT, bool>)
    void flags(FlagStr flagStr, FlagsT ... flagsVal)
    {
        // Validate FlagStr
        static_assert(flagStr.size() == 4);
        static_assert(flagStr[0] == 'Z' || flagStr[0] == '-' || flagStr[0] == '0' || flagStr[0] == '1');
        static_assert(flagStr[1] == 'N' || flagStr[0] == '-' || flagStr[0] == '0' || flagStr[0] == '1');
        static_assert(flagStr[2] == 'H' || flagStr[0] == '-' || flagStr[0] == '0' || flagStr[0] == '1');
        static_assert(flagStr[3] == 'C' || flagStr[0] == '-' || flagStr[0] == '0' || flagStr[0] == '1');

        // Validate FlagStr
        static constexpr int evalFlagsCount = 
            (flagStr[0] == 'Z') + 
            (flagStr[1] == 'N') + 
            (flagStr[2] == 'H') + 
            (flagStr[3] == 'C');
        static_assert(sizeof...(FlagsT) == evalFlagsCount);

        const auto flagVals = std::make_tuple(flagsVal...);
        static constexpr int INVALID = -1;
        static constexpr int zIdx = INVALID + 1 * (flagStr[0] == 'Z');
        static constexpr int nIdx = zIdx + 1 * (flagStr[1] == 'N');
        static constexpr int hIdx = nIdx + 1 * (flagStr[2] == 'H');
        static constexpr int cIdx = hIdx + 1 * (flagStr[3] == 'C');

        // TODO make a read mask based on if flagStr[n] == '-'
        // then make a write mask that has a 1 for 0/1/ZNHC
        // Then its read f ; clear non-read mask bits ; set bits that would be set by 1 / ZNHC
        // Instead of doing this individually for every bit below.


        auto f = m_regs.read<Register::F>();
        if constexpr (flagStr != "----"_sc)
        {
            
            
            Register::Flags::Z::write(f, false || (flagStr[0] == '1') || (flagStr[0] == '-' && Register::Flags::Z::read(f)) || (flagStr[0] == 'Z' && std::get<zIdx>(flagVals)));
            Register::Flags::N::write(f, false || (flagStr[1] == '1') || (flagStr[1] == '-' && Register::Flags::N::read(f)) || (flagStr[1] == 'N' && std::get<nIdx>(flagVals)));
            Register::Flags::H::write(f, false || (flagStr[2] == '1') || (flagStr[2] == '-' && Register::Flags::H::read(f)) || (flagStr[2] == 'H' && std::get<hIdx>(flagVals)));
            Register::Flags::C::write(f, false || (flagStr[3] == '1') || (flagStr[3] == '-' && Register::Flags::C::read(f)) || (flagStr[3] == 'C' && std::get<cIdx>(flagVals)));
        }
        m_regs.write<Register::F>(f);
    }
};

}