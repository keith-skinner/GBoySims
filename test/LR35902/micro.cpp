#include <gtest/gtest.h>
#include <LR35902/micro.hpp>
#include <LR35902/registers.hpp>
#include <components/component.hpp>
#include <components/simple.hpp>
#include <components/recorder.hpp>

using namespace LR35902;
using LR35902::micro::Alu;
using LR35902::Registers;

/*
 * OPCODES TO TEST
 00  x01   02   03   04   05  x06   07   08   09   0a   0b   0c   0d  x0e   0f  
 10  x11   12   13   14   15  x16   17   18   19   1a   1b   1c   1d  x1e   1f
 20  x21   22   23   24   25  x26   27   28   29   2a   2b   2c   2d  x2e   2f
 30  x31   32   33   34   35  o36   37   38   39   3a   3b   3c   3d  x3e   3f
x40  x41  x42  x43  x44  x45  o46  x47  x48  x49  x4a  x4b  x4c  x4d  o4e  x4f
x50  x51  x52  x53  x54  x55  o56  x57  x58  x59  x5a  x5b  x5c  x5d  o5e  x5f
x60  x61  x62  x63  x64  x65  o66  x67  x68  x69  x6a  x6b  x6c  x6d  o6e  x6f
o70  o71  o72  o73  o74  o75   76  o77  x78  x79  x7a  x7b  x7c  x7d  o7e  x7f
 80   81   82   83   84   85   86   87   88   89   8a   8b   8c   8d   8e   8f
 90   91   92   93   94   95   96   97   98   99   9a   9b   9c   9d   9e   9f
 a0   a1   a2   a3   a4   a5   a6   a7   a8   a9   aa   ab   ac   ad   ae   af
 b0   b1   b2   b3   b4   b5   b6   b7   b8   b9   ba   bb   bc   bd   be   bf
 c0   c1   c2   c3   c4   c5   c6   c7   c8   c9   ca   cb   cc   cd   ce   cf
 d0   d1   d2   d3   d4   d5   d6   d7   d8   d9   da   db   dc   dd   de   df
 e0   e1   e2   e3   e4   e5   e6   e7   e8   e9   ea   eb   ec   ed   ee   ef
 f0   f1   f2   f3   f4   f5   f6   f7   f8   f9   fa   fb   fc   fd   fe   ff 
 */


GTEST_TEST(ALU, SimpleComponent)
{
    Components::Simple simple_component;

    // Expect simple component to zero out memory on init
    EXPECT_EQ(simple_component.read(0x1234), 0x00);

    // Expect simple component to persist writes
    simple_component.write(0x1234, 0x12);
    EXPECT_EQ(simple_component.read(0x1234), 0x12);
    EXPECT_EQ(simple_component.read(0x1234), 0x12);

    // Expect simple component to overwrite writes
    simple_component.write(0x1234, 0x34);
    EXPECT_EQ(simple_component.read(0x1234), 0x34);
    EXPECT_EQ(simple_component.read(0x1234), 0x34);
    simple_component.write(0x1234, 0x34);
    EXPECT_EQ(simple_component.read(0x1234), 0x34);
    EXPECT_EQ(simple_component.read(0x1234), 0x34);
}

template<typename R16>
void LD_r16_d16(Alu& alu, Registers& regs, R16 r)
{
    // Setup
    using Type = typename R16::Type;
    const uint16_t le0000 = to_little_endian(uint16_t{0x0000u});
    const uint16_t le1234 = to_little_endian(uint16_t{0x1234u});
    EXPECT_EQ(regs.read(Type{}), le0000);
    regs.flags(1, 0, 1, 0);
    
    // Expect register to persist changes
    alu.LD(r, Args::D16{le1234});
    EXPECT_NE(regs.read(Type{}), le0000);
    EXPECT_EQ(regs.read(Type{}), le1234);

    // Expect that LD 0xNNNN does not change flags
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));

    // Expect register to persist changes
    alu.LD(r, Args::D16{le0000});
    EXPECT_NE(regs.read(Type{}), le1234);
    EXPECT_EQ(regs.read(Type{}), le0000);

    // Expect that LD 0x0000 does not change flags
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));
}

GTEST_TEST(ALU, LD_R16_D16)
{
    using namespace Args;
    Registers regs{};
    Components::Recorder recorder;
    Component component{recorder, 0x0000, 0xFFFF};
    Alu alu{regs, component};
    LD_r16_d16(alu, regs, BC{});
    LD_r16_d16(alu, regs, DE{});
    LD_r16_d16(alu, regs, HL{});
    LD_r16_d16(alu, regs, SP{});
    regs.read(BC::Type{});
    
    EXPECT_TRUE(recorder.recordings.empty());
}

template<typename R8>
void LD_r8_d8(Alu& alu, Registers& regs, R8 r)
{
    // Setup
    using Type = typename R8::Type;
    const uint8_t v00 = uint8_t{0x00};
    const uint8_t v12 = uint8_t{0x12};
    EXPECT_EQ(regs.read(Type{}), v00);
    regs.flags(1, 0, 1, 0);

    // Expect register to persist changes after LD
    alu.LD(r, Args::D8{v12});
    EXPECT_NE(regs.read(Type{}), v00);
    EXPECT_EQ(regs.read(Type{}), v12);

    // Expect that LD 0xNN does not change flags
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));

    // Expect register to persist changes after LD
    alu.LD(r, Args::D8{v00});
    EXPECT_NE(regs.read(Type{}), v12);
    EXPECT_EQ(regs.read(Type{}), v00);

    // Expect that LD 0x00 does not change flags
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));
}

GTEST_TEST(ALU, LD_r8_d8)
{
    using namespace Args;
    Registers regs{};
    Components::Recorder recorder;
    Component component{recorder, 0x0000, 0xFFFF};
    Alu alu{regs, component};
    
    LD_r8_d8(alu, regs, B{});
    LD_r8_d8(alu, regs, C{});
    LD_r8_d8(alu, regs, D{});
    LD_r8_d8(alu, regs, E{});
    LD_r8_d8(alu, regs, H{});
    LD_r8_d8(alu, regs, L{});
    LD_r8_d8(alu, regs, A{});

    EXPECT_TRUE(recorder.recordings.empty());
}

template<typename R8A, typename R8B>
void LD_r8_r8(Alu& alu, Registers& regs, R8A a, R8B b)
{
    using TypeA = typename R8A::Type;
    using TypeB = typename R8B::Type;
    
    const uint8_t v00 = uint8_t{0x00};
    const uint8_t v12 = uint8_t{0x12};
    const uint8_t v34 = uint8_t{0x34};
    const uint8_t v56 = uint8_t{0x56};
    const uint8_t v78 = uint8_t{0x78};

    EXPECT_EQ(regs.read(TypeA{}), v00);
    EXPECT_EQ(regs.read(TypeB{}), v00);

    // Set A to 0x12 and B to 0x34
    alu.LD(a, Args::D8{v12});
    alu.LD(b, Args::D8{v34});
    // if same a/b == v34
    regs.flags(1, 0, 1, 0);

    // Expect A to be 0x34 and B to be 0x34 after LD A, B
    alu.LD(a, b);
    EXPECT_NE(regs.read(TypeA{}), v12);
    EXPECT_EQ(regs.read(TypeA{}), v34);
    EXPECT_EQ(regs.read(TypeB{}), v34);

    // Expect flags to not be changed after LD
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));

    // Reset A to 0x12 and B to 0x34
    alu.LD(b, Args::D8{v78});
    alu.LD(a, Args::D8{v56});
    // if same a and b == v56
    regs.flags(0, 1, 0, 1);


    // Expect A to be 0x56 and B to be 0x56 after LD B, A
    alu.LD(b, a);
    EXPECT_NE(regs.read(TypeB{}), v78);
    EXPECT_EQ(regs.read(TypeB{}), v56);
    EXPECT_EQ(regs.read(TypeA{}), v56);

    // Expect flags to not be changed after LD
    EXPECT_FALSE(regs.test(Registers::Flags::Z));
    EXPECT_TRUE(regs.test(Registers::Flags::N));
    EXPECT_FALSE(regs.test(Registers::Flags::H));
    EXPECT_TRUE(regs.test(Registers::Flags::C));

    alu.LD(a, Args::D8{v00});
    alu.LD(b, Args::D8{v00});
}

GTEST_TEST(ALU, LD_r8_r8)
{
    using namespace Args;
    Registers regs{};
    Components::Recorder recorder;
    Component component{recorder, 0x0000, 0xFFFF};
    Alu alu{regs, component};

    LD_r8_r8(alu, regs, B{}, B{});
    LD_r8_r8(alu, regs, B{}, C{});
    LD_r8_r8(alu, regs, B{}, D{});
    LD_r8_r8(alu, regs, B{}, E{});
    LD_r8_r8(alu, regs, B{}, H{});
    LD_r8_r8(alu, regs, B{}, L{});
    LD_r8_r8(alu, regs, B{}, A{});
    
    LD_r8_r8(alu, regs, C{}, B{});
    LD_r8_r8(alu, regs, C{}, C{});
    LD_r8_r8(alu, regs, C{}, D{});
    LD_r8_r8(alu, regs, C{}, E{});
    LD_r8_r8(alu, regs, C{}, H{});
    LD_r8_r8(alu, regs, C{}, L{});
    LD_r8_r8(alu, regs, C{}, A{});

    LD_r8_r8(alu, regs, D{}, B{});
    LD_r8_r8(alu, regs, D{}, C{});
    LD_r8_r8(alu, regs, D{}, D{});
    LD_r8_r8(alu, regs, D{}, E{});
    LD_r8_r8(alu, regs, D{}, H{});
    LD_r8_r8(alu, regs, D{}, L{});
    LD_r8_r8(alu, regs, D{}, A{});

    LD_r8_r8(alu, regs, E{}, B{});
    LD_r8_r8(alu, regs, E{}, C{});
    LD_r8_r8(alu, regs, E{}, D{});
    LD_r8_r8(alu, regs, E{}, E{});
    LD_r8_r8(alu, regs, E{}, H{});
    LD_r8_r8(alu, regs, E{}, L{});
    LD_r8_r8(alu, regs, E{}, A{});

    LD_r8_r8(alu, regs, H{}, B{});
    LD_r8_r8(alu, regs, H{}, C{});
    LD_r8_r8(alu, regs, H{}, D{});
    LD_r8_r8(alu, regs, H{}, E{});
    LD_r8_r8(alu, regs, H{}, H{});
    LD_r8_r8(alu, regs, H{}, L{});
    LD_r8_r8(alu, regs, H{}, A{});

    LD_r8_r8(alu, regs, L{}, B{});
    LD_r8_r8(alu, regs, L{}, C{});
    LD_r8_r8(alu, regs, L{}, D{});
    LD_r8_r8(alu, regs, L{}, E{});
    LD_r8_r8(alu, regs, L{}, H{});
    LD_r8_r8(alu, regs, L{}, L{});
    LD_r8_r8(alu, regs, L{}, A{});

    LD_r8_r8(alu, regs, A{}, B{});
    LD_r8_r8(alu, regs, A{}, C{});
    LD_r8_r8(alu, regs, A{}, D{});
    LD_r8_r8(alu, regs, A{}, E{});
    LD_r8_r8(alu, regs, A{}, H{});
    LD_r8_r8(alu, regs, A{}, L{});
    LD_r8_r8(alu, regs, A{}, A{});

    EXPECT_TRUE(recorder.recordings.empty());
}

template<typename R16>
void INC_r16(Alu& alu, Registers& regs, R16 r)
{
    using Type = typename R16::Type;
    const uint16_t v0000 = uint16_t{0x0000};
    const uint16_t v1234 = uint16_t{0x1234};

    EXPECT_EQ(regs.read(Type{}), v0000);
    regs.flags(1, 0, 1, 0);

    alu.INC(r);
    
}

GTEST_TEST(ALU, INC_r16)
{
    using namespace Args;
    Registers regs{};
    Components::Recorder recorder;
    Component component{recorder, 0x0000, 0xFFFF};
    Alu alu{regs, component};
    INC_r16(BC{});
    INC_r16(DE{});
    INC_r16(HL{});
    INC_r16(SP{});
}

GTEST_TEST(ALU, INC_r8)
{

}

GTEST_TEST(ALU, DEC_r16)
{

}

GTEST_TEST(ALU, DEC_r8)
{

}