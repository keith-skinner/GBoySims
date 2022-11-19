#include <gtest/gtest.h>
#include <LR35902/registers.hpp>

TEST(LR35902_registers, RegisterDef)
{
    LR35902::Definitions::AF af;
    EXPECT_TRUE(af.is_valid_register<LR35902::A>());
    EXPECT_TRUE(af.is_valid_register<LR35902::F>());
    EXPECT_TRUE(af.is_valid_register<LR35902::AF>());

    //default initializes to zero
    EXPECT_EQ(0, af.read<LR35902::A>());
    EXPECT_EQ(0, af.read<LR35902::F>());
    EXPECT_EQ(0, af.read<LR35902::AF>());
    EXPECT_EQ(0, af.read<LR35902::Flags::Z>());
    EXPECT_EQ(0, af.read<LR35902::Flags::N>());
    EXPECT_EQ(0, af.read<LR35902::Flags::H>());
    EXPECT_EQ(0, af.read<LR35902::Flags::C>());

    // B is not a SubRegister of RegisterDef AF so it is not a 
    EXPECT_FALSE( (std::is_same_v<LR35902::A, LR35902::B>) );
    EXPECT_FALSE( (std::is_same_v<LR35902::F, LR35902::B>) );
    EXPECT_FALSE( (std::is_same_v<LR35902::AF, LR35902::B>) );
    EXPECT_FALSE( af.is_valid_register<LR35902::B>() );
    // af.write(LR35902::B{4}); // compile error expected because B isn't a registered subregister of AF 
    // AF includes only subregisters AF, A, F, and the ZNHC flags b/c b


    af.write(LR35902::A{5});
    EXPECT_EQ(0x05, af.read<LR35902::A>()); // A is writable
    EXPECT_EQ(0x00, af.read<LR35902::F>()); // F is distinct from A
    EXPECT_EQ(0x0500, af.read<LR35902::AF>()); // A is higher byte of AF

    af.write(LR35902::F{0b1111'0000});
    EXPECT_EQ(0x05, af.read<LR35902::A>()); // A is distinct from F
    EXPECT_EQ(0xF0, af.read<LR35902::F>()); // F is writable
    EXPECT_EQ(0x05F0, af.read<LR35902::AF>()); // F is lower byte of AF

    EXPECT_EQ(true, af.read<LR35902::Flags::Z>()); // Z is bit 7 of AF
    EXPECT_EQ(true, af.read<LR35902::Flags::N>()); // N is bit 6 of AF
    EXPECT_EQ(true, af.read<LR35902::Flags::H>()); // H is bit 5 of AF
    EXPECT_EQ(true, af.read<LR35902::Flags::C>()); // C is bit 4 of AF

    // the flags are individually accessible and affect the F register
    af.write(LR35902::Flags::Z{false});
    af.write(LR35902::Flags::N{true} );
    af.write(LR35902::Flags::H{false});
    af.write(LR35902::Flags::C{true} );
    EXPECT_EQ(0x05, af.read<LR35902::A>()); // A is distinct from flags
    EXPECT_EQ(0b0101'0000, af.read<LR35902::F>()); // F is not distinct from flags
    EXPECT_EQ(0x0500 + 0b0101'0000, af.read<LR35902::AF>()); // AF is not distinct from flags
    EXPECT_EQ(false, af.read<LR35902::Flags::Z>()); // Z is bit 7
    EXPECT_EQ(true,  af.read<LR35902::Flags::N>()); // N is bit 6
    EXPECT_EQ(false, af.read<LR35902::Flags::H>()); // H is bit 5
    EXPECT_EQ(true,  af.read<LR35902::Flags::C>()); // C is bit 4
}

TEST(LR35902_registers, RegisterFile)
{
    [[maybe_unused]]
    LR35902::RegisterFile rf;
    EXPECT_TRUE(rf.is_valid_register<LR35902::A>());
    EXPECT_EQ(0, rf.get_index<LR35902::A>());
    EXPECT_EQ(0, rf.get_index<LR35902::F>());
    EXPECT_EQ(0, rf.get_index<LR35902::AF>());
    EXPECT_EQ(0, rf.get_index<LR35902::Flags::Z>());
    EXPECT_EQ(0, rf.get_index<LR35902::Flags::N>());
    EXPECT_EQ(0, rf.get_index<LR35902::Flags::H>());
    EXPECT_EQ(0, rf.get_index<LR35902::Flags::C>());
    EXPECT_EQ(1, rf.get_index<LR35902::B>());
    EXPECT_EQ(1, rf.get_index<LR35902::C>());
    EXPECT_EQ(1, rf.get_index<LR35902::BC>());
    EXPECT_EQ(2, rf.get_index<LR35902::D>());
    EXPECT_EQ(2, rf.get_index<LR35902::E>());
    EXPECT_EQ(2, rf.get_index<LR35902::DE>());
    EXPECT_EQ(3, rf.get_index<LR35902::H>());
    EXPECT_EQ(3, rf.get_index<LR35902::L>());
    EXPECT_EQ(3, rf.get_index<LR35902::HL>());
    EXPECT_EQ(4, rf.get_index<LR35902::PC>());
    EXPECT_EQ(5, rf.get_index<LR35902::SP>());

    // default initializes to zero
    EXPECT_EQ(0, rf.read<LR35902::A>());
    EXPECT_EQ(0, rf.read<LR35902::F>());
    EXPECT_EQ(0, rf.read<LR35902::AF>());
    EXPECT_EQ(0, rf.read<LR35902::Flags::Z>());
    EXPECT_EQ(0, rf.read<LR35902::Flags::N>());
    EXPECT_EQ(0, rf.read<LR35902::Flags::H>());
    EXPECT_EQ(0, rf.read<LR35902::Flags::C>());

    // A is higher byte
    rf.write(LR35902::A{5});
    EXPECT_EQ(0x05, rf.read<LR35902::A>());
    EXPECT_EQ(0x00, rf.read<LR35902::F>());
    EXPECT_EQ(0x0500, rf.read<LR35902::AF>());

    // F is lower byte
    rf.write(LR35902::F{0b1111'0000});
    EXPECT_EQ(0x05, rf.read<LR35902::A>());
    EXPECT_EQ(0xF0, rf.read<LR35902::F>());
    EXPECT_EQ(0x05F0, rf.read<LR35902::AF>());

    // Check single bit flags
    EXPECT_EQ(true, rf.read<LR35902::Flags::Z>()); // Z is bit 7
    EXPECT_EQ(true, rf.read<LR35902::Flags::N>()); // N is bit 6
    EXPECT_EQ(true, rf.read<LR35902::Flags::H>()); // H is bit 5
    EXPECT_EQ(true, rf.read<LR35902::Flags::C>()); // C is bit 4

    // the flags are individually accessible and affect the F register
    rf.write(LR35902::Flags::Z{false});
    rf.write(LR35902::Flags::H{false});
    EXPECT_EQ(0x05, rf.read<LR35902::A>());
    EXPECT_EQ(0b0101'0000, rf.read<LR35902::F>());
    EXPECT_EQ(0x0500 + 0b0101'0000, rf.read<LR35902::AF>());
    EXPECT_EQ(false, rf.read<LR35902::Flags::Z>()); // Z is bit 7
    EXPECT_EQ(true,  rf.read<LR35902::Flags::N>()); // N is bit 6
    EXPECT_EQ(false, rf.read<LR35902::Flags::H>()); // H is bit 5
    EXPECT_EQ(true,  rf.read<LR35902::Flags::C>()); // C is bit 4


    // Need a new registerfile type with missing registers to check
    // that the registration works

    // write 1 flag
    rf.flags(LR35902::Flags::Z{true});
    EXPECT_EQ(true, rf.read<LR35902::Flags::Z>());

    // write multiple flags
    rf.flags(
        LR35902::Flags::Z{false}, 
        LR35902::Flags::N{false}, 
        LR35902::Flags::H{false}, 
        LR35902::Flags::C{false});
    EXPECT_EQ(false, rf.read<LR35902::Flags::Z>());
    EXPECT_EQ(false, rf.read<LR35902::Flags::N>());
    EXPECT_EQ(false, rf.read<LR35902::Flags::H>());
    EXPECT_EQ(false, rf.read<LR35902::Flags::C>());
}