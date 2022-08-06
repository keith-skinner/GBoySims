#include <gtest/gtest.h>
#include <LR35902/registers.hpp>
#include <utility/bits.hpp>

template<typename R8>
void access_test(const typename R8::size_t value)
{
    using namespace LR35902;
    using namespace LR35902::Register;
    Registers regs;
    regs.write(R8{}, value);
    EXPECT_EQ(regs.read(R8{}), value);
}

GTEST_TEST(Registers, Register8Access)
{
    using namespace LR35902;
    using namespace LR35902::Register;
    access_test<A>(0x01);
    access_test<F>(0x12);
    access_test<B>(0x23);
    access_test<C>(0x34);
    access_test<D>(0x45);
    access_test<E>(0x56);
    access_test<H>(0x67);
    access_test<L>(0x78);
}

template<typename R16, typename R8L, typename R8H>
void access_test()
{
    using namespace LR35902;
    Registers regs;
    using T = typename R16::size_t;

    // Expect registers to be zero on init
    EXPECT_EQ(regs.read(R16{}), 0x00);
    EXPECT_EQ(regs.read(R8H{}), 0x00);
    EXPECT_EQ(regs.read(R8L{}), 0x00);

    // Expect registers to persist writes
    const T init = to_little_endian(T{0x1234});
    regs.write(R16{}, init);
    EXPECT_NE(regs.read(R16{}), 0x0000);
    EXPECT_EQ(regs.read(R16{}), 0x1234);
    EXPECT_EQ(regs.read(R16{}), 0x1234);

    // Expect registers to overwrite values
    regs.write(R16{}, 0x0000);
    EXPECT_EQ(regs.read(R16{}), 0x0000);
    EXPECT_EQ(regs.read(R8L{}), 0x00);
    EXPECT_EQ(regs.read(R8H{}), 0x00);

    // Expect sub registers to have correct endianess
    regs.write(R16{}, 0x1234);
    EXPECT_EQ(regs.read(R8H{}), 0x12);
    EXPECT_EQ(regs.read(R8L{}), 0x34);
}

GTEST_TEST(Registers, Register16Access)
{
    using namespace LR35902::Register;
    access_test<AF, A, F>();
    access_test<BC, B, C>();
    access_test<DE, D, E>();
    access_test<HL, H, L>();
}

GTEST_TEST(Registers, RegistersTypeTraits)
{
    // int is used here to show that no random type will work and needs to be exactly these types
    using namespace LR35902::Register;
    EXPECT_EQ(is_register_v<A>, true);
    EXPECT_EQ(is_register_v<F>, true);
    EXPECT_EQ(is_register_v<B>, true);
    EXPECT_EQ(is_register_v<C>, true);
    EXPECT_EQ(is_register_v<D>, true);
    EXPECT_EQ(is_register_v<E>, true);
    EXPECT_EQ(is_register_v<H>, true);
    EXPECT_EQ(is_register_v<L>, true);
    EXPECT_EQ(is_register_v<F>, true);
    EXPECT_EQ(is_register_v<AF>, true);
    EXPECT_EQ(is_register_v<BC>, true);
    EXPECT_EQ(is_register_v<DE>, true);
    EXPECT_EQ(is_register_v<HL>, true);
    EXPECT_EQ(is_register_v<int>, false);

    EXPECT_EQ(is_register8_v<A>, true);
    EXPECT_EQ(is_register8_v<F>, true);
    EXPECT_EQ(is_register8_v<B>, true);
    EXPECT_EQ(is_register8_v<C>, true);
    EXPECT_EQ(is_register8_v<D>, true);
    EXPECT_EQ(is_register8_v<E>, true);
    EXPECT_EQ(is_register8_v<H>, true);
    EXPECT_EQ(is_register8_v<L>, true);
    EXPECT_EQ(is_register8_v<F>, true);
    EXPECT_EQ(is_register8_v<AF>, false);
    EXPECT_EQ(is_register8_v<BC>, false);
    EXPECT_EQ(is_register8_v<DE>, false);
    EXPECT_EQ(is_register8_v<HL>, false);
    EXPECT_EQ(is_register8_v<int>, false);

    EXPECT_EQ(is_register16_v<A>, false);
    EXPECT_EQ(is_register16_v<F>, false);
    EXPECT_EQ(is_register16_v<B>, false);
    EXPECT_EQ(is_register16_v<C>, false);
    EXPECT_EQ(is_register16_v<D>, false);
    EXPECT_EQ(is_register16_v<E>, false);
    EXPECT_EQ(is_register16_v<H>, false);
    EXPECT_EQ(is_register16_v<L>, false);
    EXPECT_EQ(is_register16_v<F>, false);
    EXPECT_EQ(is_register16_v<AF>, true);
    EXPECT_EQ(is_register16_v<BC>, true);
    EXPECT_EQ(is_register16_v<DE>, true);
    EXPECT_EQ(is_register16_v<HL>, true);
    EXPECT_EQ(is_register16_v<int>, false);
}

GTEST_TEST(Registers, flags)
{
    using namespace LR35902;
    
    // Expect Flags (Status) register to be 0 on init 
    Registers regs;
    EXPECT_EQ(regs.read(Register::F{}), (0b0000u << 4));
    EXPECT_FALSE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_FALSE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));

    // Expect Flags register to be set to 0xF0 when all flags are set
    regs.flags(1, 1, 1, 1);
    EXPECT_EQ(regs.read(Register::F{}), (0b1111'0000u));
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_TRUE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_TRUE(regs.test(Registers::Flags::C));

    // Expect Flags register to be set to 0x00 when all flags are unset
    regs.flags(0, 0, 0, 0);
    EXPECT_EQ(regs.read(Register::F{}), (0b0000'0000u));
    EXPECT_FALSE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_FALSE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));

    // Expect flags set with true to be set in F register
    regs.flags(true, true, true, true);
    EXPECT_EQ(regs.read(Register::F{}), (0b1111'0000u));
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_TRUE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_TRUE(regs.test(Registers::Flags::C));

    // Expect flags set with flase to be unset in F register
    regs.flags(false, false, false, false);
    EXPECT_EQ(regs.read(Register::F{}), (0b0000'0000u));
    EXPECT_FALSE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_FALSE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));

    // Expect flag values to be independent of each other
    regs.flags(0, 1, 0, 1);
    EXPECT_EQ(regs.read(Register::F{}), (0b0101'0000u));
    EXPECT_FALSE(regs.test(Registers::Flags::Z));
    EXPECT_TRUE(regs.test(Registers::Flags::N));
    EXPECT_FALSE(regs.test(Registers::Flags::H));
    EXPECT_TRUE(regs.test(Registers::Flags::C));
    regs.flags(1, 0, 1, 0);
    EXPECT_EQ(regs.read(Register::F{}), (0b1010'0000u));
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));
    regs.flags(false, true, false, true);
    EXPECT_EQ(regs.read(Register::F{}), (0b0101'0000u));
    EXPECT_FALSE(regs.test(Registers::Flags::Z));
    EXPECT_TRUE(regs.test(Registers::Flags::N));
    EXPECT_FALSE(regs.test(Registers::Flags::H));
    EXPECT_TRUE(regs.test(Registers::Flags::C));
    regs.flags(true, false, true, false);
    EXPECT_EQ(regs.read(Register::F{}), (0b1010'0000u));
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));

    // Expect Flag values to not change when set with '-'
    regs.flags('-', '-', '-', '-');
    EXPECT_EQ(regs.read(Register::F{}), (0b1010'0000u));
    EXPECT_TRUE(regs.test(Registers::Flags::Z));
    EXPECT_FALSE(regs.test(Registers::Flags::N));
    EXPECT_TRUE(regs.test(Registers::Flags::H));
    EXPECT_FALSE(regs.test(Registers::Flags::C));
}