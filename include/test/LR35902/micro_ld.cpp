#include <gtest/gtest.h>

#include <LR35902/micro.hpp>
#include <LR35902/mmu.hpp>
#include <LR35902/registers.hpp>
#include <common/utility/bits.hpp>

#include "../validation.hpp"

TEST(LR35902_micro, LD_R8_D8) // Register Immediate
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    EXPECT_TRUE(memory.records.size() == 0);
    EXPECT_TRUE(registers.records.size() == 0);
    
    micro.LD(LR35902::Args::A{}, LR35902::Args::D8{0x88}); //136 

    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 1);

    constexpr auto record0 = RegRecord{s_write(), LR35902::Args::A::name(), 0x88};
    EXPECT_EQ(registers.records[0], record0);
    EXPECT_EQ(registers.get<LR35902::A>(), 0x88);
}

TEST(LR35902_micro, LD_R16_D16) // Register Immediate
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 0);

    micro.LD(LR35902::Args::BC{}, LR35902::Args::D16{0x1616}); //5654

    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 1);

    constexpr auto record0 = RegRecord{s_write(), LR35902::Args::BC::name(), 0x1616};
    EXPECT_EQ(registers.records[0], record0);
    EXPECT_EQ(registers.read<LR35902::BC>(), 0x1616);
}

TEST(LR35902_micro, LD_R8_R8) // Register Register
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::C{0xBE});
    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 0);
    EXPECT_EQ(registers.get<LR35902::C>(), 0xBE);

    micro.LD(LR35902::Args::L{}, LR35902::Args::C{});

    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 2);
    constexpr auto r0 = RegRecord{s_read(), LR35902::Args::C::name(), 0xBE};
    constexpr auto r1 = RegRecord{s_write(), LR35902::Args::L::name(), 0xBE};
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);
    EXPECT_EQ(registers.get<LR35902::L>(), 0xBE);
    EXPECT_EQ(registers.get<LR35902::C>(), 0xBE);
}

TEST(LR35902_micro, LD_R16_R16) // Register Register
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::HL{0x5678});

    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 0);

    micro.LD(LR35902::Args::SP{}, LR35902::Args::HL{}); //22136

    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 2);

    constexpr auto r0 = RegRecord{s_read(), LR35902::HL::name(), 0x5678};
    constexpr auto r1 = RegRecord{s_write(), LR35902::SP::name(), 0x5678};
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);
    EXPECT_EQ(registers.read<LR35902::SP>(), 0x5678);
    EXPECT_EQ(registers.read<LR35902::HL>(), 0x5678);
}

TEST(LR35902_micro, LD_R8_rR16) // Register RegisterReference
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    memory.set(0xABCD, 0xBC);
    registers.set(LR35902::HL{0xABCD});

    micro.LD(LR35902::Args::B{}, LR35902::Args::rHL{});

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 2);
    
    constexpr auto m0 = MemRecord{s_read(), 0xABCD, 0xBC};
    constexpr auto r0 = RegRecord{s_read(), LR35902::Args::HL::name(), 0xABCD};
    constexpr auto r1 = RegRecord{s_write(), LR35902::Args::B::name(), 0xBC};
    
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);

    EXPECT_EQ(registers.get<LR35902::B>(), 0xBC);
    EXPECT_EQ(memory.get(0xABCD), 0xBC);
    EXPECT_EQ(registers.get<LR35902::HL>(), 0xABCD);
}

TEST(LR35902_micro, LD_R8_zR8) // Register RegisterZReference
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    memory.set(0xFFCD, 0xBC);
    registers.set(LR35902::C{0xCD});

    micro.LD(LR35902::Args::A{}, LR35902::Args::zC{});

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 2);
    
    constexpr auto m0 = MemRecord{s_read(), 0xFFCD, 0xBC};
    constexpr auto r0 = RegRecord{s_read(), LR35902::C::name(), 0xCD};
    constexpr auto r1 = RegRecord{s_write(), LR35902::A::name(), 0xBC};
    
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);

    EXPECT_EQ(registers.get<LR35902::A>(), 0xBC);
    EXPECT_EQ(memory.get(0xFFCD), 0xBC);
    EXPECT_EQ(registers.get<LR35902::C>(), 0xCD);
}

TEST(LR35902_micro, LD_R8_rA16) // Register ImmediateReference
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    memory.set(0xFFCD, 0xBC);

    micro.LD(LR35902::Args::A{}, LR35902::Args::zA8{0xCD});

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 1);
    
    constexpr auto m0 = MemRecord{s_read(), 0xFFCD, 0xBC};
    constexpr auto r0 = RegRecord{s_write(), LR35902::A::name(), 0xBC};
    
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);

    // Post Conditions
    EXPECT_EQ(registers.get<LR35902::A>(), 0xBC);
    EXPECT_EQ(memory.get(0xFFCD), 0xBC);
}

TEST(LR35902_micro, LD_R8_zA8) // Register ImmediateZReference
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    memory.set(0xFFCD, 0xBC);

    micro.LD(LR35902::Args::A{}, LR35902::Args::zA8{0xCD});

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 1);
    
    constexpr auto m0 = MemRecord{s_read(), 0xFFCD, 0xBC};
    constexpr auto r0 = RegRecord{s_write(), LR35902::A::name(), 0xBC};
    
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);

    // Post Conditions
    EXPECT_EQ(registers.get<LR35902::A>(), 0xBC);
    EXPECT_EQ(memory.get(0xFFCD), 0xBC);
}

// When the destination is a reference, the source can only be an 8-bit value

TEST(LR35902_micro, LD_zR8_R8) // RegisterZReference Register
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::C{0xCD});
    registers.set(LR35902::A{0xAB});

    micro.LD(LR35902::Args::zC{}, LR35902::Args::A{});

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 2);
    
    constexpr auto m0 = MemRecord{s_write(), 0xFFCD, 0xAB};
    constexpr auto r0 = RegRecord{s_read(), LR35902::A::name(), 0xAB};
    constexpr auto r1 = RegRecord{s_read(), LR35902::C::name(), 0xCD};
    
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);

    EXPECT_EQ(registers.get<LR35902::A>(), 0xAB);
    EXPECT_EQ(registers.get<LR35902::C>(), 0xCD);
    EXPECT_EQ(memory.get(0xFFCD), 0xAB);
}

TEST(LR35902_micro, LD_rR16_R8) // RegisterReference Register
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    // memory is 0 filled at this time
    registers.set(LR35902::D{0x14});
    registers.set(LR35902::HL{0x0120});

    micro.LD(LR35902::Args::rHL{}, LR35902::Args::D{});

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 2);
    
    constexpr auto r0 = RegRecord{s_read(), LR35902::D::name(), 0x14};
    constexpr auto r1 = RegRecord{s_read(), LR35902::HL::name(), 0x0120};
    constexpr auto m0 = MemRecord{s_write(), 0x0120, 0x14};
    
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);

    EXPECT_EQ(registers.get<LR35902::D>(), 0x14);
    EXPECT_EQ(registers.get<LR35902::HL>(), 0x0120);
    EXPECT_EQ(memory.get(0x0120), 0x14);
}

TEST(LR35902_micro, LD_zA8_R8) // ImmediateZReference Register
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::A{0xCD});

    micro.LD(LR35902::Args::zA8{0x34}, LR35902::Args::A{});

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 1);
    constexpr auto r0 = RegRecord{s_read(), LR35902::A::name(), 0xCD};
    constexpr auto m0 = MemRecord{s_write(), 0xFF34, 0xCD};
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.get<LR35902::A>(), 0xCD);
    EXPECT_EQ(memory.get(0xFF34), 0xCD);
}

TEST(LR35902_micro, LD_rA16_R8) // ImmediateReference Register
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::A{0xCD});

    micro.LD(LR35902::Args::rA16{0xBEEF}, LR35902::Args::A{});

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 1);
    constexpr auto r0 = RegRecord{s_read(), LR35902::A::name(), 0xCD};
    constexpr auto m0 = MemRecord{s_write(), 0xBEEF, 0xCD};
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.get<LR35902::A>(), 0xCD);
    EXPECT_EQ(memory.get(0xBEEF), 0xCD);
}

TEST(LR35902_micro, LD_rHL_A_true) // LD rHL+ A
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::HL{0xBEEF});
    registers.set(LR35902::A{0xCD});

    micro.LD(LR35902::Args::rHL{0xBEEF}, LR35902::Args::A{}, LR35902::Args::INC);

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 3);
    constexpr auto m0 = MemRecord{s_write(), 0xBEEF, 0xCD};
    constexpr auto r0 = RegRecord{s_read(), LR35902::A::name(), 0xCD};
    constexpr auto r1 = RegRecord{s_read(), LR35902::HL::name(), 0xBEEF};
    constexpr auto r2 = RegRecord{s_write(), LR35902::HL::name(), 0xBEEF+1};
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);
    EXPECT_EQ(registers.records[2], r2);
    EXPECT_EQ(registers.get<LR35902::A>(), 0xCD);
    EXPECT_EQ(registers.get<LR35902::HL>(), 0xBEEF+1);
    EXPECT_EQ(memory.get(0xBEEF), 0xCD);
}

TEST(LR35902_micro, LD_rHL_A_false) // LD rHL- A
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::HL{0xBEEF});
    registers.set(LR35902::A{0xCD});

    micro.LD(LR35902::Args::rHL{0xBEEF}, LR35902::Args::A{}, LR35902::Args::DEC);

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 3);
    constexpr auto m0 = MemRecord{s_write(), 0xBEEF, 0xCD};
    constexpr auto r0 = RegRecord{s_read(), LR35902::A::name(), 0xCD};
    constexpr auto r1 = RegRecord{s_read(), LR35902::HL::name(), 0xBEEF};
    constexpr auto r2 = RegRecord{s_write(), LR35902::HL::name(), 0xBEEF-1};
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);
    EXPECT_EQ(registers.records[2], r2);
    EXPECT_EQ(registers.get<LR35902::A>(), 0xCD);
    EXPECT_EQ(registers.get<LR35902::HL>(), 0xBEEF-1);
    EXPECT_EQ(memory.get(0xBEEF), 0xCD);
}

TEST(LR35902_micro, LD_A_rHL_true) // LD A rHL+
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    memory.set(0xBEEF, 0xCD);
    registers.set(LR35902::HL{0xBEEF});

    micro.LD(LR35902::Args::A{}, LR35902::Args::rHL{0xBEEF}, LR35902::Args::INC);

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 3);
    constexpr auto m0 = MemRecord{s_read(), 0xBEEF, 0xCD};
    constexpr auto r0 = RegRecord{s_read(), LR35902::HL::name(), 0xBEEF};
    constexpr auto r1 = RegRecord{s_write(), LR35902::A::name(), 0xCD};
    constexpr auto r2 = RegRecord{s_write(), LR35902::HL::name(), 0xBEEF+1};
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);
    EXPECT_EQ(registers.records[2], r2);
    EXPECT_EQ(registers.get<LR35902::A>(), 0xCD);
    EXPECT_EQ(registers.get<LR35902::HL>(), 0xBEEF+1);
    EXPECT_EQ(memory.get(0xBEEF), 0xCD);
}

TEST(LR35902_micro, LD_A_rHL_false) // LD A rHL-
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    memory.set(0xBEEF, 0xCD);
    registers.set(LR35902::HL{0xBEEF});

    micro.LD(LR35902::Args::A{}, LR35902::Args::rHL{0xBEEF}, LR35902::Args::DEC);

    EXPECT_EQ(memory.records.size(), 1);
    EXPECT_EQ(registers.records.size(), 3);
    constexpr auto m0 = MemRecord{s_read(), 0xBEEF, 0xCD};
    constexpr auto r0 = RegRecord{s_read(), LR35902::HL::name(), 0xBEEF};
    constexpr auto r1 = RegRecord{s_write(), LR35902::A::name(), 0xCD};
    constexpr auto r2 = RegRecord{s_write(), LR35902::HL::name(), 0xBEEF-1};
    EXPECT_EQ(memory.records[0], m0);
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);
    EXPECT_EQ(registers.records[2], r2);
    EXPECT_EQ(registers.get<LR35902::A>(), 0xCD);
    EXPECT_EQ(registers.get<LR35902::HL>(), 0xBEEF-1);
    EXPECT_EQ(memory.get(0xBEEF), 0xCD);
}

//TODO: Check boundries around Carry HalfCarry
TEST(LR35902_micro, LD_HL_SP_S8_positive) // LD HL, SP+S8
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::SP{0x1234});

    micro.LD(LR35902::Args::HL{}, LR35902::Args::SP{}, LR35902::Args::S8{+0x34});

    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 4);
    constexpr auto r0 = RegRecord{s_read(), LR35902::SP::name(), 0x1234};
    constexpr auto r1 = RegRecord{s_write(), LR35902::HL::name(), 0x1268};
    constexpr auto r2 = RegRecord{s_read(), LR35902::F::name(), 0b0000'0000};
    constexpr auto r3 = RegRecord{s_write(), LR35902::F::name(), 0b0000'0000};
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);
    EXPECT_EQ(registers.records[2], r2);
    EXPECT_EQ(registers.records[3], r3);
    EXPECT_EQ(registers.get<LR35902::HL>(), 0x1268);
    EXPECT_EQ(registers.get<LR35902::SP>(), 0x1234);
    EXPECT_EQ(registers.get<LR35902::F>(), 0b0000'0000);
}

//TODO: Check boundries around Carry HalfCarry
TEST(LR35902_micro, LD_HL_SP_S8_negative) // LD HL, SP+S8
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    registers.set(LR35902::SP{0x1234});

    const auto s8 = LR35902::Args::S8{-0x34};

    micro.LD(LR35902::Args::HL{}, LR35902::Args::SP{}, s8 );

    EXPECT_EQ(memory.records.size(), 0);
    EXPECT_EQ(registers.records.size(), 4);
    constexpr auto r0 = RegRecord{s_read(), LR35902::SP::name(), 0x1234};
    constexpr auto r1 = RegRecord{s_write(), LR35902::HL::name(), 0x1200};
    constexpr auto r2 = RegRecord{s_read(), LR35902::F::name(), 0b0000'0000};
    constexpr auto r3 = RegRecord{s_write(), LR35902::F::name(), 0b0000'0000};
    EXPECT_EQ(registers.records[0], r0);
    EXPECT_EQ(registers.records[1], r1);
    EXPECT_EQ(registers.records[2], r2);
    EXPECT_EQ(registers.records[3], r3);
    EXPECT_EQ(registers.get<LR35902::HL>(), 0x1200);
    EXPECT_EQ(registers.get<LR35902::SP>(), 0x1234);
    EXPECT_EQ(registers.get<LR35902::F>(), 0b0000'0000);
}

