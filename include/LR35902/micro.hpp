#pragma once
#include <memory>
#include <type_traits>
#include <concepts>
#include <cstdint>

#include <sc/string_constant.hpp>

#include <common/memory.hpp>
#include <common/utility/bits.hpp>

#include "arguments.hpp"
#include "registers.hpp"
#include "mmu.hpp"

constexpr auto FORCE_READ_WRITE_FLAGS = false;

namespace LR35902
{

template<typename T, typename ... Ts>
constexpr bool is_member_of_v = (false || ... || std::is_same_v<std::remove_cvref_t<T>, Ts>);

template<typename T, typename ... Ts>
concept a_member_of = is_member_of_v<T, Ts...>;

template<typename T>
constexpr bool is_flag_v = a_member_of<T, 
    LR35902::Flags::Z, 
    LR35902::Flags::N, 
    LR35902::Flags::H, 
    LR35902::Flags::C>;

template<typename T>
concept a_flag = is_flag_v<T>;

template<typename R8>
concept a_common_r8 = a_member_of<R8,
        LR35902::Args::A, LR35902::Args::B,
        LR35902::Args::C, LR35902::Args::D, 
        LR35902::Args::E, LR35902::Args::H,
        LR35902::Args::L, LR35902::Args::rHL>;

template<typename R16>
concept a_common_r16 = a_member_of<R16,
        LR35902::Args::BC, LR35902::Args::DE,
        LR35902::Args::HL, LR35902::Args::SP>;

template<typename Arg8>
concept a_common_arg8 = a_common_r8<Arg8> || 
        a_member_of<Arg8, LR35902::Args::D8>;

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


template<typename Registers, typename Memory>
class Micro
{
public:
    enum class Jump { Z, NZ, C, NC, None };
private:
    Registers& regs;
    Memory& mmu;
    bool IME = false; // interrupt master enable
public:

    Micro(Registers& regs, Memory& mmu)
    : regs{regs}, mmu{mmu}
    {}

    constexpr bool NOP()
    {
        flags("----"_sc);
        return true;
    }

    bool getIME() const
    {
        return IME;
    }

#pragma region('LD')

    template<
        typename DstType, typename DstAccess,
        typename SrcType, typename SrcAccess>
    constexpr bool LD(
        const LR35902::Args::Argument<DstType, DstAccess> dst,
        const LR35902::Args::Argument<SrcType, SrcAccess> src)
    {
        const auto value = read(src);
        write(dst, value);
        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::A, const Args::rHL, const std::uint16_t adj)
    {
        const auto hl = read(LR35902::Args::HL{});
        const auto rhl = read(LR35902::Args::rA16{hl});
        write(LR35902::Args::A{}, rhl);
        write(LR35902::Args::HL{}, hl + adj);
        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::rHL, const Args::A, const std::uint16_t adj)
    {
        const auto a = read(LR35902::Args::A{});
        const auto hl = read(LR35902::Args::HL{});
        write(LR35902::Args::rA16{hl}, a);
        write(LR35902::Args::HL{}, hl + adj);
        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::HL, const Args::SP, const Args::S8 off)
    {
        const auto sp = read(LR35902::Args::SP{});
        const auto voff = static_cast<decltype(sp)>(read(off)); // convert to unsigned
        write(LR35902::Args::HL{}, sp + voff);
        const bool H = half_carry(sp, voff);
        const bool C = carry(sp, voff);
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
        const bool Z = value + 1 == 0;
        const bool H = half_carry(value, 1);
        flags("Z0H-"_sc, Z, H);
        return true;
    }

    constexpr bool DEC(const a_common_r16 auto r)
    {
        write(r, read(r) - 1);
        regs.flags("----"_sc);
        return true;
    }

    constexpr bool DEC(const a_common_arg8 auto r)
    {
        using value_t = typename decltype(r)::Type::Type;
        const value_t value = read(r);
        write(r, value - 1);
        const bool Z = value - 1 == 0;
        const bool H = half_carry(value, -1);
        regs.flags("Z1H-"_sc, Z, H);
        return true;
    }

    constexpr bool ADD(const Args::HL a, const a_common_r16 auto b)
    {
        const std::uint16_t av = read(a);
        const std::uint16_t bv = read(b);
        const std::uint16_t v = (av + bv);
        write(a, v);
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        flags("-0HC"_sc, H, C);
        return true;
    }

    constexpr bool ADD(const Args::A a, const a_common_r8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = read(b);
        const std::uint8_t v = (av + bv);
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        flags("Z0HC"_sc, Z, H, C);
        return true;
    }

    constexpr bool ADD(const Args::SP a, const Args::S8 b)
    {
        const std::uint16_t av = read(a);
        const std::int8_t bv = read(b);
        const std::uint16_t v = (av + static_cast<std::uint16_t>(bv));
        write(a, v);
        const bool H = half_carry(av, static_cast<uint16_t>(bv));
        const bool C = carry(av, static_cast<uint16_t>(bv));
        regs.flags(0, 0, H, C);
        return true;
    }

    constexpr bool ADC(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bc = read(b) + read(LR35902::Flags::C{});
        const std::uint8_t v = av + bc;
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bc);
        const bool C = carry(av, bc);
        flags("Z0HC"_sc, Z, H, C);
        return true;
    }

    constexpr bool SUB(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = -to_unsigned(read(b));
        const std::uint8_t v = av + bv;
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        flags("Z1HC"_sc, Z, H, C);
        return true;
    }

    constexpr bool SBC(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = -(to_unsigned(read(b)) + to_unsigned(read(LR35902::Flags::C{})));
        const std::uint8_t v = av + bv;
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        flags("Z1HC"_sc, Z, H, C);
        return true;
    }

    constexpr bool AND(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = read(b);
        const std::uint8_t v = av & bv;
        write(a, v);
        const bool Z = v == 0;
        flags("Z010"_sc, Z);
        return true;
    }

    constexpr bool XOR(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = read(b);
        const std::uint8_t v = av ^ bv;
        write(a, v);
        const bool Z = v == 0;
        flags("Z000"_sc, Z);
        return true;
    }

    constexpr bool OR(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = read(b);
        const std::uint8_t v = av | bv;
        write(a, v);
        const bool Z = v == 0;
        flags("Z000"_sc, Z);
        return true;
    }

    constexpr bool CP(const Args::A a, const a_common_arg8 auto b)
    {
        const std::uint8_t av = read(a);
        const std::uint8_t bv = -to_unsigned(read(b));
        const std::uint8_t v = av + bv;
        write(a, v);
        const bool Z = v == 0;
        const bool H = half_carry(av, bv);
        const bool C = carry(av, bv);
        flags("Z1HC"_sc, Z, H, C);
        return true;
    }

# pragma endregion("MATH")
# pragma region("CONTROL FLOW")

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
        flags("----");
        return success;
    }

    constexpr bool JP(const Args::HL hl)
    {
        write(Args::PC{}, read(hl));
        flags("----");
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
        IME = true;
        write(Args::PC{}, pop());
        flags("----"_sc);
        return true;
    }

    constexpr bool EI()
    {
        IME = true;
        flags("----"_sc);
        return true;
    }

    constexpr bool DI()
    {
        IME = false;
        flags("----"_sc);
        return true;
    }

    constexpr bool DAA()
    {
        std::uint8_t f = regs.template read<LR35902::F>();
        std::uint8_t a = read(Args::A{});
        const bool n = f & LR35902::Flags::N::reg_mask;
        const bool c = f & LR35902::Flags::C::reg_mask;
        const bool h = f & LR35902::Flags::H::reg_mask;
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
        bool C = !n && (c || a > 0x99);
        bool Z = a == 0;
        flags("Z-0C", Z, C);
        return true;
    }




//     constexpr bool STOP()
//     {
//         return true; # TODO
//     }

//     constexpr bool HALT()
//     {
//         return true; # TODO
//     }

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
    constexpr void push(std::uint16_t value)
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
        return (static_cast<std::uint16_t>(msb) << 8) | lsb;
    }

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

    // Read from Register Immediately
    template<Type::a_register Type, LR35902::Access::a_immediate Access>
    constexpr auto read(const Args::Argument<Type, Access> arg) const -> typename Type::Type
    {
        using arg_t = decltype(arg);
        using reg_t = LR35902::Args::to_register_v<arg_t>;
        return regs. template read<reg_t>();
    }

    // Read from Immidiate Immediately
    template<Type::a_immediate Type, Access::a_immediate Access>
    constexpr auto read(const Args::Argument<Type, Access> imm) const -> typename Type::Type
    {
        return imm.type.value;
    }

    // Read Referenced Address
    template<Type::a_arg_type Type, Access::a_reference Access>
    constexpr uint8_t read(const Args::Argument<Type, Access> arg) const
    {
        const auto addr = read(Args::Argument<Type, typename LR35902::Access::Immediate>{arg.value()}) + Access::offset;
        return mmu.read(addr);
    }

    // Write value to Register
    template<Type::a_register Type, Access::a_immediate Access>
    constexpr void write(const Args::Argument<Type, Access> reg, const typename Type::Type value)
    {
        using arg_t = decltype(reg);
        regs.write(typename Args::to_register_v<arg_t>(value));
    }

    // Write value to Memory
    template<Type::a_arg_type Type, Access::a_reference Access>
    constexpr void write(const Args::Argument<Type, Access> arg, const uint8_t value)
    {
        using non_ref_t = Args::Argument<Type, ::LR35902::Access::Immediate>;
        const auto addr = read(non_ref_t{arg.value()}) + Access::offset;
        mmu.write(addr, value);
    }

    template<typename const_string>
    constexpr 
    void flags(const_string str, const bool F1 = false, const bool F2 = false, const bool F3 = false, const bool F4 = false)
    {
        static_assert(str.size() == 4, "Flags need 4 values to describe them");
        static_assert(str[0] == '-' || str[0] == '0' || str[0] == '1' || str[0] == 'Z', "Invalid flag value: str[0]");
        static_assert(str[1] == '-' || str[1] == '0' || str[1] == '1' || str[1] == 'N', "Invalid flag value: str[1]");
        static_assert(str[2] == '-' || str[2] == '0' || str[2] == '1' || str[2] == 'H', "Invalid flag value: str[2]");
        static_assert(str[3] == '-' || str[3] == '0' || str[3] == '1' || str[3] == 'C', "Invalid flag value: str[3]");

        // don't touch flags
        if constexpr (FORCE_READ_WRITE_FLAGS && str == "----"_sc) {
            // force read & write of flags
            auto f = regs. template read<LR35902::F>();
            auto result = LR35902::F{f};
            regs. template write<LR35902::F>(result);
            return;
        }
        else if constexpr (str == "----"_sc) {
            return;
        }

        static constexpr auto mask = [=](const char c) consteval -> std::uint8_t 
        {
            std::uint8_t value = 0;
            for (std::size_t i=0; i<str.size(); ++i)
                if (str[i] == c)
                    value |= 1<<i;
            return value;
        };

        

        std::uint8_t f = regs. template read<LR35902::F>();
        auto result = LR35902::F{f};

        static constexpr auto set_mask = mask('1');
        static constexpr auto change_mask = 0b00001111 & ~mask('-');
        std::uint8_t eval_mask = 0
            & (F1 * 0b0000'0001)
            & (F2 * 0b0000'0010)
            & (F3 * 0b0000'0100)
            & (F4 * 0b0000'1000);

        result.value &= change_mask;  // remove info to be overridden
        result.value |= set_mask;     // add all ones and zeros info
        result.value |= eval_mask;    // set evaluated info
        regs.template write<LR35902::F>(result);
    }
};

}