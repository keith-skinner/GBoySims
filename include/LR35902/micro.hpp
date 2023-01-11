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

constexpr auto carry = 
    [](const auto a, const decltype(a) b, const decltype(a) c = 0) -> bool {
        using I = std::remove_cvref_t<decltype(a)>;
        return bit_carry<I, std::numeric_limits<I>::digits>(a, b, c);
    };

constexpr auto half_carry = 
    [](const auto a, const auto b, const decltype(a) c = 0) -> bool {
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
    template<typename Dst, typename Src>
    constexpr bool LD(const Dst dst, const Src src)
    {
        using type_t = typename decltype(src)::Type::Type;
        const type_t value = read(src);
        write(dst, value);

        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::A, const Args::rHL, const bool inc)
    {
        const auto hl_value = read(LR35902::Args::HL{});
        const auto rhl_value = read(LR35902::Args::rHL{});
        write(LR35902::Args::A{}, rhl_value);

        const auto adj_value = [](const bool inc) { return inc ? +1 : -1; }(inc);
        write(LR35902::Args::HL{}, hl_value + adj_value);
        
        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::rHL, const Args::A, const bool inc)
    {
        const auto a_value = read(LR35902::Args::A{});
        const auto hl_value = read(LR35902::Args::HL{});
        write(LR35902::Args::rHL{}, a_value);

        const auto adj_value = [](const bool inc) { return inc ? +1 : -1; }(inc);
        write(LR35902::Args::HL{}, hl_value + adj_value);

        flags("----"_sc);
        return true;
    }

    constexpr bool LD(const Args::HL dst, const Args::SP src, const Args::S8 off)
    {
        const auto srcv = read(src);
        const auto offv = read(off);
        using uoffv_t = std::make_unsigned_t<decltype(offv)>;
        const auto uoffv = static_cast<uoffv_t>(offv);
        const auto result = srcv + offv;
        write(dst, result);

        const bool H = half_carry(srcv, uoffv);
        const bool C = carry(srcv, uoffv);
        flags("00HC"_sc, H, C);
        return true;
    }
#pragma endregion // LD

#pragma region('MATH')
    // constexpr bool INC(const )

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
    constexpr void flags(const_string str, const bool F1 = false, const bool F2 = false, const bool F3 = false, const bool F4 = false)
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

        // If there's not a '-' then you will be changed and a 0 is set otherwise a 1
        constexpr bool z_change = str[0] != '-';
        constexpr bool n_change = str[1] != '-';
        constexpr bool h_change = str[2] != '-';
        constexpr bool c_change = str[3] != '-';

        constexpr auto change_mask = [=](){
            return  (z_change ? z_reg_mask_t{} : LR35902::Flags::Z::reg_mask)
                |   (n_change ? n_reg_mask_t{} : LR35902::Flags::N::reg_mask)
                |   (h_change ? h_reg_mask_t{} : LR35902::Flags::H::reg_mask)
                |   (c_change ? c_reg_mask_t{} : LR35902::Flags::C::reg_mask);
        }();

        constexpr bool z_set1 = str[0] == '1';
        constexpr bool n_set1 = str[1] == '1';
        constexpr bool h_set1 = str[2] == '1';
        constexpr bool c_set1 = str[3] == '1';

        constexpr auto set1_mask = [=](){
            return  (z_set1 ? LR35902::Flags::Z::reg_mask : z_reg_mask_t{}) 
                |   (n_set1 ? LR35902::Flags::N::reg_mask : n_reg_mask_t{}) 
                |   (h_set1 ? LR35902::Flags::H::reg_mask : h_reg_mask_t{}) 
                |   (c_set1 ? LR35902::Flags::C::reg_mask : c_reg_mask_t{});
        }();

        constexpr bool z_eval = str[0] == 'Z';
        constexpr bool n_eval = str[1] == 'N';
        constexpr bool h_eval = str[2] == 'H';
        constexpr bool c_eval = str[3] == 'C';

        constexpr int z_idx = [=](){ return z_eval ? 0 : -1; }();
        constexpr int n_idx = [=](){ return n_eval ? (z_idx == -1 ? 0 : 1) : -1; }();
        constexpr int h_idx = [=](){ return h_eval ? (
            (z_idx == -1 && n_idx == -1) ? 0 :
            (z_idx == -1 && n_idx ==  0) ? 1 :
            (z_idx ==  0 && n_idx == -1) ? 1 : 2) : -1; 
            }();
        
        constexpr int c_idx = [=](){ return c_eval ? (
            (z_idx == -1 && n_idx == -1 && h_idx == -1) ? 0 :
            (z_idx == -1 && n_idx == -1 && h_idx ==  1) ? 1 :
            (z_idx == -1 && n_idx ==  1 && h_idx == -1) ? 1 :
            (z_idx ==  1 && n_idx == -1 && h_idx == -1) ? 1 :
            (z_idx == -1 && n_idx ==  1 && h_idx ==  1) ? 2 :
            (z_idx ==  1 && n_idx == -1 && h_idx ==  1) ? 2 :
            (z_idx ==  1 && n_idx ==  1 && h_idx == -1) ? 2 : 3): -1;
            }();
        
        const auto FN = [=](int idx){
            switch(idx) {
                case 1: return F1;
                case 2: return F2;
                case 3: return F3;
                case 4: return F4;
                default: break;
            }
            return false;
        };

        const auto eval_mask = [=](){ return 0
            |   (z_eval && FN(z_idx) ? LR35902::Flags::Z::reg_mask : z_reg_mask_t{})
            |   (n_eval && FN(n_idx) ? LR35902::Flags::N::reg_mask : n_reg_mask_t{})
            |   (h_eval && FN(h_idx) ? LR35902::Flags::H::reg_mask : h_reg_mask_t{})
            |   (c_eval && FN(c_idx) ? LR35902::Flags::C::reg_mask : c_reg_mask_t{});
        }();

        auto f = regs. template read<LR35902::F>();
        auto result = LR35902::F{f};
        result.value() &= change_mask;  // remove information off all bits to be changed; implicitly adds all 0 information
        result.value() |= set1_mask;     // add all ones information; implicitly adds all 0 information
        result.value() |= eval_mask;    // adds all evaluated information; implicitly adds all evaluated falses
        regs. template write<LR35902::F>(result);
    }
};

}