#include <gtest/gtest.h>
#include <LR35902/registers.hpp>
#include <LR35902/utility/bits.hpp>

GTEST_TEST(Registers, Register8Access)
{
    using namespace LR35902;
    Registers regs;
    regs.write(A{}, A::size_t{1});
    EXPECT_EQ(regs.read(A{}), 1);
    regs.write(F{}, F::size_t{2});
    EXPECT_EQ(regs.read(F{}), 2);
    regs.write(B{}, B::size_t{3});
    EXPECT_EQ(regs.read(B{}), 3);
    regs.write(C{}, C::size_t{4});
    EXPECT_EQ(regs.read(C{}), 4);
    regs.write(D{}, D::size_t{5});
    EXPECT_EQ(regs.read(D{}), 5);
    regs.write(E{}, E::size_t{6});
    EXPECT_EQ(regs.read(E{}), 6);
    regs.write(H{}, H::size_t{7});
    EXPECT_EQ(regs.read(H{}), 7);
    regs.write(L{}, L::size_t{8});
    EXPECT_EQ(regs.read(L{}), 8);
}

template<typename R16, typename R8L, typename R8H>
void access_test()
{
    using namespace LR35902;
    Registers regs;
    using T = typename R16::size_t;

    // Can be updated
    const T init = to_little_endian(T{0x1234});
    regs.write(R16{}, init);
    EXPECT_NE(regs.read(R16{}), 0x0000);
    regs.write(R16{}, 0x0000);
    EXPECT_EQ(regs.read(R16{}), 0x0000);
    EXPECT_EQ(regs.read(R8L{}), 0x00);
    EXPECT_EQ(regs.read(R8H{}), 0x00);

    // Updated value is correct
    const T value = to_little_endian(T{0x1234});
    regs.write(R16{}, value);
    const T new_value = regs.read(R16{});
    EXPECT_EQ(new_value, value);

    // Sub registers are correct
    EXPECT_EQ(regs.read(R8H{}), 0x12);
    EXPECT_EQ(regs.read(R8L{}), 0x34);
}

GTEST_TEST(Registers, Register16Access)
{
    using namespace LR35902;
    Registers regs;
    access_test<AF, A, F>();
    access_test<BC, B, C>();
    access_test<DE, D, E>();
    access_test<HL, H, L>();
}