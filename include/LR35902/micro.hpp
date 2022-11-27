#pragma once
#include <memory>
#include <type_traits>
#include <concepts>

#include <sc/string_constant.hpp>

#include <common/memory.hpp>

#include "arguments.hpp"
#include "registers.hpp"
#include "to_register.hpp"
#include "mmu.hpp"

namespace LR35902
{

template<typename T>
constexpr bool is_flag_v =
       std::is_same_v<T, LR35902::Flags::Z>
    || std::is_same_v<T, LR35902::Flags::N>
    || std::is_same_v<T, LR35902::Flags::H>
    || std::is_same_v<T, LR35902::Flags::C>;
template<typename T>
concept a_flag = is_flag_v<T>;

template<typename T, typename ... Ts>
constexpr bool is_member_of_v = (false || ... || std::is_same_v<T, Ts>);
template<typename T, typename ... Ts>
concept a_member_of = is_member_of_v<T, Ts...>;

template<typename T>
constexpr bool is_dst_v = a_member_of<T,
    Args::A, Args::B, Args::C, Args::D, Args::E, /* Args::F, */ Args::H, Args::L, // 8 Bit Args
    Args::AF, Args::BC, Args::DE, Args::HL, Args::SP, // 16 Bit Args
    Args::zC, Args::rBC, Args::rDE, Args::rHL, Args::zA8, Args::rA16>; // Reference args
template<typename T>
concept a_dst = is_dst_v<T>;

template<typename T>
constexpr bool is_src_v = a_member_of<T,
    Args::A, Args::B, Args::C, Args::D, Args::E, Args::H, Args::L, // 8 Bit Registers
    Args::AF, Args::BC, Args::DE, Args::HL, Args::SP, // 16 Bit Registers
    Args::zC, Args::zA8, // 8 Bit References
    Args::rBC, Args::rDE, Args::rHL, Args::rA16, // 16 Bit References
    Args::D8, Args::D16, Args::S8>; // Immidiates
template<typename T>
concept a_src = is_src_v<T>;

template<typename Registers, typename Memory>
class Micro
{
public:
    enum class Jump { Z, NZ, C, NC, None };
private:
    Registers& regs;
    Memory& mmu;
public:

    Micro(Registers& regs, Memory& mmu)
    : regs{regs}, mmu{mmu}
    {}

    constexpr bool NOP()
    {
        flags("----"_sc);
        return true;
    }

#pragma region('LD')
    constexpr bool LD(const a_dst auto dst, const a_src auto src)
    {
        using src_t = typename decltype(src)::Type::Type;
        src_t value = read(src);
        write(dst, value);
        flags("----"_sc);
        return true;
    }

    // constexpr bool LD(const Args::A dst, const Args::rHL src, const bool inc)
    // {
    //     using Args::HL;
    //     LD(dst, src);
    //     const HL::Type::Type offsfet = inc ? +1 : -1;
    //     write(HL{}, read(HL{}) + offset);
    //     regs.flags('-', '-', '-', '-');
    //     return true;
    // }

    // constexpr bool LD(const Args::rHL dst, const Args::A src, const bool inc)
    // {
    //     using Args::HL;
    //     LD(dst, src);
    //     const HL::size_t offset = inc ? +1 : -1;
    //     write(HL{}, read(HL{}) + offset);
    //     regs.flags('-', '-', '-', '-');
    //     return true;
    // }

    // constexpr bool LD(const Args::HL dst, const Args::SP src, const Args::S8 off)
    // {
    //     using T = Args::SP::size_t;
    //     const T srcv = read(src);
    //     const T offv = read(off);
    //     const T adjusted = srcv + offv;
    //     write(dst, adjusted);
    //     const bool H = half_carry(srcv, offv);
    //     const bool C = carry(srcv, offv);
    //     regs.flags(0, 0, H, C);
    //     return true;
    // }
#pragma endregion // LD

    // constexpr bool INC(const Args::Reg16 auto r)
    // {
    //     write(r, read(r) + OFFSET);
    //     regs.flags('-', '-', '-', '-');
    //     return true;
    // }

    // constexpr bool INC(const Args::StdArg auto r)
    // {
    //     const auto new_value = read(r) + OFFSET;
    //     write(r, new_value);
    //     const bool Z = new_value == 0;
    //     const bool H = half_carry(new_value, +OFFSET);
    //     regs.flags(Z, 0, H, '-');
    //     return true;
    // }

//     constexpr bool DEC(const Args::Reg16 auto r)
//     {
//         write(r, read(r) - OFFSET);
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }
//     constexpr bool DEC(const Args::StdArg auto r)
//     {
//         const auto value = read(r);
//         const auto new_value = value - OFFSET;
//         write(r, new_value);
//         const bool Z = new_value == 0;
//         const bool H = half_carry(value, -OFFSET);
//         regs.flags(Z, 1, H, '-');
//         return true;
//     }

//     constexpr bool ADD(const Args::HL a, const Args::Reg16 auto b)
//     {
//         const auto av = read(a);
//         const auto bv = read(b);
//         const auto v = (av + bv);
//         write(a, v);
//         const bool H = half_carry(av, bv);
//         const bool C = carry(av, bv);
//         regs.flags('-', 0, H, C);
//         return true;
//     }
//     constexpr bool ADD(const Args::A a, const Args::StdArgImm auto b)
//     {
//         using T = Args::A::size_t;
//         const T av = read(a);
//         const T bv = read(b);
//         const T v = (av + bv);
//         write(a, v);
//         const bool Z = v == 0;
//         const bool H = half_carry(av, bv);
//         const bool C = carry(av, bv);
//         regs.flags(Z, 0, H, C);
//         return true;
//     }
//     constexpr bool ADD(const Args::SP a, const Args::S8 b)
//     {
//         using T = Args::SP::size_t;
//         const T av = read(a);
//         const T bv = read(b);
//         const T v = (av + bv);
//         write(a, v);
//         const bool H = half_carry(av, static_cast<uint16_t>(bv));
//         const bool C = carry(av, static_cast<uint16_t>(bv));
//         regs.flags(0, 0, H, C);
//         return true;
//     }

//     constexpr bool ADC(const Args::A a, const Args::StdArgImm auto b)
//     {
//         const uint8_t av = read(a);
//         const uint8_t bv = read(b) + regs.test(Registers::Flags::C);
//         const auto v = av + bv;
//         write(a, v);
//         const bool Z = v == 0;
//         const bool H = half_carry(av, bv);
//         const bool C = carry(av, bv);
//         regs.flags(Z, 0, H, C);
//         return true;
//     }

//     constexpr bool SUB(const Args::A a, const Args::StdArgImm auto b)
//     {
//         const uint8_t av = read(a);
//         const uint8_t bv = -read(b);
//         const auto v = av + bv;
//         write(a, v);
//         const bool Z = v == 0;
//         const bool H = half_carry(av, bv);
//         const bool C = carry(av, bv);
//         regs.flags(Z, 1, H, C);
//         return true;
//     }

//     constexpr bool SBC(const Args::A a, const Args::StdArgImm auto b)
//     {
//         const uint8_t av = read(a);
//         const uint8_t bv = -(read(b) + regs.test(Registers::Flags::C));
//         const auto v = av + bv;
//         write(a, v);
//         const bool Z = v == 0;
//         const bool H = half_carry(av, bv);
//         const bool C = carry(av, bv);
//         regs.flags(Z, 1, H, C);
//         return true;
//     }

//     constexpr bool AND(const Args::A a, const Args::StdArgImm auto b)
//     {
//         const uint8_t av = read(a);
//         const uint8_t bv = read(b);
//         const auto v = av & bv;
//         write(a, v);
//         const bool Z = v == 0;
//         regs.flags(Z, 0, 1, 0);
//         return true;
//     }

//     constexpr bool XOR(const Args::A a, const Args::StdArgImm auto b)
//     {
//         const uint8_t av = read(a);
//         const uint8_t bv = read(b);
//         const auto v = av ^ bv;
//         write(a, v);
//         const bool Z = v == 0;
//         regs.flags(Z, 0, 0, 0);
//         return true;
//     }

//     constexpr bool OR(const Args::A a, const Args::StdArgImm auto b)
//     {
//         const uint8_t av = read(a);
//         const uint8_t bv = read(b);
//         const auto v = av | bv;
//         write(a, v);
//         const bool Z = v == 0;
//         regs.flags(Z, 0, 0, 0);
//         return true;
//     }

//     constexpr bool CP(const Args::A a, const Args::StdArgImm auto b)
//     {
//         const uint8_t av = read(a);
//         const uint8_t bv = -read(b);
//         const auto v = av + bv;
//         // Don't write. Only compare
//         const bool Z = v == 0;
//         const bool H = half_carry(av, bv);
//         const bool C = carry(av, bv);
//         regs.flags(Z, 1, H, C);
//         return true;
//     }

//     constexpr bool PUSH(const Args::Reg16 auto src)
//     {
//         push(read(src));
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }

//     constexpr bool POP(const Args::Reg16 auto dst)
//     {
//         write(dst, pop());
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }

//     constexpr bool RST(const Args::A16 addr)
//     {
//         push(read(Args::PC{}));
//         write(Args::PC{}, read(addr));
//         return true;
//     }

//     constexpr bool RET(Jump cond)
//     {
//         bool success = jump(cond);
//         if (success) {
//             write(Args::PC{}, pop());
//         }
//         return success;
//     }

//     constexpr bool CALL(const Args::A16 addr, Jump cond)
//     {
//         bool success = jump(cond);
//         if (success) {
//             push(read(Args::PC{}));
//             write(Args::PC{}, read(addr));
//         }
//         regs.flags('-', '-', '-', '-');
//         return success;
//     }

//     constexpr bool JR(const Args::S8 addr, Jump cond)
//     {
//         bool success = jump(cond);
//         if (success) {
//             write(Args::PC{}, read(Args::PC{}) + read(addr));
//         }
//         regs.flags('-', '-', '-', '-');
//         return success;
//     }

//     constexpr bool JP(const Args::A16 addr, Jump cond)
//     {
//         bool success = jump(cond);
//         if (success) {
//             write(Args::PC{}, read(addr));
//         }
//         regs.flags('-', '-', '-', '-');
//         return success;
//     }

//     constexpr bool JP(const Args::HL hl)
//     {
//         write(Args::PC{}, read(hl));
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }

//     constexpr bool STOP()
//     {
//         return true;
//     }

//     constexpr bool HALT()
//     {
//         return true;
//     }

//     constexpr bool RETI()
//     {
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }

//     constexpr bool DI()
//     {
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }

//     constexpr bool EI()
//     {
//         regs.flags('-', '-', '-', '-');
//         return true;
//     }

//     constexpr bool DAA()
//     {
//         return true;
//     }

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

// private:

//     constexpr void push(const uint8_t value)
//     {
//         const auto spv = read(Args::SP{});
//         write(Args::SP{}, spv - 1);
//         write(Args::rSP{}, value);
//     }
//     constexpr void push(const uint16_t value /*native endian*/)
//     {
//         const uint16_t le_value = to_little_endian(value);
//         push(static_cast<const uint8_t*>(static_cast<const void *>(&le_value))[1]);
//         push(static_cast<const uint8_t*>(static_cast<const void *>(&le_value))[0]);
//     }

//     constexpr uint8_t pop8()
//     {
//         data_t value = read(Args::rSP{});
//         const auto spv = read(Args::SP{});
//         write(Args::SP{}, spv+1);
//         return value;
//     }
//     constexpr uint16_t /*native endian*/ pop()
//     {
//         uint16_t le_value;
//         reinterpret_cast<uint8_t*>(le_value)[0] = pop8();
//         reinterpret_cast<uint8_t*>(le_value)[1] = pop8();
//         return to_native_endian(le_value);
//     }

//     constexpr bool jump(Jump cond) const
//     {
//         switch( cond ) {
//             case Jump::Z: return regs.test(Registers::Flags::Z);
//             case Jump::NZ: return !regs.test(Registers::Flags::Z);
//             case Jump::C: return regs.test(Registers::Flags::C);
//             case Jump::NC: return !regs.test(Registers::Flags::C);
//             case Jump::None: return true;
//             default: return false;
//         }
//         return false;
//     }

//     constexpr uint8_t bit(int loc, uint8_t value, int end = 0)
//     {
//         return ((value & (1 << loc)) >> loc) << end;
//     }

    // Read from Register Immediately
    template<Type::a_register Type, LR35902::Access::a_immediate Access>
    constexpr auto read(const Args::Argument<Type, Access> arg) const -> typename Type::Type
    {
        using arg_t = decltype(arg);
        using reg_t = to_register_t<arg_t>;
        return regs. template read<reg_t>();
    }

    // Read from Immidiate Immediately
    template<Type::a_immediate Type, Access::a_immediate Access>
    constexpr auto read(const Args::Argument<Type, Access> imm) const -> typename Type::Type
    {
        return imm.type.value;
    }

    // Read Address Referenced by T
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
        regs.write(to_register_t<arg_t>{value});
    }

    // Write value to Memory
    template<Type::a_arg_type Type, Access::a_reference Access, typename Type::Type>
    constexpr void write(const Args::Argument<Type, Access> arg, const uint8_t value)
    {
        const auto addr = read(Args::Argument<Type, typename Access::Immediate>{arg.value()}) + Access::offset;
        mmu.write(addr, value);
    }


    template<typename const_string>
    constexpr void flags(const_string str, bool F1 = false, bool F2 = false, bool F3 = false, bool F4 = false)
    {
        static_assert(str.size() == 4, "Flags need 4 values to describe them");
        static_assert(str[0] == '-' || str[0] == '0' || str[0] == '1' || str[0] == 'Z', "Invalid flag value: str[0]");
        static_assert(str[1] == '-' || str[1] == '0' || str[1] == '1' || str[1] == 'N', "Invalid flag value: str[1]");
        static_assert(str[2] == '-' || str[2] == '0' || str[2] == '1' || str[2] == 'H', "Invalid flag value: str[2]");
        static_assert(str[3] == '-' || str[3] == '0' || str[3] == '1' || str[3] == 'C', "Invalid flag value: str[3]");

        // don't touch flags
        if constexpr (str == "----"_sc)
            return;

        using z_reg_mask_t = decltype(LR35902::Flags::Z::reg_mask);
        using n_reg_mask_t = decltype(LR35902::Flags::N::reg_mask);
        using h_reg_mask_t = decltype(LR35902::Flags::H::reg_mask);
        using c_reg_mask_t = decltype(LR35902::Flags::C::reg_mask);

        constexpr bool zset = str[0] == 'Z';
        constexpr bool nset = str[1] == 'N';
        constexpr bool hset = str[2] == 'H';
        constexpr bool cset = str[3] == 'C';

        constexpr bool z = zset && F1;
        constexpr bool n = nset &&
                      ((zset && F2)
                    || F1);
        constexpr bool h = hset &&
                      (( zset &&  nset && F3)
                    || (!zset &&  nset && F2)
                    || ( zset && !nset && F2)
                    || F1);
        constexpr bool c = cset &&
                      (( zset &&  nset &&  hset && F4)
                    || (!zset &&  nset &&  hset && F3)
                    || ( zset && !nset &&  hset && F3)
                    || ( zset &&  nset && !hset && F3)
                    || (!zset && !nset &&  hset && F2)
                    || (!zset &&  nset && !hset && F2)
                    || ( zset && !nset && !hset && F2)
                    || F1);

        constexpr auto change_mask =
             [](const auto str){ if constexpr (str[0] == '-') return z_reg_mask_t{}; else return LR35902::Flags::Z::reg_mask; }(str)
            &[](const auto str){ if constexpr (str[1] == '-') return n_reg_mask_t{}; else return LR35902::Flags::N::reg_mask; }(str)
            &[](const auto str){ if constexpr (str[2] == '-') return h_reg_mask_t{}; else return LR35902::Flags::H::reg_mask; }(str)
            &[](const auto str){ if constexpr (str[3] == '-') return c_reg_mask_t{}; else return LR35902::Flags::C::reg_mask; }(str);

        constexpr auto set_mask =
             [](const auto str){ if constexpr (str[0] == '1') return LR35902::Flags::Z::reg_mask; else return z_reg_mask_t{}; }(str)
            &[](const auto str){ if constexpr (str[1] == '1') return LR35902::Flags::N::reg_mask; else return n_reg_mask_t{}; }(str)
            &[](const auto str){ if constexpr (str[2] == '1') return LR35902::Flags::H::reg_mask; else return h_reg_mask_t{}; }(str)
            &[](const auto str){ if constexpr (str[3] == '1') return LR35902::Flags::C::reg_mask; else return c_reg_mask_t{}; }(str);

        auto eval_mask =
             [=](){ if constexpr (z) return LR35902::Flags::Z::reg_mask; else return z_reg_mask_t{}; }()
            &[=](){ if constexpr (n) return LR35902::Flags::N::reg_mask; else return n_reg_mask_t{}; }()
            &[=](){ if constexpr (h) return LR35902::Flags::H::reg_mask; else return h_reg_mask_t{}; }()
            &[=](){ if constexpr (c) return LR35902::Flags::C::reg_mask; else return c_reg_mask_t{}; }();

        auto f = regs. template read<LR35902::F>();
        auto result = LR35902::F{f};
        result.value() &= ~change_mask; // remove information off all bits to be changed
        result.value() |= set_mask;     // add all ones information; implicitly adds all 0 information
        result.value() |= eval_mask;    // adds all evaluated information; implicitly adds all evaluated falses
        regs. template write<LR35902::F>(result);
    }
};

}