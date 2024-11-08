#include <gtest/gtest.h>
#include <LR35902/registers.hpp>

namespace R {
    using A = LR35902::Register::A;
    using F = LR35902::Register::F;
    using AF = LR35902::Register::AF;
    using B = LR35902::Register::B;
    using C = LR35902::Register::C;
    using BC = LR35902::Register::BC;
    using D = LR35902::Register::D;
    using E = LR35902::Register::E;
    using DE = LR35902::Register::DE;
    using H = LR35902::Register::H;
    using L = LR35902::Register::L;
    using HL = LR35902::Register::HL;

    using PC = LR35902::Register::PC;
    using SP = LR35902::Register::SP;

    using fZ = LR35902::Register::Flags::Z;
    using fN = LR35902::Register::Flags::N;
    using fH = LR35902::Register::Flags::H;
    using fC = LR35902::Register::Flags::C;
};

TEST(LR35902_registers, RegisterDef)
{
    using AFDef = LR35902::Definitions::AF;

    AFDef af;
    EXPECT_TRUE(AFDef::is_registered<R::A>);
    EXPECT_TRUE(AFDef::is_registered<R::F>);
    EXPECT_TRUE(AFDef::is_registered<R::AF>);

    //default initializes to zero
    EXPECT_EQ(0, af.read<R::A>());
    EXPECT_EQ(0, af.read<R::F>());
    EXPECT_EQ(0, af.read<R::AF>());
    EXPECT_EQ(0, af.read<R::fZ>());
    EXPECT_EQ(0, af.read<R::fN>());
    EXPECT_EQ(0, af.read<R::fH>());
    EXPECT_EQ(0, af.read<R::fC>());

    // B is not a SubRegister of RegisterDef AF so it is not a 
    EXPECT_FALSE( (std::is_same_v<R::A, R::B>) );
    EXPECT_FALSE( (std::is_same_v<R::F, R::B>) );
    EXPECT_FALSE( (std::is_same_v<R::AF, R::B>) );
    EXPECT_FALSE( AFDef::is_registered<R::B> );
    // af.write(LR35902::B{4}); // compile error expected because B isn't a registered subregister of AF 
    // AF includes only subregisters AF, A, F, and the ZNHC flags b/c b


    af.write<R::A>(5);
    EXPECT_EQ(0x05, af.read<R::A>()); // A is writable
    EXPECT_EQ(0x00, af.read<R::F>()); // F is distinct from A
    EXPECT_EQ(0x0500, af.read<R::AF>()); // A is higher byte of AF

    af.write<R::F>(0b1111'0000);
    EXPECT_EQ(0x05, af.read<R::A>()); // A is distinct from F
    EXPECT_EQ(0xF0, af.read<R::F>()); // F is writable
    EXPECT_EQ(0x05F0, af.read<R::AF>()); // F is lower byte of AF

    EXPECT_EQ(true, af.read<R::fZ>()); // Z is bit 7 of AF
    EXPECT_EQ(true, af.read<R::fN>()); // N is bit 6 of AF
    EXPECT_EQ(true, af.read<R::fH>()); // H is bit 5 of AF
    EXPECT_EQ(true, af.read<R::fC>()); // C is bit 4 of AF

    // the flags are individually accessible and affect the F register
    af.write<R::fZ>(false);
    af.write<R::fZ>(true);
    af.write<R::fZ>(4);
    af.write<R::fH>(false);
    af.write<R::fC>(true);

    EXPECT_EQ(0x05, af.read<R::A>()); // A is distinct from flags
    EXPECT_EQ(0b0101'0000, af.read<R::F>()); // F is not distinct from flags
    EXPECT_EQ(0x0500 + 0b0101'0000, af.read<R::AF>()); // AF is not distinct from flags
    EXPECT_EQ(false, af.read<R::fZ>()); // Z is bit 7
    EXPECT_EQ(true,  af.read<R::fN>()); // N is bit 6
    EXPECT_EQ(false, af.read<R::fH>()); // H is bit 5
    EXPECT_EQ(true,  af.read<R::fC>()); // C is bit 4
}

TEST(LR35902_registers, RegisterFile)
{
    [[maybe_unused]]
    LR35902::RegisterFile rf;
    EXPECT_TRUE(LR35902::RegisterFile::is_registered<R::A>);
    EXPECT_EQ(0, rf.index_of<R::A>);
    EXPECT_EQ(0, rf.index_of<R::F>);
    EXPECT_EQ(0, rf.index_of<R::AF>);
    EXPECT_EQ(0, rf.index_of<R::fZ>);
    EXPECT_EQ(0, rf.index_of<R::fN>);
    EXPECT_EQ(0, rf.index_of<R::fH>);
    EXPECT_EQ(0, rf.index_of<R::fC>);
    EXPECT_EQ(1, rf.index_of<R::B>);
    EXPECT_EQ(1, rf.index_of<R::C>);
    EXPECT_EQ(1, rf.index_of<R::BC>);
    EXPECT_EQ(2, rf.index_of<R::D>);
    EXPECT_EQ(2, rf.index_of<R::E>);
    EXPECT_EQ(2, rf.index_of<R::DE>);
    EXPECT_EQ(3, rf.index_of<R::H>);
    EXPECT_EQ(3, rf.index_of<R::L>);
    EXPECT_EQ(3, rf.index_of<R::HL>);
    EXPECT_EQ(4, rf.index_of<R::PC>);
    EXPECT_EQ(5, rf.index_of<R::SP>);

    // // default initializes to zero
    // EXPECT_EQ(0, rf.read<R::A>());
    // EXPECT_EQ(0, rf.read<R::F>());
    // EXPECT_EQ(0, rf.read<R::AF>());
    // EXPECT_EQ(0, rf.read<R::fZ>());
    // EXPECT_EQ(0, rf.read<R::fN>());
    // EXPECT_EQ(0, rf.read<R::fH>());
    // EXPECT_EQ(0, rf.read<R::fC>());

    // // A is higher byte
    // rf.write<R::A>(5);
    // EXPECT_EQ(0x05, rf.read<R::A>());
    // EXPECT_EQ(0x00, rf.read<R::F>());
    // EXPECT_EQ(0x0500, rf.read<R::AF>());

    // // F is lower byte
    // rf.write(LR35902::F{0b1111'0000});
    // EXPECT_EQ(0x05, rf.read<R::A>());
    // EXPECT_EQ(0xF0, rf.read<R::F>());
    // EXPECT_EQ(0x05F0, rf.read<R::AF>());

    // // Check single bit flags
    // EXPECT_EQ(true, rf.read<R::fZ>()); // Z is bit 7
    // EXPECT_EQ(true, rf.read<R::fN>()); // N is bit 6
    // EXPECT_EQ(true, rf.read<R::fH>()); // H is bit 5
    // EXPECT_EQ(true, rf.read<R::fC>()); // C is bit 4

    // // the flags are individually accessible and affect the F register
    // rf.write(R::fZ{false});
    // rf.write(R::fH{false});
    // EXPECT_EQ(0x05, rf.read<R::A>());
    // EXPECT_EQ(0b0101'0000, rf.read<R::F>());
    // EXPECT_EQ(0x0500 + 0b0101'0000, rf.read<R::AF>());
    // EXPECT_EQ(false, rf.read<R::fZ>()); // Z is bit 7
    // EXPECT_EQ(true,  rf.read<R::fN>()); // N is bit 6
    // EXPECT_EQ(false, rf.read<R::fH>()); // H is bit 5
    // EXPECT_EQ(true,  rf.read<R::fC>()); // C is bit 4


    // // Need a new registerfile type with missing registers to check
    // // that the registration works

    // // write 1 flag
    // rf.flags(R::fZ{true});
    // EXPECT_EQ(true, rf.read<R::fZ>());

    // // write multiple flags
    // rf.flags(
    //     R::fZ{false}, 
    //     R::fN{false}, 
    //     R::fH{false}, 
    //     R::fC{false});
    // EXPECT_EQ(false, rf.read<R::fZ>());
    // EXPECT_EQ(false, rf.read<R::fN>());
    // EXPECT_EQ(false, rf.read<R::fH>());
    // EXPECT_EQ(false, rf.read<R::fC>());
}