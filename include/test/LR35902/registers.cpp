#include <gtest/gtest.h>
#include <LR35902/registers.hpp>
#include <iostream>

TEST(LR35902_registers, constructable)
{
    [[maybe_unused]]
    LR35902::RegisterFile a;
}